/**
 * usb_core.h
 * Public USB stack API
 *
 * Uses official USB_EP_TABLE_t and bit definitions from ioavr64du32.h.
 * Architecture: fully interrupt-driven (USB0_BUSEVENT + USB0_TRNCOMPL).
 * A sketch needs no usbPoll() calls; usbPoll() is a no-op kept only for
 * source compatibility.
 */
#ifndef USB_CORE_H
#define USB_CORE_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "usb_descriptors.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * USB SETUP packet structure (USB 2.0 spec 9.3, Table 9-2)
 * ============================================================ */
typedef struct __attribute__((packed)) {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_t;

/* ============================================================
 * Control transfer state machine
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
 * Public API
 * ============================================================ */
void usbInit(void);
void usbAttach(void);
void usbDetach(void);
bool usbIsConfigured(void);

/* Stage `n` bytes from a PROGMEM source into the config-descriptor accumulator.
 * Used by usb_standard.c to ship device/string descriptors that live in flash. */
void usbcore_acc_reset(void);
void usbcore_acc_load_P(const uint8_t *src_pgm, uint16_t n);
const uint8_t *usbcore_acc_buf(void);
uint16_t       usbcore_acc_len(void);

/* No-op kept for source compatibility. The stack is interrupt-driven, so
 * calling this from loop() is neither required nor has any effect. */
void usbPoll(void);

/* ============================================================
 * Internal globals shared between USB modules
 *
 * g_ep_table is USB_EP_TABLE_t (from ioavr64du32.h):
 *   FIFO[32]    : transaction-complete FIFO area
 *   EP[16]      : endpoint descriptors (16 x 16 bytes)
 *   FRAMENUM    : frame number (2 bytes)
 *
 * Access endpoints as: g_ep_table.EP[n].OUT.STATUS, .CTRL, .CNT, etc.
 * EPPTR must be set to &g_ep_table.EP[0] (NOT the struct base).
 * ============================================================ */
extern USB_EP_TABLE_t g_ep_table;

extern uint8_t  g_ep0_setup_buf[8];
extern uint8_t  g_ep0_data_buf[USB_EP0_SIZE];
extern uint8_t  g_ep1_in_buf[USB_EP1_SIZE];   /* CDC notify           */
extern uint8_t  g_ep2_out_buf[USB_EP2_SIZE];  /* CDC data RX          */
extern uint8_t  g_ep3_in_buf[USB_EP3_SIZE];   /* CDC data TX          */

extern ctrl_state_t g_ctrl_state;
extern uint8_t  g_pending_address;
extern volatile uint8_t  g_current_configuration;
extern uint8_t  g_remote_wakeup_enabled;

/* Diagnostic counters */

/* diagnostic snapshot globals (captured at the end of the TRNCOMPL ISR) */

/* ============================================================
 * Internal helpers (shared between USB modules)
 * ============================================================ */
void ep0_start_data_in(const uint8_t *data, uint16_t len, uint16_t host_requested);
void ep0_start_data_out(uint8_t *buffer, uint16_t len);
void ep0_send_zlp(void);
void ep0_stall(void);

/* RMWBUSY wait - call before any STATUS[n].xxxCLR/xxxSET write */
static inline void usb_rmw_wait(void) {
    while (USB0.INTFLAGSB & USB_RMWBUSY_bm) {}
}

#ifdef __cplusplus
}
#endif

#endif /* USB_CORE_H */
