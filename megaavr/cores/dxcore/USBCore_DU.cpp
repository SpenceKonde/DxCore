/**
 * USBCore_DU.cpp - PluggableUSB bridge for the AVR DU native-USB stack.
 *
 * Provides:
 *   - PluggableUSB_::PluggableUSB_()    (the API declares it, core must define it)
 *   - epBuffer()                        (per-EP type storage queried by plug())
 *   - USB_SendControl / USB_Send / ... (the USBAPI consumed by HID & friends)
 *   - usbcore_*() helpers called from usb_standard.c when handling SETUP
 *
 * Design notes:
 *   * Endpoint allocation:  EP0=control, EP1..3=CDC (notify/RX/TX),
 *     EP4..7=dynamic PluggableUSB. totalEP=8 slots, plug() refuses any
 *     module that would push lastEp past it.
 *   * USB_SendControl appends to a 256B static accumulator. The caller in
 *     usb_standard.c (after PluggableUSB().setup() / getInterface() returns)
 *     emits the accumulator via the existing ep0_start_data_in() multi-packet
 *     control-IN path. No change to the EP0 state machine itself.
 *   * USB_Send (HID reports) appends to a per-EP staging buffer; when the
 *     caller sets TRANSFER_RELEASE we move the bytes into the EP DATAPTR,
 *     arm the IN, and busy-wait for TRNCOMPL. Reports are small (HID = 8B
 *     typical) and only sent at host poll cadence, so the blocking cost is
 *     bounded and matches the standard Arduino HID semantics.
 */
#include <avr/io.h>
#if defined(USB0)

#include <Arduino.h>
#include <string.h>
#include <avr/pgmspace.h>

extern "C" {
  #include "usb_core.h"
  #include "usb_descriptors.h"
}
#include "USBCore_DU.h"
#include "usb_ep_types.h"

#include "api/USBAPI.h"
#include "api/PluggableUSB.h"

/* ============================================================
 *  Constants
 * ============================================================ */
#define USBCORE_NUM_EP        8                 /* EP0..EP7 (logical max)    */
#define USBCORE_DYN_EP_BASE   4                 /* first plugged EP          */
#define USBCORE_DYN_EP_COUNT  (USBCORE_NUM_EP - USBCORE_DYN_EP_BASE)
#define USBCORE_CDC_LAST_EP   3                 /* CDC owns EP1..EP3       */
#define USBCORE_CDC_NUM_IF    2                 /* CDC owns IF0..IF1       */
#define USBCORE_ACC_SIZE      192       /* CDC IAD+CDC*2+HID ~= 100 B, with headroom */
#define USBCORE_DYN_EP_BUF    USB_EP_SIZE       /* per-dynamic-EP staging  */

/* ============================================================
 *  Accumulator buffer for USB_SendControl
 * ============================================================ */
static uint8_t   s_acc[USBCORE_ACC_SIZE];
static uint16_t  s_acc_pos = 0;

void usbcore_acc_reset(void)      { s_acc_pos = 0; }
const uint8_t *usbcore_acc_buf(void) { return s_acc; }
uint16_t       usbcore_acc_len(void) { return s_acc_pos; }

/* Stage `n` bytes from a PROGMEM source into s_acc at the current write
 * position.  Used by usb_standard.c (which is C, hence cannot call the
 * C++ USB_SendControl directly) to ship device/string descriptors that
 * now live in flash.  Truncates silently if s_acc would overflow.       */
void usbcore_acc_load_P(const uint8_t *src_pgm, uint16_t n) {
    uint16_t room = USBCORE_ACC_SIZE - s_acc_pos;
    if (room == 0) return;
    if (n > room) n = room;
    memcpy_P(&s_acc[s_acc_pos], src_pgm, n);
    s_acc_pos += n;
}

/* ============================================================
 *  EP-type storage queried by PluggableUSB::plug()
 *
 *  The framework writes `node->endpointType[i]` as `unsigned int`
 *  to *(unsigned int *)epBuffer(lastEp), so we hand back a pointer
 *  to a uint16_t per slot. CDC slots (1..3) are pre-filled with
 *  EP_TYPE_INTERRUPT_IN / EP_TYPE_BULK_OUT / EP_TYPE_BULK_IN so a
 *  scan over the whole table can apply CDC and dynamic EPs uniformly
 *  if ever needed.
 * ============================================================ */
static uint8_t s_ep_types[USBCORE_NUM_EP] = {
    EP_TYPE_CONTROL,         /* EP0 control            */
    EP_TYPE_INTERRUPT_IN,    /* EP1 CDC notify         */
    EP_TYPE_BULK_OUT,        /* EP2 CDC data RX        */
    EP_TYPE_BULK_IN,         /* EP3 CDC data TX        */
    0, 0, 0, 0               /* EP4..EP7 dynamic       */
};

void *epBuffer(unsigned int n) {
    if (n >= USBCORE_NUM_EP) return NULL;
    return &s_ep_types[n];
}

/* ============================================================
 *  PluggableUSB_ constructor  (the API only declares it)
 *
 *  CDC occupies IF0..1 and EP1..3, so plugged modules start at
 *  lastIf = 2, lastEp = 4. totalEP is the per-class member that
 *  plug() compares against (lastEp + numEndpoints > totalEP rejects).
 * ============================================================ */
PluggableUSB_::PluggableUSB_() :
    lastIf(USBCORE_CDC_NUM_IF),
    lastEp(USBCORE_CDC_LAST_EP + 1),
    rootNode(NULL),
    totalEP(USBCORE_NUM_EP - 1)   /* highest EP number usable = 7 */
{
}

/* ============================================================
 *  USBSetup conversion (our packed usb_setup_t -> framework type)
 * ============================================================ */
static inline USBSetup as_usbsetup(const usb_setup_t *s) {
    USBSetup u;
    u.bmRequestType = s->bmRequestType;
    u.bRequest      = s->bRequest;
    u.wValueL       = (uint8_t)(s->wValue & 0xFF);
    u.wValueH       = (uint8_t)((s->wValue >> 8) & 0xFF);
    u.wIndex        = s->wIndex;
    u.wLength       = s->wLength;
    return u;
}

/* ============================================================
 *  USBAPI - control transfers
 * ============================================================ */
int USB_SendControl(uint8_t flags, const void* d, int len) {
    if (len <= 0) return 0;
    int room = USBCORE_ACC_SIZE - s_acc_pos;
    if (room <= 0) return -1;
    int n = (len > room) ? room : len;
    if (flags & TRANSFER_PGM) {
        memcpy_P(&s_acc[s_acc_pos], d, n);
    } else {
        memcpy(&s_acc[s_acc_pos], d, n);
    }
    s_acc_pos += n;
    return n;
}

/* ============================================================
 *  Control-OUT data stage staging (plugged / HID host->device)
 *
 *  Flow (see usb_standard.c usb_handle_class_request):
 *    SETUP(host->device, wLength>0, non-CDC IF)
 *      -> usbcore_ctrl_out_begin() saves the SETUP and returns the length
 *         to arm; usb_standard.c arms EP0 OUT at usbcore_ctrl_out_buf().
 *    EP0 OUT data lands -> handle_ep0_out_complete()
 *      -> usb_class_data_out_complete() -> usbcore_ctrl_out_dispatch()
 *         re-runs the owning module's setup(), which reads the staged
 *         bytes synchronously via USB_RecvControl().
 *  Only one control transfer is in flight at a time, so a single set of
 *  static state is sufficient (and is mutually exclusive with the CDC
 *  SET_LINE_CODING path).
 * ============================================================ */
static usb_setup_t s_ctrl_out_setup;             /* SETUP to re-dispatch        */
static uint8_t     s_ctrl_out_buf[USB_EP_SIZE];  /* EP0 OUT landing buffer      */
static uint16_t    s_ctrl_out_len     = 0;       /* bytes the host will send    */
static uint16_t    s_ctrl_out_pos     = 0;       /* USB_RecvControl read cursor */
static bool        s_ctrl_out_pending = false;

uint16_t usbcore_ctrl_out_begin(const usb_setup_t *s) {
    s_ctrl_out_setup = *s;
    uint16_t n = s->wLength;
    if (n > sizeof(s_ctrl_out_buf)) n = sizeof(s_ctrl_out_buf); /* single-packet cap */
    s_ctrl_out_len     = n;
    s_ctrl_out_pos     = 0;
    s_ctrl_out_pending = true;
    return n;
}

uint8_t *usbcore_ctrl_out_buf(void)     { return s_ctrl_out_buf; }
bool     usbcore_ctrl_out_pending(void) { return s_ctrl_out_pending; }

void usbcore_ctrl_out_dispatch(void) {
    /* EP0 OUT has filled s_ctrl_out_buf. Hand it to the owning module: its
     * setup() calls USB_RecvControl() below, which now returns the staged
     * bytes. The status-stage ZLP is issued by handle_ep0_out_complete()
     * after we return. */
    s_ctrl_out_pending = false;
    s_ctrl_out_pos     = 0;
    USBSetup u = as_usbsetup(&s_ctrl_out_setup);
    PluggableUSB().setup(u);
}

/* SET_REPORT / HID feature report: return bytes staged by the EP0 OUT data
 * stage (see above). Sequential reads, like the Arduino AVR core. */
int USB_RecvControl(void* d, int len) {
    if (len <= 0) return 0;
    uint16_t avail = s_ctrl_out_len - s_ctrl_out_pos;
    uint16_t n = ((uint16_t)len > avail) ? avail : (uint16_t)len;
    memcpy(d, &s_ctrl_out_buf[s_ctrl_out_pos], n);
    s_ctrl_out_pos += n;
    return (int)n;
}
int USB_RecvControlLong(void* d, int len) { return USB_RecvControl(d, len); }

/* ============================================================
 *  Dynamic-EP staging buffers and USBAPI
 * ============================================================ */
/* Staging buffers only allocated for plugged EPs (EP_BASE..EP_BASE+COUNT-1).
 * Indexed by (ep - USBCORE_DYN_EP_BASE).  EP0..EP3 are owned by CDC and use
 * usb_core's g_ep0..g_ep3 buffers directly, so no staging is needed here. */
static uint8_t  s_dyn_ep_buf[USBCORE_DYN_EP_COUNT][USBCORE_DYN_EP_BUF];
static uint8_t  s_dyn_ep_pos[USBCORE_DYN_EP_COUNT];

/* Per dynamic OUT (host->device) endpoint receive ring. The TRNCOMPL ISR
 * copies each received packet out of the endpoint buffer (s_dyn_ep_buf[idx],
 * which is the OUT EP's DATAPTR) into this ring and immediately re-arms the
 * endpoint; USB_Recv()/USB_Available() consume it from main context. Single
 * producer (ISR) / single consumer (main) with uint8_t head/tail = lock-free
 * on this 8-bit core (same pattern as the CDC RX ring). */
#define USBCORE_DYN_RX_RING   64    /* >= one max packet; tune to RAM budget */
static uint8_t          s_dyn_rx_ring[USBCORE_DYN_EP_COUNT][USBCORE_DYN_RX_RING];
static volatile uint8_t s_dyn_rx_head[USBCORE_DYN_EP_COUNT];  /* written by ISR  */
static volatile uint8_t s_dyn_rx_tail[USBCORE_DYN_EP_COUNT];  /* written by main */

/* Forward declaration: dyn_ep_release() is defined further down, but
 * USB_Flush() below needs it to push a staged short packet. */
static int dyn_ep_release(uint8_t ep);

uint8_t USB_Available(uint8_t ep) {
    ep &= 0x07;
    if (ep < USBCORE_DYN_EP_BASE || ep >= USBCORE_NUM_EP) return 0;
    uint8_t i = ep - USBCORE_DYN_EP_BASE;
    int16_t n = (int16_t)s_dyn_rx_head[i] - (int16_t)s_dyn_rx_tail[i];
    if (n < 0) n += USBCORE_DYN_RX_RING;
    return (uint8_t)n;
}

/* TBD when an OUT EP is plugged */
uint8_t USB_SendSpace(uint8_t ep)      {
    ep &= 0x07;
    if (ep < USBCORE_DYN_EP_BASE || ep >= USBCORE_NUM_EP) return 0;
    return USBCORE_DYN_EP_BUF - s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE];
}

int USB_Recv(uint8_t ep, void* data, int len) {
    ep &= 0x07;
    if (ep < USBCORE_DYN_EP_BASE || ep >= USBCORE_NUM_EP) return -1;
    if (len <= 0) return 0;
    uint8_t  i   = ep - USBCORE_DYN_EP_BASE;
    uint8_t *dst = (uint8_t *)data;
    int got = 0;
    while (got < len && s_dyn_rx_tail[i] != s_dyn_rx_head[i]) {
        dst[got++] = s_dyn_rx_ring[i][s_dyn_rx_tail[i]];
        s_dyn_rx_tail[i] = (uint8_t)((s_dyn_rx_tail[i] + 1) % USBCORE_DYN_RX_RING);
    }
    return got;
}
int USB_Recv(uint8_t ep) {
    uint8_t b;
    return (USB_Recv(ep, &b, 1) == 1) ? (int)b : -1;
}

/* Push whatever is staged on a dynamic IN endpoint out to the bus. The
 * Arduino USBAPI lets a library stage payload with USB_Send() (without
 * TRANSFER_RELEASE) and then emit a sub-maxpacket packet with USB_Flush();
 * MIDIUSB sends every event this way. HID never does - it ORs
 * TRANSFER_RELEASE into each report - so USB_Flush() is first exercised here. */
void    USB_Flush(uint8_t ep) {
    ep &= 0x07;
    if (ep < USBCORE_DYN_EP_BASE || ep >= USBCORE_NUM_EP) return;
    if (s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE] > 0) {
        dyn_ep_release(ep);
    }
}

/* Issue an IN packet on a dynamic EP from its staging buffer.
 * Blocking - waits for HW to be idle, sets CNT/clears BUSNAK, then waits
 * for TRNCOMPL. Times out (returns) if the host never IN-polls; HID will
 * just retry next report. */
static int dyn_ep_release(uint8_t ep) {
    if (ep < 4 || ep >= USBCORE_NUM_EP) return -1;

    /* Wait for HW idle (BUSNAK = 1 means no packet armed). */
    uint16_t tmo = 0;
    while (!(g_ep_table.EP[ep].IN.STATUS & USB_BUSNAK_bm)) {
        if (++tmo == 0) return -1;
    }

    /* Arm the packet. */
    g_ep_table.EP[ep].IN.DATAPTR = (uint16_t)s_dyn_ep_buf[ep - USBCORE_DYN_EP_BASE];
    g_ep_table.EP[ep].IN.CNT     = s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE];
    /* Activate: clear BUSNAK (+ stale flags) via INCLR, but PRESERVE TOGGLE so
     * the hardware keeps DATA0/DATA1 in sync with the host (see usb_core.c). */
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
    USB0.STATUS[ep].INCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    /* Wait for completion. */
    tmo = 0;
    while (!(g_ep_table.EP[ep].IN.STATUS & USB_TRNCOMPL_bm)) {
        if (++tmo == 0) { s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE] = 0; return -1; }
    }
    /* Clear TRNCOMPL only; the hardware re-sets BUSNAK after the IN completes.
     * Never write STATUS directly here - it would clear TOGGLE. */
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
    USB0.STATUS[ep].INCLR = USB_TRNCOMPL_bm;

    int sent = s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE];
    s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE] = 0;
    return sent;
}

int USB_Send(uint8_t ep_with_flags, const void* data, int len) {
    uint8_t ep    = ep_with_flags & 0x07;
    uint8_t flags = ep_with_flags & 0xF8;
    if (ep < 4 || ep >= USBCORE_NUM_EP) return -1;
    if (len < 0) return -1;

    /* Append payload to the staging buffer. */
    int total = 0;
    const uint8_t *src = (const uint8_t *)data;
    while (len > 0) {
        int room = USBCORE_DYN_EP_BUF - s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE];
        if (room == 0) {
            /* Flush a full packet, then keep going. */
            int r = dyn_ep_release(ep);
            if (r < 0) return -1;
            room = USBCORE_DYN_EP_BUF;
        }
        int n = (len > room) ? room : len;
        if (flags & TRANSFER_PGM) {
            memcpy_P(&s_dyn_ep_buf[ep - USBCORE_DYN_EP_BASE][s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE]], src, n);
        } else {
            memcpy(&s_dyn_ep_buf[ep - USBCORE_DYN_EP_BASE][s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE]], src, n);
        }
        s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE] += n;
        src   += n;
        len   -= n;
        total += n;
    }

    if (flags & TRANSFER_RELEASE) {
        int r = dyn_ep_release(ep);
        if (r < 0) return -1;
    }
    return total;
}

/* ============================================================
 *  Dynamic config descriptor builder  -  emits into accumulator
 * ============================================================ */
/* Emit the 66-byte CDC block (IAD + IF0 + IF1 + 3 EPs) by slicing it out
 * of the existing g_config_descriptor (bytes 9..74). Sharing the single
 * source of truth means we cannot drift between the static and dynamic
 * representations of CDC. */
static void emit_cdc_interfaces(uint8_t *ifCount) {
    USB_SendControl(TRANSFER_PGM, &g_config_descriptor[9], 66);
    *ifCount += 2;
}

void usbcore_build_config_descriptor(void) {
    s_acc_pos = 0;

    /* Skeleton CONFIGURATION header (wTotalLength + bNumInterfaces patched later) */
    static const uint8_t cfg_hdr[9] PROGMEM = {
        9,            /* bLength                                */
        0x02,         /* bDescriptorType = CONFIGURATION        */
        0, 0,         /* wTotalLength    (patched below)        */
        0,            /* bNumInterfaces  (patched below)        */
        1,            /* bConfigurationValue                    */
        0,            /* iConfiguration                         */
        0xA0,         /* bmAttributes: bus-powered, rem.wakeup  */
        50            /* bMaxPower = 100 mA                     */
    };
    USB_SendControl(TRANSFER_PGM, cfg_hdr, sizeof(cfg_hdr));

    uint8_t ifCount = 0;
    emit_cdc_interfaces(&ifCount);
    PluggableUSB().getInterface(&ifCount);

    /* Patch wTotalLength and bNumInterfaces. */
    s_acc[2] = (uint8_t)(s_acc_pos & 0xFF);
    s_acc[3] = (uint8_t)((s_acc_pos >> 8) & 0xFF);
    s_acc[4] = ifCount;
}

/* ============================================================
 *  Dispatch helpers (called from usb_standard.c)
 * ============================================================ */
bool usbcore_try_plugged_setup(const usb_setup_t *s) {
    USBSetup u = as_usbsetup(s);
    return PluggableUSB().setup(u);
}

bool usbcore_try_plugged_get_descriptor(const usb_setup_t *s) {
    USBSetup u = as_usbsetup(s);
    return PluggableUSB().getDescriptor(u) > 0;
}

/* ============================================================
 *  SET_CONFIGURATION : program dynamic EPs from epBuffer types
 *  (CDC EPs are programmed inline in handle_set_configuration.)
 * ============================================================ */
void usbcore_init_plugged_endpoints(void) {
    for (uint8_t ep = 4; ep < USBCORE_NUM_EP; ep++) {
        uint8_t t = s_ep_types[ep];
        if (t == 0) continue;       /* slot unused */

        uint8_t  dir_in  = (t & 0x80) ? 1 : 0;
        /* DU TYPE: control / iso / bulkint (bulk and interrupt are the same
         * register value; the descriptor type bits 1..0 distinguish them). */
        uint8_t  type    = (t & 0x03);
        uint8_t  ctrl_tp = (type == 0) ? USB_TYPE_CONTROL_gc
                         : (type == 1) ? USB_TYPE_ISO_gc
                         :               USB_TYPE_BULKINT_gc;

        /* Per-EP buffer size selector. We use 64-byte buffers for all
         * dynamic EPs; HID/Keyboard/Mouse only send a handful per report. */
        uint8_t  bufsz   = USB_BUFSIZE_DEFAULT_BUF64_gc;

        s_dyn_ep_pos[ep - USBCORE_DYN_EP_BASE] = 0;
        if (dir_in) {
            g_ep_table.EP[ep].IN.CTRL    = ctrl_tp | bufsz;
            g_ep_table.EP[ep].IN.DATAPTR = (uint16_t)s_dyn_ep_buf[ep - USBCORE_DYN_EP_BASE];
            g_ep_table.EP[ep].IN.CNT     = 0;
            g_ep_table.EP[ep].IN.STATUS  = USB_BUSNAK_bm;    /* idle, owned by SW */
        } else {
            g_ep_table.EP[ep].OUT.CTRL    = ctrl_tp | bufsz;
            g_ep_table.EP[ep].OUT.DATAPTR = (uint16_t)s_dyn_ep_buf[ep - USBCORE_DYN_EP_BASE];
            g_ep_table.EP[ep].OUT.CNT     = 0;
            g_ep_table.EP[ep].OUT.STATUS  = 0;               /* armed for RX */
        }
    }
}

/* Service dynamic OUT (host->device) endpoints. Called from the TRNCOMPL ISR.
 * Dynamic IN reports still complete synchronously inside USB_Send(); only OUT
 * endpoints need servicing here. For each plugged OUT EP that completed a
 * transaction: ack TRNCOMPL, copy the packet into its RX ring, then re-arm.
 * All STATUS access is OUTCLR (atomic, preserves the DATA0/DATA1 TOGGLE) -
 * never a direct STATUS write. */
void usbcore_service_dynamic_ep_trncompl(void) {
    for (uint8_t ep = USBCORE_DYN_EP_BASE; ep < USBCORE_NUM_EP; ep++) {
        uint8_t t = s_ep_types[ep];
        if (t == 0 || (t & 0x80)) continue;            /* unused or IN endpoint */
        if (!(g_ep_table.EP[ep].OUT.STATUS & USB_TRNCOMPL_bm)) continue;

        uint8_t i = ep - USBCORE_DYN_EP_BASE;
        while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
        USB0.STATUS[ep].OUTCLR = USB_TRNCOMPL_bm;      /* ack the transaction */

        uint16_t cnt = g_ep_table.EP[ep].OUT.CNT;
        if (cnt > USBCORE_DYN_EP_BUF) cnt = USBCORE_DYN_EP_BUF;
        for (uint16_t k = 0; k < cnt; k++) {
            uint8_t next = (uint8_t)((s_dyn_rx_head[i] + 1) % USBCORE_DYN_RX_RING);
            if (next == s_dyn_rx_tail[i]) break;       /* ring full: drop the rest */
            s_dyn_rx_ring[i][s_dyn_rx_head[i]] = s_dyn_ep_buf[i][k];
            s_dyn_rx_head[i] = next;
        }

        g_ep_table.EP[ep].OUT.CNT = 0;
        while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
        USB0.STATUS[ep].OUTCLR = USB_BUSNAK_bm;        /* re-arm for next packet */
    }
}

/* ============================================================
 *  USBDevice control object  (Arduino Leonardo / UNO R4 compatible)
 *  Declared in api/USBAPI.h; implemented here for the AVR-DU USB peripheral
 *  so libraries referencing the global USBDevice (HID-Project System,
 *  MIDIUSB, ...) link and run unchanged.
 * ============================================================ */

/* USB0 bus-signalling bits for device-initiated remote wakeup.
 * Datasheet DS40002548A: USB.CTRLB and USB.BUSSTATE. Fallbacks match the
 * documented bit positions if the I/O header names them differently. */
#ifndef USB_URESUME_bm
#define USB_URESUME_bm    (1 << 3)   /* CTRLB / BUSSTATE bit 3: Upstream Resume */
#endif
#ifndef USB_SUSPENDED_bm
#define USB_SUSPENDED_bm  (1 << 1)   /* BUSSTATE bit 1: Bus Suspended           */
#endif
#ifndef USB_WTRSM_bm
#define USB_WTRSM_bm      (1 << 4)   /* BUSSTATE bit 4: Wait Time Resume elapsed */
#endif

USBDevice_::USBDevice_() { }

bool USBDevice_::configured() { return usbIsConfigured(); }
void USBDevice_::attach()     { usbAttach(); }
void USBDevice_::detach()     { usbDetach(); }
void USBDevice_::poll()       { usbPoll(); }

bool USBDevice_::isSuspended() {
    return (USB0.BUSSTATE & USB_SUSPENDED_bm) != 0;
}

/* Device-initiated remote wakeup (upstream resume).
 * Returns false unless the host enabled it (SET_FEATURE DEVICE_REMOTE_WAKEUP,
 * tracked in g_remote_wakeup_enabled) AND the bus is suspended. Per the
 * datasheet, an upstream resume must not start until the bus has been
 * suspended >= 5 ms (T_WTRSM); BUSSTATE.WTRSM signals that. We wait (bounded)
 * for WTRSM, then write CTRLB.URESUME (hardware self-clears it). */
bool USBDevice_::wakeupHost() {
    if (!g_remote_wakeup_enabled)            return false;
    if (!(USB0.BUSSTATE & USB_SUSPENDED_bm)) return false;

    for (uint16_t guard = 0; !(USB0.BUSSTATE & USB_WTRSM_bm); ++guard) {
        if (guard == 0xFFFF) return false;   /* WTRSM never set: bail out */
    }

    USB0.CTRLB |= USB_URESUME_bm;   /* initiate upstream resume (self-clearing) */
    return true;
}

USBDevice_ USBDevice;

#endif /* USB0 */
