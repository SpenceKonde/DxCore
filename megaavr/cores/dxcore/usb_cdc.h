/**
 * usb_cdc.h
 * CDC-ACM (Virtual COM Port) implementation
 *
 *   EP1 IN   notification (interrupt 16B) — initialised but unused
 *   EP2 OUT  data RX from host            (bulk 64B)
 *   EP3 IN   data TX to   host            (bulk 64B)
 */
#ifndef USB_CDC_H
#define USB_CDC_H

#include <stdint.h>
#include <stdbool.h>
#include "usb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * Public API
 * ============================================================ */

/* True when host has SET_CONFIGURATION and asserted DTR */
bool usbCdcReady(void);
bool usbCdcTxIdle(void);   /* TX ring empty AND no packet in flight */

/* diagnostic: total raw bytes received on the CDC bulk-OUT endpoint */
extern volatile uint16_t g_cdc_rx_total;
extern volatile uint16_t g_cdc_tx_starts;
extern volatile uint16_t g_cdc_tx_pkts;
uint8_t usbCdcLineState(void);
bool    usbCdcTxInFlight(void);

/* Diagnostic: current host-requested line coding baud (dwDTERate). Useful
 * for confirming the 1200 bps touch path from a user sketch via Serial1. */
uint32_t usbCdcLineCodingBaud(void);

/* Reset-surviving breadcrumb (diagnostic). Call usbCdcDiagBreadcrumbInit()
 * once from setup(); then read the counters to see how far the last 1200bps
 * touch got even though the chip has since bounced back into the app:
 *   Saw1200   - SET_LINE_CODING(1200) reached the native CDC at least once
 *   TrigCount - trigger_1200bps_reset() was entered (the reset was requested)
 *   TrigBaud  - g_line_coding.dwDTERate captured at the last trigger
 *   TrigSite  - 1 = SET_CONTROL_LINE_STATE path, 2 = SET_LINE_CODING path */
void     usbCdcDiagBreadcrumbInit(void);
uint16_t usbCdcDiagSaw1200(void);
uint16_t usbCdcDiagTrigCount(void);
uint32_t usbCdcDiagTrigBaud(void);
uint8_t  usbCdcDiagTrigSite(void);

/* True when EP3 IN is ready to accept a fresh buffer */
bool usbCdcTxReady(void);

/* Returns number of bytes available to read from RX buffer */
uint16_t usbCdcAvailable(void);

/* Returns free slots in the TX ring (for availableForWrite()) */
uint16_t usbCdcTxFree(void);

/* Read one byte from RX buffer, returns -1 if empty */
int usbCdcRead(void);

/* Read up to maxlen bytes into dst, returns bytes actually read */
uint16_t usbCdcReadBytes(uint8_t *dst, uint16_t maxlen);

/* Write a single byte (queues; returns false on overflow) */
bool usbCdcWriteByte(uint8_t b);

/* Write a buffer; returns number actually queued */
uint16_t usbCdcWrite(const uint8_t *src, uint16_t len);

/* Convenience helpers */
uint16_t usbCdcPrint(const char *s);
uint16_t usbCdcPrintln(const char *s);

/* ============================================================
 * Internal hooks called by USB stack
 * ============================================================ */
void usb_cdc_handle_class_request(const usb_setup_t *s);
void usb_cdc_data_out_complete(void);     /* SET_LINE_CODING data done */
void usb_cdc_on_configured(void);          /* SET_CONFIGURATION(1)     */
void usb_cdc_on_reset(void);               /* Bus reset                */
void usb_cdc_on_ep2_out(uint16_t cnt);     /* EP2 OUT TRNCOMPL         */
void usb_cdc_on_ep3_in_done(void);         /* EP3 IN TRNCOMPL          */
void usb_cdc_on_sof(void);                 /* SOF: idle TX flush (ZLP) */

/* Call from main loop to drive the TX pump */
void usbCdcPoll(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_CDC_H */
