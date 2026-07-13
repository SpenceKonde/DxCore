/*
 * usbcdcboot/src/usb_min.h
 * --------------------------------------------------------------------
 *  Clean-room implementation.  License: LGPL 2.1.
 *
 *  Minimal USB device API for the AVRDU CDC bootloader.  Implements
 *  USB 2.0 Full-Speed enumeration, Standard control transfers, and
 *  bulk IN/OUT pipes only.  No interrupt EP, no HID, no Suspend
 *  handling.
 *
 *  Implementation is a reduced port of the runtime stack
 *  (AVRDU_CDC/usb_core.c) trimmed for size.  Both files share the
 *  same authorship lineage (clean-room from USB 2.0 spec + AVR DU
 *  datasheet section 21).
 */
#ifndef AVRDU_BL_USB_MIN_H
#define AVRDU_BL_USB_MIN_H

#include <stdint.h>
#include <stdbool.h>

/* Bring up the USB peripheral and EP table.  Called once at startup. */
void usb_min_init(void);

/* Drive the D+ pull-up and start enumeration. */
void usb_min_attach(void);

/* Drop D+ to force the host to re-enumerate or notice us going away. */
void usb_min_detach(void);

/* Service control / bulk EPs.  Non-blocking; called from main loop. */
void usb_min_poll(void);

/* True once the host has selected configuration 1.  Bulk EPs are
 * armed after this transition. */
bool usb_min_is_configured(void);

#endif  /* AVRDU_BL_USB_MIN_H */
