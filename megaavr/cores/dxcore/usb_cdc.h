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

uint8_t usbCdcLineState(void);
bool    usbCdcTxInFlight(void);

/* Diagnostic: current host-requested line coding baud (dwDTERate). Useful
 * for confirming the 1200 bps touch path from a user sketch via Serial1. */
uint32_t usbCdcLineCodingBaud(void);

/* Remaining fields of the host-requested line coding, exposed so that
 * USBSerial can offer the standard Serial_ accessors (stopbits(),
 * paritytype(), numbits()). Raw CDC-ACM encodings:
 *   stop bits : 0 = 1, 1 = 1.5, 2 = 2
 *   parity    : 0 = none, 1 = odd, 2 = even, 3 = mark, 4 = space
 *   data bits : 5, 6, 7, 8 or 16 (the literal bit count) */
uint8_t usbCdcLineCodingStopBits(void);
uint8_t usbCdcLineCodingParity(void);
uint8_t usbCdcLineCodingDataBits(void);

/* Most recent CDC SEND_BREAK duration requested by the host, consumed on
 * read: returns 0..0xFFFF once, then -1 until the next request arrives
 * (0 = end break, 0xFFFF = indefinite break). Matches Serial_::readBreak()
 * in the official native-USB cores. */
int32_t usbCdcReadBreak(void);

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
