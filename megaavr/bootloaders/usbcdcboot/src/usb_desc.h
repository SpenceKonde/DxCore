/*
 * usbcdcboot/src/usb_desc.h
 * --------------------------------------------------------------------
 *  Clean-room.  Reference: USB 2.0, USB CDC 1.20 / PSTN 1.20.
 *  License: LGPL 2.1.
 *
 *  Descriptor constants and externs for the bootloader.  The
 *  bootloader exposes a single plain CDC ACM function (no IAD, no HID).
 *
 *    EP map:
 *      EP0 IN/OUT  Control                 64 B
 *      EP1 IN      CDC notification        16 B  (Interrupt, never sent)
 *      EP2 OUT     CDC data host->device   64 B  (Bulk)
 *      EP3 IN      CDC data device->host   64 B  (Bulk)
 *
 *    Interface map:
 *      IF 0   CDC Communication
 *      IF 1   CDC Data
 *
 *    VID / PID = 0x1209 / 0x0001  (pid.codes test PID)
 */
#ifndef AVRDU_BL_USB_DESC_H
#define AVRDU_BL_USB_DESC_H

#include <stdint.h>

/* Identity */
#define USB_BL_VID              0x1209
#define USB_BL_PID              0x0001
#define USB_BL_DEVICE_VER       0x0100

/* Endpoints */
#define USB_BL_EP0_SIZE         64
#define USB_BL_EP1_SIZE         16    /* CDC notify (interrupt)         */
#define USB_BL_EP2_SIZE         64    /* CDC bulk RX                    */
#define USB_BL_EP3_SIZE         64    /* CDC bulk TX                    */
#define USB_BL_MAXEP            3     /* highest EP number used         */

/* Interfaces */
#define USB_BL_IF_CDC_COMM      0
#define USB_BL_IF_CDC_DATA      1
#define USB_BL_NUM_INTERFACES   2

/* Descriptor type codes (USB 2.0 Table 9-5) */
#define DESC_TYPE_DEVICE        0x01
#define DESC_TYPE_CONFIG        0x02
#define DESC_TYPE_STRING        0x03
#define DESC_TYPE_INTERFACE     0x04
#define DESC_TYPE_ENDPOINT      0x05
#define DESC_TYPE_CS_INTERFACE  0x24    /* CDC functional descriptor    */

/* Total configuration descriptor length:
 *    9  Configuration
 *  + 9  IF0 CDC Comm
 *  + 5  CDC Header functional
 *  + 5  CDC Call Mgmt functional
 *  + 4  CDC ACM functional
 *  + 5  CDC Union functional
 *  + 7  EP1 IN  (notify)
 *  + 9  IF1 CDC Data
 *  + 7  EP2 OUT (bulk RX)
 *  + 7  EP3 IN  (bulk TX)
 *  ----
 *   67 bytes
 */
#define USB_BL_CONFIG_TOTAL_LEN  67

extern const uint8_t g_bl_device_descriptor[18];
extern const uint8_t g_bl_config_descriptor[USB_BL_CONFIG_TOTAL_LEN];
extern const uint8_t g_bl_string_langid[4];
extern const uint8_t g_bl_string_manufacturer[];
extern const uint8_t g_bl_string_product[];
extern const uint8_t g_bl_string_serial[];

extern const uint8_t g_bl_string_manufacturer_len;
extern const uint8_t g_bl_string_product_len;
extern const uint8_t g_bl_string_serial_len;

/* ============================================================
 * USB SETUP packet structure (USB 2.0 Table 9-2)
 * Defined here so both usb_min.c and cdc_min.c share the same type.
 * ============================================================ */
typedef struct __attribute__((packed)) {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_t;

#endif /* AVRDU_BL_USB_DESC_H */
