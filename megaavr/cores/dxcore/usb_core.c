/* AVR DU native-USB stack. Compiled only on parts with the USB peripheral. */
#include <avr/io.h>   /* defines USB0 on parts that have USB; must precede the guard */
#if defined(USB0)
/**
 * usb_core.c
 * USB stack core: initialization, interrupt service routines, control
 * state machine
 *
 * Target: AVR64DU32 with DxCore
 *
 * Architecture:
 *   - Uses official USB_EP_TABLE_t (FIFO[32] + EP[16] + FRAMENUM)
 *   - EPPTR set to &g_ep_table.EP[0]  (FIFO occupies negative offsets)
 *   - Fully interrupt-driven: USB0_BUSEVENT_vect (reset/SOF) and
 *     USB0_TRNCOMPL_vect (SETUP + per-endpoint TRNCOMPL) drive all activity;
 *     usbPoll() is a no-op kept for source compatibility.
 *   - Uses official USB_*_bm macros from ioavr64du32.h
 *
 *  EP map (CDC fixed on EP1..EP3; HID and other classes are allocated
 *  dynamically on EP4+ by PluggableUSB modules - see USBCore_DU.cpp):
 *    EP0      Control
 *    EP1 IN   CDC notify     (interrupt 16B)
 *    EP2 OUT  CDC data RX    (bulk      64B)
 *    EP3 IN   CDC data TX    (bulk      64B)
 *    EP4..EP7 dynamic        (PluggableUSB: HID Keyboard/Mouse share one
 *                             interrupt-IN EP via Report IDs, Leonardo-style)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "usb_core.h"
#include "usb_standard.h"
#include "USBCore_DU.h"

/* ============================================================
 * Global state
 * ============================================================ */
USB_EP_TABLE_t g_ep_table __attribute__((aligned(2)));

uint8_t  g_ep0_setup_buf[8]            __attribute__((aligned(2)));
uint8_t  g_ep0_data_buf[USB_EP0_SIZE]  __attribute__((aligned(2)));
uint8_t  g_ep1_in_buf[USB_EP1_SIZE]    __attribute__((aligned(2)));
uint8_t  g_ep2_out_buf[USB_EP2_SIZE]   __attribute__((aligned(2)));
uint8_t  g_ep3_in_buf[USB_EP3_SIZE]    __attribute__((aligned(2)));
uint8_t  g_ep4_in_buf[USB_EP4_SIZE]    __attribute__((aligned(2)));
uint8_t  g_ep5_in_buf[USB_EP5_SIZE]    __attribute__((aligned(2)));
uint8_t  g_ep6_in_buf[USB_EP6_SIZE]    __attribute__((aligned(2)));

ctrl_state_t g_ctrl_state          = CTRL_IDLE;

/* --- diagnostic snapshot (read by the test sketch over Serial1) --- */
volatile uint8_t g_dbg_ctrl_state    = 0;
volatile uint8_t g_dbg_ep0out_status = 0;
volatile uint8_t g_dbg_ep0in_status  = 0;
volatile uint8_t g_dbg_intflagsb     = 0;
volatile uint8_t g_dbg_usbaddr       = 0;
uint8_t          g_pending_address   = 0;
volatile uint8_t g_current_configuration = 0;
uint8_t          g_remote_wakeup_enabled = 0;

volatile uint16_t g_reset_count          = 0;
volatile uint16_t g_setup_count          = 0;
volatile uint16_t g_get_desc_count       = 0;
volatile uint16_t g_stall_count          = 0;
volatile uint16_t g_setcfg_count         = 0;
volatile uint16_t g_class_req_count      = 0;
volatile uint16_t g_mpkt_count           = 0;   /* multipacket IN transfers started */
volatile uint16_t g_ep0in_tc_count       = 0;   /* EP0 IN transaction-complete events */
volatile uint8_t  g_last_cfg_value       = 0xFF;
volatile uint8_t  g_last_bmRequestType   = 0;
volatile uint8_t  g_last_bRequest        = 0;
volatile uint16_t g_last_wValue          = 0;
volatile uint16_t g_last_wLength         = 0;

/* ============================================================
 * RMW busy wait
 * Datasheet 28.3.3.1: "The result of a write to STATUS{In/Out}{Clear/Set}
 * while RMWBUSY is set is undefined."
 * ============================================================ */
static inline void rmw_wait(void) {
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
}

/* ============================================================
 * Forward declarations (CDC hooks; see usb_cdc.c)
 *
 * These are weak-linked so a no-CDC build still links.
 * usb_cdc.c provides strong implementations.
 * ============================================================ */
__attribute__((weak)) void usb_cdc_on_configured(void)  { }
__attribute__((weak)) void usb_cdc_on_reset(void)       { }
__attribute__((weak)) void usb_cdc_on_ep2_out(uint16_t cnt) { (void)cnt; }
__attribute__((weak)) void usb_cdc_on_ep3_in_done(void) { }
__attribute__((weak)) void usb_cdc_on_sof(void)         { }

/* ============================================================
 * Endpoint table initialization
 *
 * On bus-reset all endpoints are wiped and rebuilt:
 *   EP0 control armed for SETUP,
 *   EP1/EP3/EP4/EP5/EP6 IN  NAK until data is queued,
 *   EP2 OUT armed to receive bulk data from host.
 * ============================================================ */
static void usb_ep_table_init(void) {
    /* Zero entire table (FIFO + EP[16] + FRAMENUM) */
    memset(&g_ep_table, 0, sizeof(g_ep_table));

    /* EP0 OUT: Control, ready to receive SETUP */
    g_ep_table.EP[0].OUT.CTRL    = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_ep_table.EP[0].OUT.DATAPTR = (uint16_t)g_ep0_setup_buf;
    g_ep_table.EP[0].OUT.STATUS  = 0x00;

    /* EP0 IN: Control, NAK until response queued */
    g_ep_table.EP[0].IN.CTRL     = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_ep_table.EP[0].IN.DATAPTR  = (uint16_t)g_ep0_data_buf;
    g_ep_table.EP[0].IN.STATUS   = USB_BUSNAK_bm;

    /* EP1 IN: CDC notification (interrupt 16B), NAK initially */
    g_ep_table.EP[1].IN.CTRL     = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF16_gc;
    g_ep_table.EP[1].IN.DATAPTR  = (uint16_t)g_ep1_in_buf;
    g_ep_table.EP[1].IN.STATUS   = USB_BUSNAK_bm;

    /* EP2 OUT: CDC data RX (bulk 64B), armed to receive */
    g_ep_table.EP[2].OUT.CTRL    = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_ep_table.EP[2].OUT.DATAPTR = (uint16_t)g_ep2_out_buf;
    g_ep_table.EP[2].OUT.STATUS  = 0x00;

    /* EP3 IN: CDC data TX (bulk 64B), NAK until data queued */
    g_ep_table.EP[3].IN.CTRL     = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_ep_table.EP[3].IN.DATAPTR  = (uint16_t)g_ep3_in_buf;
    g_ep_table.EP[3].IN.STATUS   = USB_BUSNAK_bm;

    /* HID and other classes are NOT hardcoded here. EP4..EP7 stay zeroed by
     * the memset above (TYPE = DISABLED) and are programmed dynamically at
     * SET_CONFIGURATION by usbcore_init_plugged_endpoints() from the EP types
     * the PluggableUSB modules registered via plug(). Keyboard and Mouse use
     * the bundled HID library, which is a single PluggableUSB module owning
     * one interrupt-IN EP and one interface (Report IDs multiplex the two),
     * exactly as on the Arduino Leonardo. */
}

/* ============================================================
 * EP0 control transfer helpers
 * ============================================================ */

/* ---- EP0 IN data stage: software-driven single-packet chunking ----
 * The AVR DU hardware MULTIPKT feature did not reliably complete EP0 control
 * reads larger than the endpoint size on this part. Instead we stream the
 * data stage as a sequence of single-packet IN transactions, each copied into
 * the word-aligned g_ep0_data_buf (the exact path already proven to work for
 * short descriptors). The hardware auto-toggles DATA0/DATA1 between
 * consecutive IN packets, so we must NOT touch TOGGLE between chunks. */
static const uint8_t *g_ep0_in_src;      /* next byte to send           */
static uint16_t       g_ep0_in_rem;      /* bytes still to send         */
static bool           g_ep0_in_need_zlp; /* terminating ZLP required?   */

/* Load and fire one IN packet (n <= USB_EP0_SIZE). EP0.IN must be deactivated
 * (BUSNAK set) on entry — true right after SETUP and after each IN TRNCOMPL,
 * which satisfies the datasheet rule that CNT/MCNT are written while NAKed. */
static void ep0_send_chunk(uint16_t n) {
    for (uint16_t i = 0; i < n; i++) g_ep0_data_buf[i] = g_ep0_in_src[i];
    g_ep0_in_src += n;
    g_ep0_in_rem -= n;

    g_ep_table.EP[0].IN.DATAPTR = (uint16_t)g_ep0_data_buf;
    g_ep_table.EP[0].IN.CNT     = n;
    g_ep_table.EP[0].IN.MCNT    = 0;
    g_ep_table.EP[0].IN.CTRL    = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    /* Activate: clear flags incl. BUSNAK, but preserve TOGGLE (DATA0/1 seq) */
    rmw_wait();
    USB0.STATUS[0].INCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;
}

void ep0_start_data_in(const uint8_t *data, uint16_t len, uint16_t host_requested) {
    if (len > host_requested) len = host_requested;

    g_ep0_in_src = data;
    g_ep0_in_rem = len;
    /* A short final packet ends the transfer. If we send a whole number of
     * max-size packets yet fewer bytes than requested, a ZLP is needed. */
    g_ep0_in_need_zlp = (len < host_requested) && (len != 0)
                        && ((len % USB_EP0_SIZE) == 0);

    if (len > USB_EP0_SIZE) g_mpkt_count++;   /* diagnostic: multi-chunk read */

    /* Pre-arm EP0.OUT once to ACK the status-stage ZLP that follows the data */
    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    g_ctrl_state = CTRL_DATA_IN_STAGE;

    /* Fire the first packet (len==0 sends a single ZLP) */
    uint16_t n = (g_ep0_in_rem > USB_EP0_SIZE) ? USB_EP0_SIZE : g_ep0_in_rem;
    ep0_send_chunk(n);
}

void ep0_start_data_out(uint8_t *buffer, uint16_t len) {
    /* Control-transfer data stage, OUT direction (host -> device, e.g.
     * SET_LINE_CODING's 7-byte payload).
     *
     * Datasheet 28.3.2.2 says the data stage uses the IN buffer pointer,
     * but the OUT-transaction machinery 28.3.2.4 says received data lands
     * at EP[n].OUT.DATAPTR - and that is what the silicon actually does
     * (confirmed empirically: with IN.DATAPTR-only the line-coding payload
     * never reached our buffer, so SET_LINE_CODING(1200) never updated the
     * baud and the touch-reset never fired). We therefore point BOTH
     * direction pointers at the caller's buffer with identical multipacket
     * settings, so the bytes land in `buffer` no matter which the silicon
     * consults, then restore OUT.DATAPTR to the SETUP buffer when the data
     * stage completes (see handle_ep0_out_complete). */
    g_ep_table.EP[0].IN.DATAPTR  = (uint16_t)buffer;
    g_ep_table.EP[0].IN.CNT      = 0;
    g_ep_table.EP[0].IN.MCNT     = len;
    g_ep_table.EP[0].IN.CTRL     = USB_TYPE_CONTROL_gc | USB_MULTIPKT_bm | USB_BUFSIZE_DEFAULT_BUF64_gc;

    g_ep_table.EP[0].OUT.DATAPTR = (uint16_t)buffer;
    g_ep_table.EP[0].OUT.CNT     = 0;
    g_ep_table.EP[0].OUT.MCNT    = len;
    g_ep_table.EP[0].OUT.CTRL    = USB_TYPE_CONTROL_gc | USB_MULTIPKT_bm | USB_BUFSIZE_DEFAULT_BUF64_gc;

    /* Clear OUT BUSNAK to accept incoming OUT tokens.
     * IN.BUSNAK stays set; the status-stage ZLP IN is armed later. */
    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    g_ctrl_state = CTRL_DATA_OUT_STAGE;
}

void ep0_send_zlp(void) {
    g_ep_table.EP[0].IN.CNT  = 0;
    g_ep_table.EP[0].IN.MCNT = 0;
    g_ep_table.EP[0].IN.CTRL = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    rmw_wait();
    USB0.STATUS[0].INCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    g_ctrl_state = CTRL_STATUS_IN_STAGE;
}

void ep0_stall(void) {
    g_stall_count++;
    g_ep_table.EP[0].OUT.CTRL |= USB_DOSTALL_bm;
    g_ep_table.EP[0].IN.CTRL  |= USB_DOSTALL_bm;
    g_ctrl_state = CTRL_IDLE;
}

/* ============================================================
 * SETUP packet handler
 * ============================================================ */

static void handle_setup(void) {
    usb_setup_t *s = (usb_setup_t *)g_ep0_setup_buf;

    g_last_bmRequestType = s->bmRequestType;
    g_last_bRequest      = s->bRequest;
    g_last_wValue        = s->wValue;
    g_last_wLength       = s->wLength;

    /* Clear DOSTALL on both EP0 directions for the NEW transfer.
     * While EPSETUP=1 the HW overrides DOSTALL (datasheet 28.7.4),
     * but once we clear EPSETUP below, a stale DOSTALL would STALL
     * the data/status stage. CTRL is plain SRAM, direct write is safe. */
    g_ep_table.EP[0].OUT.CTRL = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_ep_table.EP[0].IN.CTRL  = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    /* Clear EPSETUP on both directions (required before decoding) */
    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_EPSETUP_bm;
    rmw_wait();
    USB0.STATUS[0].INCLR  = USB_EPSETUP_bm;

    uint8_t type = s->bmRequestType & 0x60;
    switch (type) {
    case 0x00: usb_handle_standard_request(s); break;
    case 0x20: usb_handle_class_request(s);    break;
    default:   ep0_stall();                    break;
    }
}

/* ============================================================
 * EP completion handlers
 * ============================================================ */
extern void usb_class_data_out_complete(void);   /* in usb_standard.c */

static void handle_ep0_in_complete(void) {
    switch (g_ctrl_state) {
    case CTRL_DATA_IN_STAGE:
        if (g_ep0_in_rem > 0) {
            /* more data to send: fire the next single-packet chunk */
            uint16_t n = (g_ep0_in_rem > USB_EP0_SIZE) ? USB_EP0_SIZE : g_ep0_in_rem;
            ep0_send_chunk(n);
        } else if (g_ep0_in_need_zlp) {
            /* exact multiple of max packet but short of wLength: terminate */
            g_ep0_in_need_zlp = false;
            ep0_send_chunk(0);
        } else {
            /* all data sent — EP0.OUT was pre-armed in ep0_start_data_in() */
            g_ctrl_state = CTRL_STATUS_OUT_STAGE;
        }
        break;

    case CTRL_STATUS_PENDING_ADDR:
        USB0.ADDR = g_pending_address;
        g_pending_address = 0;
        g_ctrl_state = CTRL_IDLE;
        break;

    case CTRL_STATUS_IN_STAGE:
    default:
        g_ctrl_state = CTRL_IDLE;
        break;
    }
}

static void handle_ep0_out_complete(void) {
    switch (g_ctrl_state) {
    case CTRL_DATA_OUT_STAGE:
        /* DATA-OUT received — let class handler inspect, then ZLP status.
         * Restore the OUT direction to its SETUP-receive configuration:
         * we repointed DATAPTR and enabled multipacket in
         * ep0_start_data_out(), and the next SETUP packet must be DMA'd
         * back into g_ep0_setup_buf with the plain control config. */
        usb_class_data_out_complete();
        g_ep_table.EP[0].OUT.DATAPTR = (uint16_t)g_ep0_setup_buf;
        g_ep_table.EP[0].OUT.CNT     = 0;
        g_ep_table.EP[0].OUT.MCNT    = 0;
        g_ep_table.EP[0].OUT.CTRL    = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
        ep0_send_zlp();
        break;

    case CTRL_STATUS_OUT_STAGE:
    default:
        g_ctrl_state = CTRL_IDLE;
        break;
    }
}

/* ============================================================
 * Polled event loop — call from loop() as often as possible
 * ============================================================ */
/* ============================================================
 * Event service routines (invoked from the USB interrupt vectors)
 * ============================================================ */

/* Bus events — currently just bus RESET (INTFLAGSA / USB0_BUSEVENT_vect). */
static void usb_service_busevent(void) {
    uint8_t flags_a = USB0.INTFLAGSA;
    if (flags_a & USB_RESET_bm) {
        g_reset_count++;
        USB0.ADDR = 0;
        usb_ep_table_init();
        g_ctrl_state            = CTRL_IDLE;
        g_current_configuration = 0;
        g_pending_address       = 0;
        usb_cdc_on_reset();
        USB0.INTFLAGSA = USB_RESET_bm;
    }
    /* Start-of-Frame (1 ms): drive the CDC idle-flush so a paused TX
     * stream that ended on a full 64-byte packet gets a terminating ZLP. */
    if (flags_a & USB_SOF_bm) {
        usb_cdc_on_sof();
        USB0.INTFLAGSA = USB_SOF_bm;
    }
}

/* Transaction events — SETUP and per-endpoint TRNCOMPL
 * (INTFLAGSB / USB0_TRNCOMPL_vect). */
static void usb_service_trncompl(void) {
    uint8_t flags_b = USB0.INTFLAGSB;

    /* SETUP arrived */
    if (flags_b & USB_SETUP_bm) {
        g_setup_count++;
        handle_setup();
        USB0.INTFLAGSB = USB_SETUP_bm;
    }

    /* Transaction complete on any endpoint */
    if (flags_b & USB_TRNCOMPL_bm) {
        if (g_ep_table.EP[0].IN.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[0].INCLR = USB_TRNCOMPL_bm;
            g_ep0in_tc_count++;
            handle_ep0_in_complete();
        }
        if (g_ep_table.EP[0].OUT.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[0].OUTCLR = USB_TRNCOMPL_bm;
            handle_ep0_out_complete();
        }
        /* EP1 IN: CDC notification — HW re-sets BUSNAK, no action */
        if (g_ep_table.EP[1].IN.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[1].INCLR = USB_TRNCOMPL_bm;
        }
        /* EP2 OUT: CDC data from host */
        if (g_ep_table.EP[2].OUT.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[2].OUTCLR = USB_TRNCOMPL_bm;
            uint16_t cnt = g_ep_table.EP[2].OUT.CNT;
            usb_cdc_on_ep2_out(cnt);
            /* Re-arm EP2 OUT for next packet */
            g_ep_table.EP[2].OUT.CNT = 0;
            rmw_wait();
            USB0.STATUS[2].OUTCLR = USB_BUSNAK_bm;
        }
        /* EP3 IN: CDC TX done */
        if (g_ep_table.EP[3].IN.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[3].INCLR = USB_TRNCOMPL_bm;
            usb_cdc_on_ep3_in_done();
        }
        /* Dynamic PluggableUSB EPs (EP4+): IN reports are completed
         * synchronously inside USB_Send() (it polls the per-EP TRNCOMPL in
         * the SRAM endpoint table itself), so no servicing is required in the
         * ISR. Dynamic OUT (host->device) data is drained here. */
        usbcore_service_dynamic_ep_trncompl();
        USB0.INTFLAGSB = USB_TRNCOMPL_bm;
    }

    /* diagnostic snapshot of EP0 / control state for the test sketch */
    g_dbg_ctrl_state    = (uint8_t)g_ctrl_state;
    g_dbg_ep0out_status = g_ep_table.EP[0].OUT.STATUS;
    g_dbg_ep0in_status  = g_ep_table.EP[0].IN.STATUS;
    g_dbg_intflagsb     = USB0.INTFLAGSB;
    g_dbg_usbaddr       = USB0.ADDR;
}

/* The USB stack is interrupt-driven; the application no longer needs to call
 * usbPoll() from loop(). Retained as a no-op for source compatibility. */
void usbPoll(void) { }

/* Bus-event vector: RESET (and, in future, SUSPEND/RESUME). */
ISR(USB0_BUSEVENT_vect) {
    usb_service_busevent();
}

/* Transaction-complete vector: SETUP + per-endpoint TRNCOMPL. */
ISR(USB0_TRNCOMPL_vect) {
    usb_service_trncompl();
}

/* ============================================================
 * Initialization
 * ============================================================ */
void usbInit(void) {
    /* 1. Enable OSCHF SOF auto-tune, INCREMENTAL search algorithm.
     *
     * Per datasheet (CLKCTRL.OSCHFCTRLA, sec 12.5.8 / 12.3.5):
     *   - AUTOTUNE=SOF makes OSCHF (and its fixed 4 MHz tap feeding the
     *     PLL48M USB clock) tune itself against the USB Start-of-Frame.
     *     This is REQUIRED for crystal-less USB.
     *   - ALGSEL selects the tuning algorithm used after each USB bus reset:
     *       0 = Binary search    (DEFAULT) -- "may temporarily change the
     *           oscillator output frequency up to HALF the tune range",
     *           which corrupts the 48 MHz USB clock mid-enumeration. The
     *           host then resets, the search restarts and swings again -> a
     *           reset storm with intermittent control-transfer failures.
     *       1 = Incremental search -- nudges at most 5 tune steps after a
     *           reset, keeping the USB clock stable enough to enumerate.
     *   - The datasheet states ALGSEL "must be written simultaneously with
     *     writing the SOF setting to the AUTOTUNE bit field", so we set both
     *     fields in a single write below.
     */
    uint8_t oschf = CLKCTRL.OSCHFCTRLA;
    oschf &= ~(CLKCTRL_AUTOTUNE_gm | CLKCTRL_ALGSEL_bm);
    oschf |=  (CLKCTRL_AUTOTUNE_SOF_gc | CLKCTRL_ALGSEL_INCR_gc);
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, oschf);

    /* 2. Enable VUSB regulator (5V VDD -> 3.3V VUSB) */
    SYSCFG.VUSBCTRL = SYSCFG_USBVREG_bm;

    /* 3. Settle */
    _delay_ms(1);

    /* 4. Build EP table */
    usb_ep_table_init();

    /* 5. Point USB peripheral at &g_ep_table.EP[0]
     *    Hardware layout (datasheet 28.7.1):
     *      EPPTR - 32 .. -1   : FIFO[31..0] (negative offsets)
     *      EPPTR + 0          : EP[0].OUT.STATUS
     *      EPPTR + (N+1)*16   : FRAMENUM
     *    USB_EP_TABLE_t has FIFO[32] first, so EPPTR points past it. */
    USB0.EPPTR = (uint16_t)&g_ep_table.EP[0];

    /* 6. Enable USB interrupts (interrupt-driven; loop() need not poll).
     *    INTCTRLA -> USB0_BUSEVENT_vect ; INTCTRLB -> USB0_TRNCOMPL_vect */
    USB0.INTCTRLA = USB_RESET_bm | USB_SOF_bm;        /* bus reset + SOF      */
    USB0.INTCTRLB = USB_SETUP_bm | USB_TRNCOMPL_bm;   /* setup + trncompl     */

    /* 7. Enable USB peripheral; MAXEP in bits [3:0] */
    USB0.CTRLA = USB_ENABLE_bm | USB_MAXEP;

    /* 8. Wait for PLL lock (with timeout for debug safety) */
    {
        uint32_t timeout = 1000000UL;
        while (!(CLKCTRL.USBPLLSTATUS & CLKCTRL_PLLS_bm) && --timeout) {}
        if (timeout == 0) {
            /* PLL lock failed - panic-blink the LED on PF2 (Curiosity Nano) */
            VPORTF.DIR |= (1 << 2);
            while (1) {
                PORTF.OUTTGL = (1 << 2);
                _delay_ms(100);
            }
        }
    }

    /* 9. Disable GNAUTO/GNAK (we manage NAK manually) */
    USB0.CTRLB = 0;

    /* 10. Make sure global interrupts are enabled so the USB ISRs run.
     *     (Arduino normally has them on already; this is belt-and-braces.) */
    sei();
}

void usbAttach(void) {
    USB0.CTRLB |= USB_ATTACH_bm;
}

void usbDetach(void) {
    USB0.CTRLB &= ~USB_ATTACH_bm;
}

bool usbIsConfigured(void) {
    return g_current_configuration != 0;
}

#endif /* USB0 */
