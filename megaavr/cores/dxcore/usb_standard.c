/* AVR DU native-USB stack. Compiled only on parts with the USB peripheral. */
#include <avr/io.h>   /* defines USB0 on parts that have USB; must precede the guard */
#if defined(USB0)
/**
 * usb_standard.c
 * USB Standard Request handlers + Class request dispatcher
 *
 * Class dispatch is by interface:
 *   IF 0  CDC Comm   -> usb_cdc_handle_class_request
 *   IF 2..N (HID etc.)  -> usbcore_try_plugged_setup (PluggableUSB bridge)
 */
#include <avr/io.h>
#include <string.h>
#include "usb_core.h"
#include "usb_standard.h"
#include "usb_cdc.h"
#include "USBCore_DU.h"   /* Phase 2: PluggableUSB bridge helpers */

/* ============================================================
 * Standard request codes (USB 2.0 Table 9-4)
 * ============================================================ */
#define REQ_GET_STATUS         0x00
#define REQ_CLEAR_FEATURE      0x01
#define REQ_SET_FEATURE        0x03
#define REQ_SET_ADDRESS        0x05
#define REQ_GET_DESCRIPTOR     0x06
#define REQ_SET_DESCRIPTOR     0x07
#define REQ_GET_CONFIGURATION  0x08
#define REQ_SET_CONFIGURATION  0x09
#define REQ_GET_INTERFACE      0x0A
#define REQ_SET_INTERFACE      0x0B

#define REQ_RCPT_DEVICE        0x00
#define REQ_RCPT_INTERFACE     0x01
#define REQ_RCPT_ENDPOINT      0x02
#define REQ_RCPT_MASK          0x1F

#define FEATURE_ENDPOINT_HALT        0x00
#define FEATURE_DEVICE_REMOTE_WAKEUP 0x01

/* Static buffers for short responses */
static uint8_t s_status_buf[2];
static uint8_t s_single_byte_buf;

/* ============================================================
 * GET_DESCRIPTOR
 * ============================================================ */

static void handle_get_descriptor(const usb_setup_t *s) {

    uint8_t  type  = (s->wValue >> 8) & 0xFF;
    uint8_t  index = s->wValue & 0xFF;
    const uint8_t *p = NULL;
    uint16_t len = 0;

    switch (type) {
    case DESC_TYPE_DEVICE:
        /* Stage from PROGMEM into s_acc; ep0_start_data_in() reads from RAM. */
        usbcore_acc_reset();
        usbcore_acc_load_P(g_device_descriptor, sizeof(g_device_descriptor));
        p   = usbcore_acc_buf();
        len = usbcore_acc_len();
        break;

    case DESC_TYPE_CONFIG:
        /* Phase 2: build descriptor on the fly so PluggableUSB modules
         * (HID Keyboard, Mouse, etc., when present) can append their
         * interfaces/endpoints after the CDC IFs. With no modules
         * registered the result is byte-identical to the Phase 1 static
         * descriptor (75 B CDC-only). */
        usbcore_build_config_descriptor();
        p   = usbcore_acc_buf();
        len = usbcore_acc_len();
        break;

    case DESC_TYPE_STRING: {
        /* Stage the requested string descriptor from PROGMEM into s_acc.
         * Each USB string descriptor is small (<=30 B) so this trivially fits. */
        const uint8_t *src = NULL;
        uint16_t       sz  = 0;
        switch (index) {
        case 0: src = g_string_langid;        sz = sizeof(g_string_langid);        break;
        case 1: src = g_string_manufacturer;  sz = g_string_manufacturer_len;      break;
        case 2: src = g_string_product;       sz = g_string_product_len;           break;
        case 3: src = g_string_serial;        sz = g_string_serial_len;            break;
        default: ep0_stall(); return;
        }
        usbcore_acc_reset();
        usbcore_acc_load_P(src, sz);
        p   = usbcore_acc_buf();
        len = usbcore_acc_len();
        break;
    }

    default:
        /* Phase 2: legacy DESC_TYPE_HID / DESC_TYPE_HID_REPORT branches
         * (which sliced into the static g_config_descriptor) have been
         * removed. HID modules now own those descriptors via PluggableUSB.
         *
         * Ask the registered PluggableUSB modules (HID report descriptors
         * come in as wValue[H]=0x22 here). */
        usbcore_acc_reset();
        if (usbcore_try_plugged_get_descriptor(s)) {
            p   = usbcore_acc_buf();
            len = usbcore_acc_len();
        } else {
            ep0_stall();
            return;
        }
        break;
    }

    ep0_start_data_in(p, len, s->wLength);
}

/* ============================================================
 * GET_STATUS
 * ============================================================ */
static void handle_get_status(const usb_setup_t *s) {
    if (!(s->bmRequestType & 0x80)) { ep0_stall(); return; }
    s_status_buf[1] = 0;

    switch (s->bmRequestType & REQ_RCPT_MASK) {
    case REQ_RCPT_DEVICE:
        s_status_buf[0] = (g_remote_wakeup_enabled << 1);
        break;
    case REQ_RCPT_INTERFACE:
        s_status_buf[0] = 0;
        break;
    case REQ_RCPT_ENDPOINT: {
        uint8_t ep_num = s->wIndex & 0x0F;
        if (ep_num > USB_MAXEP) { ep0_stall(); return; }
        if (s->wIndex & 0x80)
            s_status_buf[0] = (g_ep_table.EP[ep_num].IN.CTRL & USB_DOSTALL_bm) ? 1 : 0;
        else
            s_status_buf[0] = (g_ep_table.EP[ep_num].OUT.CTRL & USB_DOSTALL_bm) ? 1 : 0;
        break;
    }
    default:
        ep0_stall();
        return;
    }
    ep0_start_data_in(s_status_buf, 2, s->wLength);
}

/* ============================================================
 * CLEAR_FEATURE / SET_FEATURE
 * ============================================================ */
static void handle_set_or_clear_feature(const usb_setup_t *s, uint8_t set) {
    switch (s->bmRequestType & REQ_RCPT_MASK) {
    case REQ_RCPT_DEVICE:
        if (s->wValue == FEATURE_DEVICE_REMOTE_WAKEUP) {
            g_remote_wakeup_enabled = set;
            ep0_send_zlp();
        } else { ep0_stall(); }
        break;
    case REQ_RCPT_ENDPOINT: {
        if (s->wValue != FEATURE_ENDPOINT_HALT) { ep0_stall(); return; }
        uint8_t ep_num = s->wIndex & 0x0F;
        if (ep_num > USB_MAXEP) { ep0_stall(); return; }
        if (s->wIndex & 0x80) {
            if (set) g_ep_table.EP[ep_num].IN.CTRL  |= USB_DOSTALL_bm;
            else     g_ep_table.EP[ep_num].IN.CTRL  &= ~USB_DOSTALL_bm;
        } else {
            if (set) g_ep_table.EP[ep_num].OUT.CTRL |= USB_DOSTALL_bm;
            else     g_ep_table.EP[ep_num].OUT.CTRL &= ~USB_DOSTALL_bm;
        }
        ep0_send_zlp();
        break;
    }
    default:
        ep0_stall();
    }
}

/* ============================================================
 * SET_ADDRESS — address is applied AFTER status-stage IN ZLP completes
 * ============================================================ */
static void handle_set_address(const usb_setup_t *s) {
    g_pending_address = s->wValue & 0x7F;
    g_ep_table.EP[0].IN.CNT  = 0;
    g_ep_table.EP[0].IN.MCNT = 0;
    g_ep_table.EP[0].IN.CTRL = USB_TYPE_CONTROL_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;

    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
    USB0.STATUS[0].INCLR = USB_BUSNAK_bm;

    g_ctrl_state = CTRL_STATUS_PENDING_ADDR;
}

/* ============================================================
 * GET_CONFIGURATION / SET_CONFIGURATION
 * ============================================================ */
static void handle_get_configuration(const usb_setup_t *s) {
    s_single_byte_buf = g_current_configuration;
    ep0_start_data_in(&s_single_byte_buf, 1, s->wLength);
}

static void handle_set_configuration(const usb_setup_t *s) {
    uint8_t cfg = s->wValue & 0xFF;

    if (cfg == 0) {
        g_ep_table.EP[1].IN.CTRL  = USB_TYPE_DISABLE_gc;
        g_ep_table.EP[2].OUT.CTRL = USB_TYPE_DISABLE_gc;
        g_ep_table.EP[3].IN.CTRL  = USB_TYPE_DISABLE_gc;
        g_ep_table.EP[4].IN.CTRL  = USB_TYPE_DISABLE_gc;
        g_ep_table.EP[5].IN.CTRL  = USB_TYPE_DISABLE_gc;
        g_ep_table.EP[6].IN.CTRL  = USB_TYPE_DISABLE_gc;
        g_current_configuration = 0;
        ep0_send_zlp();
    } else if (cfg == 1) {
        /* CDC: EP1 IN (notify), EP2 OUT (RX), EP3 IN (TX) */
        g_ep_table.EP[1].IN.CTRL    = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF16_gc;
        g_ep_table.EP[1].IN.DATAPTR = (uint16_t)g_ep1_in_buf;
        g_ep_table.EP[1].IN.STATUS  = USB_BUSNAK_bm;

        g_ep_table.EP[2].OUT.CTRL    = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
        g_ep_table.EP[2].OUT.DATAPTR = (uint16_t)g_ep2_out_buf;
        g_ep_table.EP[2].OUT.STATUS  = 0x00;  /* armed for RX */

        g_ep_table.EP[3].IN.CTRL    = USB_TYPE_BULKINT_gc | USB_BUFSIZE_DEFAULT_BUF64_gc;
        g_ep_table.EP[3].IN.DATAPTR = (uint16_t)g_ep3_in_buf;
        g_ep_table.EP[3].IN.STATUS  = USB_BUSNAK_bm;

        /* Program any PluggableUSB-allocated dynamic EPs (HID etc.). */
        usbcore_init_plugged_endpoints();
        g_current_configuration = 1;
        usb_cdc_on_configured();
        ep0_send_zlp();
    } else {
        ep0_stall();
    }
}

/* ============================================================
 * GET_INTERFACE / SET_INTERFACE
 * ============================================================ */
static void handle_get_interface(const usb_setup_t *s) {
    uint8_t iface = s->wIndex & 0xFF;
    if (iface >= USB_NUM_INTERFACES) { ep0_stall(); return; }
    s_single_byte_buf = 0;
    ep0_start_data_in(&s_single_byte_buf, 1, s->wLength);
}

static void handle_set_interface(const usb_setup_t *s) {
    uint8_t iface = s->wIndex & 0xFF;
    if (iface < USB_NUM_INTERFACES && (s->wValue & 0xFF) == 0) {
        ep0_send_zlp();
    } else {
        ep0_stall();
    }
}

/* ============================================================
 * Dispatchers
 * ============================================================ */
void usb_handle_standard_request(const usb_setup_t *s) {
    switch (s->bRequest) {
    case REQ_GET_STATUS:         handle_get_status(s);              break;
    case REQ_CLEAR_FEATURE:      handle_set_or_clear_feature(s, 0); break;
    case REQ_SET_FEATURE:        handle_set_or_clear_feature(s, 1); break;
    case REQ_SET_ADDRESS:        handle_set_address(s);             break;
    case REQ_GET_DESCRIPTOR:     handle_get_descriptor(s);          break;
    case REQ_GET_CONFIGURATION:  handle_get_configuration(s);       break;
    case REQ_SET_CONFIGURATION:  handle_set_configuration(s);       break;
    case REQ_GET_INTERFACE:      handle_get_interface(s);           break;
    case REQ_SET_INTERFACE:      handle_set_interface(s);           break;
    default:                     ep0_stall();                       break;
    }
}

void usb_handle_class_request(const usb_setup_t *s) {
    uint8_t iface = s->wIndex & 0xFF;
    if (iface == CDC_COMM_INTERFACE || iface == CDC_DATA_INTERFACE) {
        usb_cdc_handle_class_request(s);
        return;
    }
    /* Phase 2: defer all other interfaces to whichever PluggableUSB module
     * claimed them at static init time (HID via the bundled HID library,
     * XInput in a future build mode, etc.).
     *
     * host->device request WITH a data stage (HID SET_REPORT carrying the
     * keyboard-LED byte, feature reports, ...): the payload arrives in an
     * EP0 OUT data stage. Receive it first, then re-dispatch to the module
     * from usb_class_data_out_complete() so its setup() can read it via
     * USB_RecvControl(). This mirrors the CDC SET_LINE_CODING flow. */
    if (!(s->bmRequestType & 0x80) && s->wLength > 0) {
        uint16_t n = usbcore_ctrl_out_begin(s);
        if (n == 0) { ep0_stall(); return; }
        ep0_start_data_out(usbcore_ctrl_out_buf(), n);
        return;   /* continued in usb_class_data_out_complete() */
    }

    /* No data stage, or device->host: dispatch immediately. The module
     * either calls USB_SendControl to push response bytes into the
     * accumulator (which we then ship as a control-IN), or just consumes
     * the SETUP and we ZLP. */
    if (usbcore_try_plugged_setup(s)) {
        uint16_t n = usbcore_acc_len();
        if (n > 0) {
            ep0_start_data_in(usbcore_acc_buf(), n, s->wLength);
            usbcore_acc_reset();
        } else {
            ep0_send_zlp();
        }
    } else {
        ep0_stall();
    }
}

/* ============================================================
 * EP0 data-out completion -> dispatch to whichever class needs it
 *
 * Two consumers share the EP0 OUT data stage (mutually exclusive, one
 * control transfer at a time):
 *   - plugged PluggableUSB modules (HID SET_REPORT / LED, feature reports)
 *   - CDC SET_LINE_CODING (7-byte line coding)
 * ============================================================ */
extern void usb_cdc_data_out_complete(void);   /* in usb_cdc.c     */

void usb_class_data_out_complete(void) {
    /* A plugged (HID) host->device data stage takes priority: deliver the
     * staged bytes to the owning module, which reads them via USB_RecvControl. */
    if (usbcore_ctrl_out_pending()) {
        usbcore_ctrl_out_dispatch();
        return;   /* status-stage ZLP is sent by handle_ep0_out_complete() */
    }
    /* Otherwise it is the CDC SET_LINE_CODING payload. */
    usb_cdc_data_out_complete();
}

#endif /* USB0 */
