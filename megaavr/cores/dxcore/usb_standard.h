/**
 * usb_standard.h
 * USB Standard / Class request dispatcher declarations
 */
#ifndef USB_STANDARD_H
#define USB_STANDARD_H

#include "usb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dispatcher entry points - called from SETUP handler */
void usb_handle_standard_request(const usb_setup_t *s);
void usb_handle_class_request(const usb_setup_t *s);

/* Called when EP0 DATA-OUT stage completes */
void usb_class_data_out_complete(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_STANDARD_H */
