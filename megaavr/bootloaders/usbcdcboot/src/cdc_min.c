/*
 * usbcdcboot/src/cdc_min.c
 * --------------------------------------------------------------------
 *  Clean-room implementation.  References:
 *    - USB CDC 1.20 specification
 *    - USB CDC PSTN subclass 1.20 specification (Table 13: class
 *      request codes for ACM)
 *
 *  No source from LUFA, TinyUSB, Optiboot or any other CDC stack
 *  was consulted while writing this file.
 *
 *  Minimal CDC ACM for the bootloader.  No ring buffers: RX is
 *  drained directly from the EP2 OUT buffer one byte at a time;
 *  TX accumulates bytes into a small scratch and pushes a packet
 *  when the scratch fills or cdc_min_flush() is called.
 *
 *  License: LGPL 2.1.
 */

#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "cdc_min.h"
#include "usb_desc.h"

/* ============================================================
 * Bring in the EP table and buffers declared in usb_min.c so we
 * can drive EP2 OUT / EP3 IN directly without going through an
 * abstraction layer.  These are not in usb_min.h to avoid
 * polluting the public API.
 * ============================================================ */
extern USB_EP_TABLE_t g_bl_ep_table;
extern uint8_t g_bl_ep2_out[USB_BL_EP2_SIZE];
extern uint8_t g_bl_ep3_in [USB_BL_EP3_SIZE];

/* Helpers from usb_min.c (usb_setup_t comes from usb_desc.h). */
void usb_min_ep0_send_zlp(void);
void usb_min_ep0_start_data_in (const uint8_t *d, uint16_t l, uint16_t h);
void usb_min_ep0_start_data_out(uint8_t *b, uint16_t l);
void usb_min_ep0_stall(void);

/* ============================================================
 * CDC PSTN class request codes (CDC PSTN 1.20 Table 13)
 * ============================================================ */
#define CDC_REQ_SET_LINE_CODING        0x20
#define CDC_REQ_GET_LINE_CODING        0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE 0x22
#define CDC_REQ_SEND_BREAK             0x23

#define CDC_CTRL_LINE_DTR              0x01
#define CDC_CTRL_LINE_RTS              0x02

/* ============================================================
 * CDC line coding (PSTN 1.20 Table 17): 7 bytes.
 * The bootloader doesn't drive a UART so this is purely cosmetic,
 * but the host reads/writes it during enumeration and during the
 * 1200 bps touch detection logic.
 * ============================================================ */
typedef struct __attribute__((packed)) {
    uint32_t dwDTERate;
    uint8_t  bCharFormat;
    uint8_t  bParityType;
    uint8_t  bDataBits;
} cdc_line_coding_t;

static cdc_line_coding_t s_line_coding = {
    .dwDTERate   = 115200,
    .bCharFormat = 0,
    .bParityType = 0,
    .bDataBits   = 8
};

static uint8_t s_control_line_state    = 0;
static bool    s_pending_set_line_coding = false;

/* ============================================================
 * RX state
 *   The runtime stack uses a ring buffer.  For the bootloader we
 *   leave the bytes inside the EP2 OUT buffer and consume them
 *   linearly.  When all bytes are consumed we re-arm EP2 OUT by
 *   clearing BUSNAK.
 * ============================================================ */
static volatile uint16_t s_rx_cnt   = 0;   /* total bytes in current OUT packet */
static volatile uint16_t s_rx_idx   = 0;   /* next byte to deliver to caller    */

/* ============================================================
 * TX state
 *   Bytes pile up in g_bl_ep3_in until a full 64 B packet is
 *   ready, or cdc_min_flush() is called.  When the EP3 IN
 *   transaction completes, s_tx_in_flight clears and the next
 *   pump tick can ship the next packet.
 * ============================================================ */
static volatile bool     s_tx_in_flight = false;
static volatile uint16_t s_tx_len       = 0;   /* bytes staged in g_bl_ep3_in   */

/* ============================================================
 * Note: bl_exit_via_wdt() is intentionally NOT called from this
 * file anymore.  The 1200-bps + DTR-fall pattern is the host's
 * way of saying "enter bootloader"; if we are already here, the
 * correct reaction is no reaction.
 * ============================================================ */

/* ============================================================
 * Class request handler (called from usb_min)
 * ============================================================ */
void cdc_min_handle_class_request(const usb_setup_t *s) {
    switch (s->bRequest) {

    case CDC_REQ_SET_LINE_CODING:
        if (s->wLength == sizeof(cdc_line_coding_t)) {
            s_pending_set_line_coding = true;
            usb_min_ep0_start_data_out((uint8_t *)&s_line_coding,
                                       sizeof(cdc_line_coding_t));
        } else {
            usb_min_ep0_stall();
        }
        break;

    case CDC_REQ_GET_LINE_CODING:
        usb_min_ep0_start_data_in((const uint8_t *)&s_line_coding,
                                  sizeof(cdc_line_coding_t), s->wLength);
        break;

    case CDC_REQ_SET_CONTROL_LINE_STATE: {
        uint8_t new_state = s->wValue & 0x03;
        bool dtr_falling = (s_control_line_state & CDC_CTRL_LINE_DTR)
                           && !(new_state & CDC_CTRL_LINE_DTR);
        s_control_line_state = new_state;
        usb_min_ep0_send_zlp();
        /* 1200 bps + DTR-fall is the Arduino upload "touch": the host
         * is telling us to BE in the bootloader so it can upload.  We
         * are already here, so the correct response is to do nothing
         * and let avrdude continue with STK500v1 on this same port.
         * (Earlier versions reset out of BL here, which broke upload.) */
        (void)dtr_falling;
        break;
    }

    case CDC_REQ_SEND_BREAK:
        usb_min_ep0_send_zlp();
        break;

    default:
        usb_min_ep0_stall();
        break;
    }
}

/* Called from usb_min after the EP0 DATA-OUT stage completes for
 * SET_LINE_CODING.  The bytes have already been written directly
 * into s_line_coding (we passed its address to ep0_start_data_out). */
void cdc_min_data_out_complete(void) {
    if (s_pending_set_line_coding) {
        s_pending_set_line_coding = false;
        /* Nothing else to do - DTE rate / data bits etc. are not
         * actually applied to any UART here. */
    }
}

/* ============================================================
 * Lifecycle hooks
 * ============================================================ */
void cdc_min_on_reset(void) {
    s_rx_cnt = 0;
    s_rx_idx = 0;
    s_tx_in_flight = false;
    s_tx_len = 0;
    s_control_line_state = 0;
    s_pending_set_line_coding = false;
}

void cdc_min_on_ep2_out(uint16_t cnt) {
    /* Latch this packet; cdc_min_rx_pop() will deliver bytes from it
     * one at a time.  We do NOT re-arm EP2 OUT yet - that happens
     * inside cdc_min_poll() once the last byte has been consumed. */
    if (cnt > USB_BL_EP2_SIZE) cnt = USB_BL_EP2_SIZE;
    s_rx_cnt = cnt;
    s_rx_idx = 0;
}

void cdc_min_on_ep3_in_done(void) {
    s_tx_in_flight = false;
}

/* ============================================================
 * Public init - nothing dynamic to clear here that cdc_min_on_reset
 * doesn't already cover, but expose the symbol for main.c's call. */
void cdc_min_init(void) {
    cdc_min_on_reset();
}

/* ============================================================
 *  Poll: re-arm EP2 OUT if the current OUT packet has been
 *  fully consumed, and push the staging TX buffer to EP3 IN
 *  if it is non-empty and the previous transmit has completed.
 * ============================================================ */
void cdc_min_poll(void) {
    /* RX re-arm */
    if (s_rx_cnt != 0 && s_rx_idx >= s_rx_cnt) {
        s_rx_cnt = 0;
        s_rx_idx = 0;
        g_bl_ep_table.EP[2].OUT.CNT = 0;
        while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
        USB0.STATUS[2].OUTCLR = USB_BUSNAK_bm;
    }

    /* TX pump */
    if (!s_tx_in_flight
        && s_tx_len > 0
        && (g_bl_ep_table.EP[3].IN.STATUS & USB_BUSNAK_bm)) {
        g_bl_ep_table.EP[3].IN.CNT = s_tx_len;
        while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
        USB0.STATUS[3].INCLR = USB_BUSNAK_bm;
        s_tx_in_flight = true;
        s_tx_len = 0;
    }
}

/* ============================================================
 *  RX byte fetch.  Returns -1 if no byte is currently buffered.
 * ============================================================ */
int cdc_min_rx_pop(void) {
    if (s_rx_idx >= s_rx_cnt) return -1;
    return g_bl_ep2_out[s_rx_idx++];
}

/* ============================================================
 *  TX byte push.  Buffers into the EP3 IN packet.  If the packet
 *  fills, transparently waits for the previous packet to drain
 *  by spinning on usb_min_poll() until BUSNAK comes back.
 * ============================================================ */
extern void usb_min_poll(void);

void cdc_min_tx_byte(uint8_t b) {
    /* If the staging buffer is full, ship it and wait. */
    if (s_tx_len >= USB_BL_EP3_SIZE) {
        /* Force the pump; spin on USB until the in-flight transfer
         * completes and BUSNAK is set again. */
        while (s_tx_in_flight || s_tx_len >= USB_BL_EP3_SIZE) {
            cdc_min_poll();
            usb_min_poll();
        }
    }
    g_bl_ep3_in[s_tx_len++] = b;
}

/* ============================================================
 *  TX flush.  Push whatever's staged regardless of how full the
 *  packet is.  Caller is responsible for polling afterwards if
 *  it cares about the bytes being on the wire before returning.
 * ============================================================ */
void cdc_min_flush(void) {
    /* If the staging buffer is empty there is nothing to flush. */
    if (s_tx_len == 0 && !s_tx_in_flight) return;

    /* Wait until the endpoint is ready and we have something to send. */
    while (s_tx_in_flight || s_tx_len == 0) {
        cdc_min_poll();
        usb_min_poll();
        if (!s_tx_in_flight && s_tx_len == 0) return;
    }

    /* Arm the partial packet. */
    g_bl_ep_table.EP[3].IN.CNT = s_tx_len;
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
    USB0.STATUS[3].INCLR = USB_BUSNAK_bm;
    s_tx_in_flight = true;
    s_tx_len = 0;
}
