/*
 * usbcdcboot/src/usb_min.c
 * --------------------------------------------------------------------
 *  Clean-room implementation.  References:
 *    - USB 2.0 specification (chapter 9: Device Framework)
 *    - AVR64DU32 datasheet section 28 (USB peripheral) - DS40002676
 *    - Microchip ATPACK device headers (avr/io.h, ioavr64du32.h)
 *
 *  No source from LUFA, TinyUSB, V-USB, Optiboot or any other USB
 *  or bootloader project was consulted while writing this file.
 *
 *  Polled USB peripheral driver: handles bus reset, SETUP packets,
 *  the Standard request set, and dispatches Class requests to
 *  cdc_min_handle_class_request().  EP table layout:
 *
 *    EP0 IN/OUT  Control                   64 B
 *    EP1 IN      CDC notification          16 B  (Interrupt, never sent)
 *    EP2 OUT     CDC data host->device     64 B  (Bulk)
 *    EP3 IN      CDC data device->host     64 B  (Bulk)
 *
 *  License: LGPL 2.1.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb_min.h"
#include "usb_desc.h"

/* Hooks for cdc_min */
void cdc_min_handle_class_request(const usb_setup_t *s);
void cdc_min_on_ep2_out(uint16_t cnt);
void cdc_min_on_ep3_in_done(void);
void cdc_min_on_reset(void);

/* ============================================================
 * Control transfer state machine - mirrors runtime but smaller
 * ============================================================ */
typedef enum {
    CTRL_IDLE,
    CTRL_DATA_IN_STAGE,
    CTRL_DATA_OUT_STAGE,
    CTRL_STATUS_IN_STAGE,
    CTRL_STATUS_OUT_STAGE,
    CTRL_STATUS_PENDING_ADDR
} ctrl_state_t;

/* ============================================================
 * Global state - exported for cdc_min to drive EP2/EP3 directly
 * ============================================================ */
USB_EP_TABLE_t g_bl_ep_table __attribute__((aligned(2)));

uint8_t  g_bl_ep0_setup[8]                    __attribute__((aligned(2)));
uint8_t  g_bl_ep0_data[USB_BL_EP0_SIZE]       __attribute__((aligned(2)));
uint8_t  g_bl_ep1_in [USB_BL_EP1_SIZE]        __attribute__((aligned(2)));
uint8_t  g_bl_ep2_out[USB_BL_EP2_SIZE]        __attribute__((aligned(2)));
uint8_t  g_bl_ep3_in [USB_BL_EP3_SIZE]        __attribute__((aligned(2)));

static ctrl_state_t s_ctrl_state         = CTRL_IDLE;
static uint8_t      s_pending_address    = 0;
static uint8_t      s_current_configuration = 0;
static bool         s_pending_class_data_out = false;

/* ============================================================
 * Small helpers
 * ============================================================ */
static inline void rmw_wait(void) {
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
}

/* EP table reset - called on bus reset. */
static void ep_table_init(void) {
    memset(&g_bl_ep_table, 0, sizeof(g_bl_ep_table));

    /* EP0 OUT: Control, armed for SETUP */
    g_bl_ep_table.EP[0].OUT.CTRL    = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_bl_ep_table.EP[0].OUT.DATAPTR = (uint16_t)g_bl_ep0_setup;
    g_bl_ep_table.EP[0].OUT.STATUS  = 0x00;

    /* EP0 IN: Control, NAK until response queued */
    g_bl_ep_table.EP[0].IN.CTRL     = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_bl_ep_table.EP[0].IN.DATAPTR  = (uint16_t)g_bl_ep0_data;
    g_bl_ep_table.EP[0].IN.STATUS   = USB_BUSNAK_bm;

    /* EP1 IN: CDC notification, NAK forever (we never send a notify) */
    g_bl_ep_table.EP[1].IN.CTRL     = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF16_gc;
    g_bl_ep_table.EP[1].IN.DATAPTR  = (uint16_t)g_bl_ep1_in;
    g_bl_ep_table.EP[1].IN.STATUS   = USB_BUSNAK_bm;

    /* EP2 OUT: CDC bulk RX, armed for first packet */
    g_bl_ep_table.EP[2].OUT.CTRL    = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_bl_ep_table.EP[2].OUT.DATAPTR = (uint16_t)g_bl_ep2_out;
    g_bl_ep_table.EP[2].OUT.STATUS  = 0x00;

    /* EP3 IN: CDC bulk TX, NAK until cdc_min arms a packet */
    g_bl_ep_table.EP[3].IN.CTRL     = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_bl_ep_table.EP[3].IN.DATAPTR  = (uint16_t)g_bl_ep3_in;
    g_bl_ep_table.EP[3].IN.STATUS   = USB_BUSNAK_bm;
}

/* ============================================================
 * EP0 helpers
 * ============================================================ */
static void ep0_stall(void) {
    g_bl_ep_table.EP[0].OUT.CTRL |= USB_DOSTALL_bm;
    g_bl_ep_table.EP[0].IN.CTRL  |= USB_DOSTALL_bm;
    s_ctrl_state = CTRL_IDLE;
}

static void ep0_start_data_in(const uint8_t *data, uint16_t len, uint16_t host_requested) {
    if (len > host_requested) len = host_requested;

    if (len <= USB_BL_EP0_SIZE) {
        /* Single-packet: copy to scratch */
        for (uint16_t i = 0; i < len; i++) g_bl_ep0_data[i] = data[i];
        g_bl_ep_table.EP[0].IN.DATAPTR = (uint16_t)g_bl_ep0_data;
        g_bl_ep_table.EP[0].IN.CNT     = len;
        g_bl_ep_table.EP[0].IN.MCNT    = 0;
        g_bl_ep_table.EP[0].IN.CTRL    = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    } else {
        /* Multi-packet (configuration descriptor at 67 B is the only
         * case here, but coding it general lets us avoid a special
         * path for any future descriptor that might grow). */
        g_bl_ep_table.EP[0].IN.DATAPTR = (uint16_t)data;
        g_bl_ep_table.EP[0].IN.CNT     = len;
        g_bl_ep_table.EP[0].IN.MCNT    = 0;
        g_bl_ep_table.EP[0].IN.CTRL    = USB_TYPE_CONTROL_gc | USB_MULTIPKT_bm
                                       | USB_AZLP_bm | USB_BUFSIZE_DEFAULT_BUF64_gc;
    }

    rmw_wait();
    USB0.STATUS[0].INCLR  = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;
    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    s_ctrl_state = CTRL_DATA_IN_STAGE;
}

static void ep0_start_data_out(uint8_t *buffer, uint16_t len) {
    /* On AVR DU, Data-stage OUT lands in the EP0.IN buffer (datasheet
     * 28.3.2.2 - control transfer OUT data flow).                    */
    g_bl_ep_table.EP[0].IN.DATAPTR = (uint16_t)buffer;
    g_bl_ep_table.EP[0].IN.CNT     = 0;
    g_bl_ep_table.EP[0].IN.MCNT    = len;
    g_bl_ep_table.EP[0].IN.CTRL    = USB_TYPE_CONTROL_gc | USB_MULTIPKT_bm | USB_BUFSIZE_DEFAULT_BUF64_gc;

    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    s_ctrl_state = CTRL_DATA_OUT_STAGE;
}

static void ep0_send_zlp(void) {
    g_bl_ep_table.EP[0].IN.CNT  = 0;
    g_bl_ep_table.EP[0].IN.MCNT = 0;
    g_bl_ep_table.EP[0].IN.CTRL = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    rmw_wait();
    USB0.STATUS[0].INCLR = USB_UNFOVF_bm | USB_TRNCOMPL_bm | USB_STALLED_bm | USB_BUSNAK_bm;

    s_ctrl_state = CTRL_STATUS_IN_STAGE;
}

/* Expose helpers to cdc_min via these wrapper names. */
void usb_min_ep0_send_zlp(void)                                     { ep0_send_zlp(); }
void usb_min_ep0_start_data_in (const uint8_t *d, uint16_t l, uint16_t h) { ep0_start_data_in(d,l,h); }
void usb_min_ep0_start_data_out(uint8_t *b, uint16_t l)             { ep0_start_data_out(b,l); s_pending_class_data_out = true; }
void usb_min_ep0_stall(void)                                        { ep0_stall(); }

/* ============================================================
 * Standard request handler
 *
 * The arduino programmer in avrdude does not need anything exotic.
 * We implement the bare-minimum subset that all USB hosts will
 * exercise during enumeration:
 *   GET_DESCRIPTOR    (device, config, string)
 *   SET_ADDRESS
 *   GET_CONFIGURATION
 *   SET_CONFIGURATION
 *   GET_STATUS        (device / interface / endpoint, return 0)
 *   CLEAR/SET_FEATURE (endpoint halt only, no-op)
 * ============================================================ */
#define REQ_GET_STATUS         0x00
#define REQ_CLEAR_FEATURE      0x01
#define REQ_SET_FEATURE        0x03
#define REQ_SET_ADDRESS        0x05
#define REQ_GET_DESCRIPTOR     0x06
#define REQ_GET_CONFIGURATION  0x08
#define REQ_SET_CONFIGURATION  0x09
#define REQ_GET_INTERFACE      0x0A
#define REQ_SET_INTERFACE      0x0B

static uint8_t  s_status_buf[2];
static uint8_t  s_single_byte_buf;

static void handle_get_descriptor(const usb_setup_t *s) {
    uint8_t  type  = (s->wValue >> 8) & 0xFF;
    uint8_t  index = s->wValue & 0xFF;
    const uint8_t *p = NULL;
    uint16_t len = 0;

    switch (type) {
    case DESC_TYPE_DEVICE:
        p = g_bl_device_descriptor;
        len = sizeof(g_bl_device_descriptor);
        break;

    case DESC_TYPE_CONFIG:
        p = g_bl_config_descriptor;
        len = sizeof(g_bl_config_descriptor);
        break;

    case DESC_TYPE_STRING:
        switch (index) {
        case 0: p = g_bl_string_langid;       len = sizeof(g_bl_string_langid);   break;
        case 1: p = g_bl_string_manufacturer; len = g_bl_string_manufacturer_len; break;
        case 2: p = g_bl_string_product;      len = g_bl_string_product_len;      break;
        case 3: p = g_bl_string_serial;       len = g_bl_string_serial_len;       break;
        default: ep0_stall(); return;
        }
        break;

    default:
        ep0_stall();
        return;
    }

    ep0_start_data_in(p, len, s->wLength);
}

static void handle_standard_request(const usb_setup_t *s) {
    switch (s->bRequest) {

    case REQ_GET_DESCRIPTOR:
        handle_get_descriptor(s);
        break;

    case REQ_SET_ADDRESS:
        /* USB 2.0 9.4.6: address must be applied AFTER the status-IN
         * stage completes (host expects to ACK at the old address). */
        s_pending_address = s->wValue & 0x7F;
        ep0_send_zlp();
        s_ctrl_state = CTRL_STATUS_PENDING_ADDR;
        break;

    case REQ_GET_CONFIGURATION:
        s_single_byte_buf = s_current_configuration;
        ep0_start_data_in(&s_single_byte_buf, 1, s->wLength);
        break;

    case REQ_SET_CONFIGURATION:
        s_current_configuration = s->wValue & 0xFF;
        ep0_send_zlp();
        /* Re-arm EP2 OUT so it accepts the first STK500 byte from
         * the host as soon as the host starts pumping. */
        rmw_wait();
        USB0.STATUS[2].OUTCLR = USB_BUSNAK_bm;
        break;

    case REQ_GET_STATUS:
        s_status_buf[0] = 0;            /* not self-powered, not RW    */
        s_status_buf[1] = 0;
        ep0_start_data_in(s_status_buf, 2, s->wLength);
        break;

    case REQ_CLEAR_FEATURE:
    case REQ_SET_FEATURE:
        /* Endpoint halt clear/set: we don't STALL any application EP,
         * so this is effectively a no-op.  ACK with ZLP.            */
        ep0_send_zlp();
        break;

    case REQ_GET_INTERFACE:
        s_single_byte_buf = 0;
        ep0_start_data_in(&s_single_byte_buf, 1, s->wLength);
        break;

    case REQ_SET_INTERFACE:
        ep0_send_zlp();
        break;

    default:
        ep0_stall();
        break;
    }
}

/* ============================================================
 * SETUP packet handler - dispatch by request type
 * ============================================================ */
static void handle_setup(void) {
    usb_setup_t *s = (usb_setup_t *)g_bl_ep0_setup;

    /* Clear stale DOSTALL on both directions for the new transfer */
    g_bl_ep_table.EP[0].OUT.CTRL = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
    g_bl_ep_table.EP[0].IN.CTRL  = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    rmw_wait();
    USB0.STATUS[0].OUTCLR = USB_EPSETUP_bm;
    rmw_wait();
    USB0.STATUS[0].INCLR  = USB_EPSETUP_bm;

    switch (s->bmRequestType & 0x60) {
    case 0x00:  handle_standard_request(s);            break;
    case 0x20:  cdc_min_handle_class_request(s);       break;
    default:    ep0_stall();                           break;
    }
}

/* ============================================================
 * EP completion handlers
 * ============================================================ */
static void handle_ep0_in_complete(void) {
    switch (s_ctrl_state) {
    case CTRL_DATA_IN_STAGE:
        s_ctrl_state = CTRL_STATUS_OUT_STAGE;
        break;

    case CTRL_STATUS_PENDING_ADDR:
        USB0.ADDR = s_pending_address;
        s_pending_address = 0;
        s_ctrl_state = CTRL_IDLE;
        break;

    case CTRL_STATUS_IN_STAGE:
    default:
        s_ctrl_state = CTRL_IDLE;
        break;
    }
}

extern void cdc_min_data_out_complete(void);

static void handle_ep0_out_complete(void) {
    switch (s_ctrl_state) {
    case CTRL_DATA_OUT_STAGE:
        if (s_pending_class_data_out) {
            s_pending_class_data_out = false;
            cdc_min_data_out_complete();
        }
        ep0_send_zlp();
        break;

    case CTRL_STATUS_OUT_STAGE:
    default:
        s_ctrl_state = CTRL_IDLE;
        break;
    }
}

/* ============================================================
 * Polled event loop
 * ============================================================ */
void usb_min_poll(void) {
    uint8_t flags_a = USB0.INTFLAGSA;
    uint8_t flags_b = USB0.INTFLAGSB;

    if (flags_a & USB_RESET_bm) {
        USB0.ADDR = 0;
        ep_table_init();
        s_ctrl_state              = CTRL_IDLE;
        s_current_configuration   = 0;
        s_pending_address         = 0;
        s_pending_class_data_out  = false;
        cdc_min_on_reset();
        USB0.INTFLAGSA = USB_RESET_bm;
    }

    if (flags_b & USB_SETUP_bm) {
        handle_setup();
        USB0.INTFLAGSB = USB_SETUP_bm;
    }

    if (flags_b & USB_TRNCOMPL_bm) {
        if (g_bl_ep_table.EP[0].IN.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[0].INCLR = USB_TRNCOMPL_bm;
            handle_ep0_in_complete();
        }
        if (g_bl_ep_table.EP[0].OUT.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[0].OUTCLR = USB_TRNCOMPL_bm;
            handle_ep0_out_complete();
        }
        if (g_bl_ep_table.EP[1].IN.STATUS & USB_TRNCOMPL_bm) {
            /* Notification EP - never sent, but clear flag defensively. */
            rmw_wait();
            USB0.STATUS[1].INCLR = USB_TRNCOMPL_bm;
        }
        if (g_bl_ep_table.EP[2].OUT.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[2].OUTCLR = USB_TRNCOMPL_bm;
            uint16_t cnt = g_bl_ep_table.EP[2].OUT.CNT;
            cdc_min_on_ep2_out(cnt);
            /* cdc_min will re-arm EP2 OUT when it has consumed
             * the bytes (clears BUSNAK).  We do NOT clear it here. */
        }
        if (g_bl_ep_table.EP[3].IN.STATUS & USB_TRNCOMPL_bm) {
            rmw_wait();
            USB0.STATUS[3].INCLR = USB_TRNCOMPL_bm;
            cdc_min_on_ep3_in_done();
        }
        USB0.INTFLAGSB = USB_TRNCOMPL_bm;
    }
}

/* ============================================================
 * Initialization / attach / detach
 * ============================================================ */
void usb_min_init(void) {
    /* OSCHF SOF auto-tune so the USB peripheral can lock onto host
     * frame timing without an external crystal.
     *
     * CRITICAL: ALGSEL must be set to INCR (incremental search) together
     * with AUTOTUNE=SOF.  Per datasheet, ALGSEL "must be written
     * simultaneously with writing the SOF setting to the AUTOTUNE bit
     * field".  ALGSEL=0 (binary search, reset default) "may overshoot
     * oscillator output frequency up to HALF the tune range", which
     * corrupts the 48 MHz USB clock mid-enumeration and causes the host
     * to reject the device.  ALGSEL=1 (incremental) nudges at most 5
     * tune steps after a reset, keeping the USB clock stable.
     *
     * Without this, cold-boot enumeration fails because the factory-
     * default OSCHF tune is ~+/-2% off and the binary search swings
     * wildly trying to refine it. */
    uint8_t oschf = CLKCTRL.OSCHFCTRLA;
    oschf &= ~(CLKCTRL_AUTOTUNE_gm | CLKCTRL_ALGSEL_bm);
    oschf |=  (CLKCTRL_AUTOTUNE_SOF_gc | CLKCTRL_ALGSEL_INCR_gc);
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, oschf);

    /* VUSB supply. VREG=1 builds (default hexes): the internal regulator
     * derives the 3.3 V VUSB rail (and D+ pull-up reference) from a 5 V VDD
     * (power configurations 5b/5s). VREG=0 builds (*_novreg hexes): the board
     * feeds 3.3 V into VUSB externally (configuration 3s, datasheet directs
     * USBVREG = 0). See Makefile / build_all_du.bat. */
#if defined(USB_VREG_INTERNAL)
    SYSCFG.VUSBCTRL = SYSCFG_USBVREG_bm;
#else
    SYSCFG.VUSBCTRL = 0;
#endif

    _delay_ms(1);

    ep_table_init();

    /* Point the USB peripheral at the EP table's EP[0]. */
    USB0.EPPTR = (uint16_t)&g_bl_ep_table.EP[0];

    /* Polled - no interrupts. */
    USB0.INTCTRLA = 0;
    USB0.INTCTRLB = 0;

    /* Enable + advertise highest endpoint number. */
    USB0.CTRLA = USB_ENABLE_bm | USB_BL_MAXEP;

    /* Wait for the USB PLL.  Cap the wait so we don't deadlock if
     * the USB clock domain is misconfigured. */
    {
        /* Wait for PLL48M to lock.  If it doesn't, we have no USB clock
         * and there is nothing useful we can do here - just keep trying.
         * (The recovery path is for the user to UPDI-reflash.) */
        uint32_t timeout = 1000000UL;
        while (!(CLKCTRL.USBPLLSTATUS & CLKCTRL_PLLS_bm) && --timeout) { }
    }

    USB0.CTRLB = 0;     /* GNAUTO/GNAK off; we manage NAK manually. */
}

void usb_min_attach(void) {
    USB0.CTRLB |= USB_ATTACH_bm;
}

void usb_min_detach(void) {
    USB0.CTRLB &= ~USB_ATTACH_bm;
}

bool usb_min_is_configured(void) {
    return s_current_configuration != 0;
}
