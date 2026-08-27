/*
 * usbcdcboot/src/cdc_min.h
 * --------------------------------------------------------------------
 *  Clean-room implementation.  Reference: USB CDC 1.20 + PSTN 1.20.
 *  License: LGPL 2.1.
 *
 *  Minimal CDC ACM API for the bootloader.  Provides byte-level
 *  push/pop functions on top of the bulk IN/OUT pipes managed by
 *  usb_min.  No ring buffers - the bootloader processes one packet
 *  at a time.
 */
#ifndef AVRDU_BL_CDC_MIN_H
#define AVRDU_BL_CDC_MIN_H

#include <stdint.h>

void cdc_min_init(void);
void cdc_min_poll(void);            /* called from main loop */

/* Pop one received byte; returns -1 if none available. */
int  cdc_min_rx_pop(void);

/* Push one byte to the host.  Buffered internally; flush via the
 * regular poll cycle or explicitly via cdc_min_flush(). */
void cdc_min_tx_byte(uint8_t b);

/* Force any buffered TX bytes onto the wire immediately. */
void cdc_min_flush(void);

#endif  /* AVRDU_BL_CDC_MIN_H */
