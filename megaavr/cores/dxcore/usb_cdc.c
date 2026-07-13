/* AVR DU native-USB stack. Compiled only on parts with the USB peripheral. */
#include <avr/io.h>   /* defines USB0 on parts that have USB; must precede the guard */
#if defined(USB0)
/**
 * usb_cdc.c
 * CDC-ACM implementation: line coding, control line state, ring buffers,
 * Leonardo-style 1200bps touch reset.
 *
 *   EP1 IN  (16B) - notification (unused, but initialised)
 *   EP2 OUT (64B) - data RX (host -> device)
 *   EP3 IN  (64B) - data TX (device -> host)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>
#include "usb_core.h"
#include "usb_cdc.h"

/* ============================================================
 * CDC class request codes (USB CDC PSTN 1.20 Table 13)
 * ============================================================ */
#define CDC_REQ_SET_LINE_CODING        0x20
#define CDC_REQ_GET_LINE_CODING        0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE 0x22
#define CDC_REQ_SEND_BREAK             0x23

#define CDC_CTRL_LINE_DTR              0x01
#define CDC_CTRL_LINE_RTS              0x02

/* ============================================================
 * Bootloader handshake contract
 *
 * When USING_AVRDU_CDC_BOOTLOADER is defined (set by boards.txt for
 * the avrduusb.* board family), the 1200 bps touch path writes a
 * known magic value into GPR.GPR1 (low) + GPR.GPR2 (high) just before
 * triggering a watchdog reset.  The bootloader reads those GPRs on
 * every boot and stays resident if it sees AVRDU_BL_MAGIC_STAY.
 *
 * Why GPRs and not an SRAM address?  AVR64DU32 SRAM ends at 0x7FFF,
 * and the very first CALL in the bootloader's crt0 (the call to
 * main()) pushes the return address to 0x7FFE/0x7FFF, destroying any
 * 16-bit value we had stored there.  The four general-purpose I/O
 * registers (datasheet section 9 "General Purpose Registers", at I/O
 * 0x001C..0x001F) survive software/watchdog/BOD resets and are never
 * touched by crt0 or the stack, so they are the natural place for
 * this kind of cross-reset handshake.
 *
 * GPR.GPR0 is reserved by the Optiboot/DxCore convention to carry the
 * reset cause to the application (see cores/dxcore/main.cpp's
 * init_reset_flags), so we use GPR1 + GPR2 here. GPR3 stays free.
 *
 * The mapping must match exactly between this file and
 * bootloaders/avrdu_cdc_bl/src/main.c.
 * ============================================================ */
#define AVRDU_BL_MAGIC_STAY  0xB007u

/* ============================================================
 * Diagnostic bit flags stored in GPR.GPR3.
 *
 * GPR.GPR3 is preserved across software / WDT / BOD resets and is not
 * touched by the BL's normal startup sequence, so we can use it as a
 * cross-reset breadcrumb trail: the runtime sets bits as it makes
 * progress through the 1200 bps touch path, and the BL displays them
 * on the diagnostic LED at startup.  After the user pulls the breadcrumb
 * pattern off the LED they can press PF6 to re-enter the BL via EXTRF;
 * the bits will be cleared then so the next attempt starts fresh.
 *
 * Bit layout:
 *   0  any SET_LINE_CODING was received
 *   1  SET_LINE_CODING completed with dwDTERate == 1200
 *   2  any SET_CONTROL_LINE_STATE was received
 *   3  SET_CONTROL_LINE_STATE delivered DTR=0 while baud==1200
 *   4  trigger_1200bps_reset() was entered
 *   5  reserved
 *   6  reserved
 *   7  reserved
 * ============================================================ */
#define AVRDU_DIAG_SLC_SEEN     (1u << 0)
#define AVRDU_DIAG_SLC_1200     (1u << 1)
#define AVRDU_DIAG_CLS_SEEN     (1u << 2)
#define AVRDU_DIAG_CLS_DTR0     (1u << 3)
#define AVRDU_DIAG_TRIG_ENTER   (1u << 4)

static inline void diag_set(uint8_t bm) {
#ifdef USING_AVRDU_CDC_BOOTLOADER
    GPR.GPR3 |= bm;
#else
    (void)bm;
#endif
}

/* ============================================================
 * CDC Line Coding structure (7 bytes)
 * ============================================================ */
typedef struct __attribute__((packed)) {
    uint32_t dwDTERate;        /* baud rate */
    uint8_t  bCharFormat;      /* 0=1 stop, 1=1.5, 2=2 */
    uint8_t  bParityType;      /* 0=none, 1=odd, 2=even, 3=mark, 4=space */
    uint8_t  bDataBits;        /* 5,6,7,8,16 */
} cdc_line_coding_t;

static cdc_line_coding_t g_line_coding = {
    .dwDTERate   = 115200,
    .bCharFormat = 0,
    .bParityType = 0,
    .bDataBits   = 8
};

static uint8_t volatile  g_control_line_state = 0;   /* DTR/RTS */
static bool    g_pending_set_line_coding = false;

/* ============================================================
 * Reset-surviving breadcrumb (diagnostic)
 *
 * Placed in .noinit so crt0 does NOT clear it, and SRAM contents
 * are retained across any non-POR reset. This lets us see, after
 * the chip bounces back into the app, exactly how far the touch
 * sequence got - something the GPR3 breadcrumb cannot do because
 * GPRs are reset to 0x00 by the reset itself.
 *
 *   g_bc_sig         : signature; if not VALID after boot we came
 *                      from POR (RAM was random) -> zero the counters
 *   g_bc_saw1200     : times SET_LINE_CODING completed with baud==1200
 *                      (i.e. the 1200bps touch's line-coding DID reach us)
 *   g_bc_trig        : times trigger_1200bps_reset() was entered
 *   g_bc_trig_baud   : g_line_coding.dwDTERate captured at the last trigger
 *   g_bc_trig_site   : 1 = fired from SET_CONTROL_LINE_STATE path,
 *                      2 = fired from SET_LINE_CODING-completion path
 * ============================================================ */
#define AVRDU_BC_SIG_VALID 0xB00Du
volatile uint16_t g_bc_sig       __attribute__((section(".noinit")));
volatile uint16_t g_bc_saw1200   __attribute__((section(".noinit")));
volatile uint16_t g_bc_trig      __attribute__((section(".noinit")));
volatile uint32_t g_bc_trig_baud __attribute__((section(".noinit")));
volatile uint8_t  g_bc_trig_site __attribute__((section(".noinit")));

/* Called once from the sketch's setup(). Initialises the breadcrumb on
 * a cold (POR) boot and returns nothing; the sketch reads the globals via
 * the accessors below. */
void usbCdcDiagBreadcrumbInit(void) {
    if (g_bc_sig != AVRDU_BC_SIG_VALID) {
        g_bc_sig       = AVRDU_BC_SIG_VALID;
        g_bc_saw1200   = 0;
        g_bc_trig      = 0;
        g_bc_trig_baud = 0;
        g_bc_trig_site = 0;
    }
}
uint16_t usbCdcDiagSaw1200(void)   { return g_bc_saw1200; }
uint16_t usbCdcDiagTrigCount(void) { return g_bc_trig; }
uint32_t usbCdcDiagTrigBaud(void)  { return g_bc_trig_baud; }
uint8_t  usbCdcDiagTrigSite(void)  { return g_bc_trig_site; }

/* ============================================================
 * RX / TX ring buffers
 * ============================================================ */
#define CDC_RX_RING_SIZE   192
#define CDC_TX_RING_SIZE   192

static uint8_t  g_rx_ring[CDC_RX_RING_SIZE];
static volatile uint8_t g_rx_head = 0;     /* write by USB stack */
static volatile uint8_t g_rx_tail = 0;     /* read by app        */

static uint8_t  g_tx_ring[CDC_TX_RING_SIZE];
static volatile uint8_t g_tx_head = 0;     /* write by app       */
static volatile uint8_t g_tx_tail = 0;     /* read by USB stack  */

static volatile bool g_tx_in_flight = false;  /* EP3 currently transmitting */
static volatile bool g_tx_last_full = false;  /* last armed EP3 IN packet was exactly USB_EP3_SIZE */

/* ============================================================
 * 1200 bps touch-reset (Leonardo style)
 *
 * The IDE drops DTR low while baud=1200 to request the bootloader.
 * We hand off to the bootloader by issuing a SOFTWARE RESET: this
 * sets RSTCTRL.RSTFR.SWRF, which - unlike the GPR registers, whose
 * reset value is 0x00 - is RETAINED across the reset by design (the
 * datasheet: "After any Reset, the source that caused the Reset is
 * found in the Reset Flag"). The bootloader stays resident whenever
 * it sees SWRF. The earlier GPR-magic scheme could never work because
 * the reset clears the GPRs before the BL can read them.
 * ============================================================ */
static void trigger_1200bps_reset(void) {
    diag_set(AVRDU_DIAG_TRIG_ENTER);
    /* Reset-surviving breadcrumb: record that we got here and at what baud. */
    g_bc_trig++;
    g_bc_trig_baud = g_line_coding.dwDTERate;

    /* Detach from the bus so the host sees a clean disconnect, then give
     * it a moment before we reset and re-enumerate as the bootloader. */
    USB0.CTRLB &= ~USB_ATTACH_bm;
    for (volatile uint32_t i = 0; i < 120000UL; i++) { __asm__ __volatile__("nop"); }

    /* Software reset -> RSTFR.SWRF set on next boot -> bootloader stays. */
    _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRST_bm);
    while (1) { /* await reset */ }
}

/* ============================================================
 * EP2 OUT (RX from host) — invoked from the TRNCOMPL ISR
 * ============================================================ */
volatile uint16_t g_cdc_rx_total = 0;   /* diagnostic: raw bytes received on EP2 OUT */
volatile uint16_t g_cdc_tx_starts = 0;  /* diagnostic: EP3 IN transfers started */
volatile uint16_t g_cdc_tx_pkts   = 0;  /* diagnostic: EP3 IN completions (host read) */

void usb_cdc_on_ep2_out(uint16_t cnt) {
    if (cnt > USB_EP2_SIZE) cnt = USB_EP2_SIZE;
    g_cdc_rx_total += cnt;
    for (uint16_t i = 0; i < cnt; i++) {
        uint8_t next = (uint8_t)((g_rx_head + 1) % CDC_RX_RING_SIZE);
        if (next == g_rx_tail) break;            /* overflow: drop rest */
        g_rx_ring[g_rx_head] = g_ep2_out_buf[i];
        g_rx_head = next;
    }
}

/* ============================================================
 * EP3 IN done — clear in-flight flag so next packet can be queued
 * ============================================================ */
static void cdc_tx_pump(void);  /* fwd decl */

void usb_cdc_on_ep3_in_done(void) {
    g_cdc_tx_pkts++;
    g_tx_in_flight = false;
    cdc_tx_pump();           /* send next queued chunk, if any (ISR context) */
}

/* ============================================================
 * TX pump — fills EP3 IN buffer from g_tx_ring.
 * Runs in ISR context (EP3-IN-done and SOF hooks); from main context it
 * is reached via cdc_tx_kick(), which masks interrupts to stay race-free.
 * ============================================================ */
static void cdc_tx_pump(void) {
    if (g_tx_in_flight) return;
    if (g_current_configuration != 1) return;

    /* Endpoint must be NAK (= ready) before we touch CNT */
    if (!(g_ep_table.EP[3].IN.STATUS & USB_BUSNAK_bm)) return;

    uint16_t n = 0;
    while (n < USB_EP3_SIZE) {
        if (g_tx_tail == g_tx_head) break;
        g_ep3_in_buf[n++] = g_tx_ring[g_tx_tail];
        g_tx_tail = (g_tx_tail + 1) % CDC_TX_RING_SIZE;
    }
    if (n == 0) return;

    g_ep_table.EP[3].IN.CNT = n;
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
    USB0.STATUS[3].INCLR = USB_BUSNAK_bm;
    g_cdc_tx_starts++;
    g_tx_in_flight = true;
    /* Remember whether this was a maximum-length packet: a full 64-byte
     * bulk-IN packet does NOT terminate the host's transfer, so if the
     * ring then drains we must emit a ZLP (see usb_cdc_on_sof). */
    g_tx_last_full = (n == USB_EP3_SIZE);
}

/* Start a TX transfer from NON-interrupt (main) context, race-free against
 * the EP3-done ISR by briefly masking interrupts around the pump. */
static inline void cdc_tx_kick(void) {
    uint8_t sreg = SREG;
    cli();
    cdc_tx_pump();
    SREG = sreg;
}

/* Enqueue one byte into the TX ring WITHOUT kicking (internal helper). */
static bool tx_ring_put(uint8_t b) {
    uint8_t next = (uint8_t)((g_tx_head + 1) % CDC_TX_RING_SIZE);
    if (next == g_tx_tail) return false;     /* ring full */
    g_tx_ring[g_tx_head] = b;
    g_tx_head = next;
    return true;
}

/* ============================================================
 * Start-of-Frame hook (called from the bus-event ISR every 1 ms).
 *
 * A USB bulk-IN transfer is only delivered to the host application when it
 * is terminated by a short packet (< wMaxPacketSize) or a zero-length
 * packet (ZLP). A tight sketch loop keeps the TX ring full, so every packet
 * is a full 64-byte packet and the host never sees a terminator - the data
 * sits buffered on the host until ~64-byte boundaries pile up. This is why
 * inserting delay() (which lets the ring drain to a short final packet)
 * "fixes" output.
 *
 * Here we flush automatically: once per frame, if nothing is in flight and
 * the ring is empty but the last packet we sent was full, arm a ZLP to
 * close the transfer so the host releases the buffered bytes. The flag is
 * cleared after one ZLP, so a continuous stream (ring never empty) is never
 * interrupted and an idle link emits exactly one terminating ZLP.
 * ============================================================ */
void usb_cdc_on_sof(void) {
    if (g_current_configuration != 1) return;
    if (g_tx_in_flight) return;
    /* Data queued but not yet shipped: ensure forward progress. */
    if (g_tx_head != g_tx_tail) { cdc_tx_pump(); return; }
    /* Ring empty: terminate a dangling full-packet transfer with a ZLP. */
    if (g_tx_last_full && (g_ep_table.EP[3].IN.STATUS & USB_BUSNAK_bm)) {
        g_ep_table.EP[3].IN.CNT = 0;
        while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
        USB0.STATUS[3].INCLR = USB_BUSNAK_bm;
        g_cdc_tx_starts++;
        g_tx_in_flight = true;
        g_tx_last_full = false;
    }
}

void usbCdcPoll(void) {
    /* Interrupt-driven now; retained for API compatibility. A kick here is
     * harmless and guarantees forward progress if ever called manually. */
    cdc_tx_kick();
}

/* ============================================================
 * Reset / configured hooks
 * ============================================================ */
void usb_cdc_on_reset(void) {
    g_rx_head = g_rx_tail = 0;
    g_tx_head = g_tx_tail = 0;
    g_tx_in_flight = false;
    g_tx_last_full = false;
    g_control_line_state = 0;
    g_pending_set_line_coding = false;
}

void usb_cdc_on_configured(void) {
    g_tx_in_flight = false;
    g_tx_last_full = false;
}

/* ============================================================
 * Class request handling
 * ============================================================ */
void usb_cdc_handle_class_request(const usb_setup_t *s) {
    switch (s->bRequest) {
    case CDC_REQ_SET_LINE_CODING:
        diag_set(AVRDU_DIAG_SLC_SEEN);
        if (s->wLength == sizeof(cdc_line_coding_t)) {
            g_pending_set_line_coding = true;
            ep0_start_data_out((uint8_t *)&g_line_coding, sizeof(cdc_line_coding_t));
        } else {
            ep0_stall();
        }
        break;

    case CDC_REQ_GET_LINE_CODING:
        ep0_start_data_in((const uint8_t *)&g_line_coding,
                          sizeof(cdc_line_coding_t), s->wLength);
        break;

    case CDC_REQ_SET_CONTROL_LINE_STATE: {
        diag_set(AVRDU_DIAG_CLS_SEEN);
        uint8_t new_state = s->wValue & 0x03;
        g_control_line_state = new_state;
        ep0_send_zlp();
        /* Caterina-style touch-reset condition: do NOT require a falling
         * edge on DTR.  The Windows USB CDC driver does not always raise
         * DTR on port open (especially when the previous host close left
         * DTR low), so the "DTR was 1, now 0" edge we used to require
         * is unreliable.  Arduino Leonardo (Caterina) and Pro Micro both
         * use the simpler "DTR currently low AND baud == 1200" test, and
         * 1200 bps is by convention reserved for this purpose - no real
         * application uses 1200 bps for ongoing communication - so a false
         * positive is in practice impossible.
         */
        if ((new_state & CDC_CTRL_LINE_DTR) == 0
                && g_line_coding.dwDTERate == 1200) {
            diag_set(AVRDU_DIAG_CLS_DTR0);
            g_bc_trig_site = 1;          /* fired from SET_CONTROL_LINE_STATE */
            trigger_1200bps_reset();
        }
        break;
    }

    case CDC_REQ_SEND_BREAK:
        ep0_send_zlp();
        break;

    default:
        ep0_stall();
        break;
    }
}

/* Called from usb_class_data_out_complete after EP0 DATA-OUT stage */
void usb_cdc_data_out_complete(void) {
    if (g_pending_set_line_coding) {
        g_pending_set_line_coding = false;
        if (g_line_coding.dwDTERate == 1200) {
            diag_set(AVRDU_DIAG_SLC_1200);
            g_bc_saw1200++;              /* the touch's line-coding reached us */
            /* Order-independent touch detection. The host may set 1200 baud
             * AFTER it has already driven DTR low: the order of
             * SET_LINE_CODING vs SET_CONTROL_LINE_STATE during a port open
             * is host-specific, and a redundant DTR=0 (when DTR was already
             * 0) is frequently not re-sent. In that case the
             * SET_CONTROL_LINE_STATE handler never sees "DTR=0 while
             * baud==1200", because at the time it ran the baud was not yet
             * 1200. So we evaluate the same condition here, when the baud
             * becomes 1200, using the DTR state captured so far. Whichever
             * of the two requests completes the (DTR==0 && baud==1200) pair
             * last is the one that fires. */
            if ((g_control_line_state & CDC_CTRL_LINE_DTR) == 0) {
                diag_set(AVRDU_DIAG_CLS_DTR0);
                g_bc_trig_site = 2;      /* fired from SET_LINE_CODING completion */
                ep0_send_zlp();              /* finish this request's status stage */
                trigger_1200bps_reset();     /* never returns */
            }
        }
        /* g_line_coding has been filled by the EP0 OUT stage already
         * (ep0_start_data_out pointed the endpoint at it). */
    }
}

/* ============================================================
 * Public API
 * ============================================================ */
bool usbCdcReady(void) {
    return (g_current_configuration == 1)
        && (g_control_line_state & CDC_CTRL_LINE_DTR);
}

bool usbCdcTxReady(void) {
    if (g_current_configuration != 1) return false;
    return !g_tx_in_flight && (g_ep_table.EP[3].IN.STATUS & USB_BUSNAK_bm);
}

/* True when nothing is queued and no packet is in flight (flush() target). */
bool usbCdcTxIdle(void) {
    return (!g_tx_in_flight) && (g_tx_head == g_tx_tail);
}

/* diagnostics */
uint8_t usbCdcLineState(void) { return g_control_line_state; }
bool    usbCdcTxInFlight(void){ return g_tx_in_flight; }
uint32_t usbCdcLineCodingBaud(void) { return g_line_coding.dwDTERate; }

uint16_t usbCdcAvailable(void) {
    int16_t n = (int16_t)g_rx_head - (int16_t)g_rx_tail;
    if (n < 0) n += CDC_RX_RING_SIZE;
    return (uint16_t)n;
}

uint16_t usbCdcTxFree(void) {
    int16_t used = (int16_t)g_tx_head - (int16_t)g_tx_tail;
    if (used < 0) used += CDC_TX_RING_SIZE;
    /* tx_ring_put() keeps one slot empty to tell "full" from "empty" */
    return (uint16_t)((CDC_TX_RING_SIZE - 1) - used);
}

int usbCdcRead(void) {
    if (g_rx_head == g_rx_tail) return -1;
    uint8_t b = g_rx_ring[g_rx_tail];
    g_rx_tail = (g_rx_tail + 1) % CDC_RX_RING_SIZE;
    return b;
}

uint16_t usbCdcReadBytes(uint8_t *dst, uint16_t maxlen) {
    uint16_t n = 0;
    while (n < maxlen) {
        int c = usbCdcRead();
        if (c < 0) break;
        dst[n++] = (uint8_t)c;
    }
    return n;
}

bool usbCdcWriteByte(uint8_t b) {
    if (!tx_ring_put(b)) return false;
    cdc_tx_kick();                   /* start TX now; ISR drives the rest */
    return true;
}

uint16_t usbCdcWrite(const uint8_t *src, uint16_t len) {
    uint16_t n = 0;
    while (n < len && tx_ring_put(src[n])) n++;
    if (n) cdc_tx_kick();            /* one kick for the whole buffer */
    return n;
}

uint16_t usbCdcPrint(const char *s) {
    uint16_t n = 0;
    while (s[n]) {
        if (!usbCdcWriteByte((uint8_t)s[n])) break;
        n++;
    }
    return n;
}

uint16_t usbCdcPrintln(const char *s) {
    uint16_t n = usbCdcPrint(s);
    usbCdcWriteByte('\r');
    usbCdcWriteByte('\n');
    return n + 2;
}

#endif /* USB0 */
