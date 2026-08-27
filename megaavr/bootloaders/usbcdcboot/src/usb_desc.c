/*
 * usbcdcboot/src/usb_desc.c
 * --------------------------------------------------------------------
 *  Clean-room.  Reference: USB 2.0 spec chapter 9, USB CDC 1.20,
 *  PSTN 1.20.  No code from any other USB stack was consulted.
 *  License: LGPL 2.1.
 *
 *  Descriptor tables for the AVRDU CDC bootloader.  Plain CDC ACM
 *  device, two interfaces, one notification + two bulk endpoints.
 */

#include "usb_desc.h"

/* ============================================================
 * Device Descriptor (USB 2.0 Table 9-8)
 *   bDeviceClass    = 0x02 (CDC)
 *   bDeviceSubClass = 0x00
 *   bDeviceProtocol = 0x00
 * For a simple single-function CDC device this is the canonical
 * way.  No IAD needed since the bootloader has only one function.
 * ============================================================ */
const uint8_t g_bl_device_descriptor[18] = {
    18,                                 /* bLength                       */
    DESC_TYPE_DEVICE,                   /* bDescriptorType               */
    0x00, 0x02,                         /* bcdUSB = 2.00                 */
    0x02,                               /* bDeviceClass    = CDC         */
    0x00,                               /* bDeviceSubClass               */
    0x00,                               /* bDeviceProtocol               */
    USB_BL_EP0_SIZE,                    /* bMaxPacketSize0               */
    (uint8_t)(USB_BL_VID & 0xFF),
    (uint8_t)((USB_BL_VID >> 8) & 0xFF),
    (uint8_t)(USB_BL_PID & 0xFF),
    (uint8_t)((USB_BL_PID >> 8) & 0xFF),
    (uint8_t)(USB_BL_DEVICE_VER & 0xFF),
    (uint8_t)((USB_BL_DEVICE_VER >> 8) & 0xFF),
    0x01,                               /* iManufacturer                 */
    0x02,                               /* iProduct                      */
    0x03,                               /* iSerialNumber                 */
    0x01                                /* bNumConfigurations            */
};

/* ============================================================
 * Configuration Descriptor (67 bytes)
 * ============================================================ */
const uint8_t g_bl_config_descriptor[USB_BL_CONFIG_TOTAL_LEN] = {

    /* === Configuration Descriptor (9) === */
    9,                                  /* bLength                       */
    DESC_TYPE_CONFIG,                   /* bDescriptorType               */
    (uint8_t)(USB_BL_CONFIG_TOTAL_LEN & 0xFF),
    (uint8_t)(USB_BL_CONFIG_TOTAL_LEN >> 8),
    USB_BL_NUM_INTERFACES,              /* bNumInterfaces = 2            */
    1,                                  /* bConfigurationValue           */
    0,                                  /* iConfiguration                */
    0x80,                               /* bmAttributes: bus-powered     */
    50,                                 /* bMaxPower = 100 mA            */

    /* ======================================================
     * Interface 0: CDC Communication
     * ====================================================== */
    9,                                  /* bLength                       */
    DESC_TYPE_INTERFACE,                /* bDescriptorType               */
    USB_BL_IF_CDC_COMM,                 /* bInterfaceNumber              */
    0,                                  /* bAlternateSetting             */
    1,                                  /* bNumEndpoints = 1 (notify)    */
    0x02,                               /* bInterfaceClass    = CDC      */
    0x02,                               /* bInterfaceSubClass = ACM      */
    0x01,                               /* bInterfaceProtocol = AT cmds  */
    0,                                  /* iInterface                    */

    /* CDC Header functional descriptor (5) - CDC PSTN Table 15 */
    5,                                  /* bFunctionLength               */
    DESC_TYPE_CS_INTERFACE,             /* bDescriptorType (CS_IF)       */
    0x00,                               /* bDescriptorSubtype = Header   */
    0x10, 0x01,                         /* bcdCDC = 1.10                 */

    /* CDC Call Management functional descriptor (5) */
    5,                                  /* bFunctionLength               */
    DESC_TYPE_CS_INTERFACE,
    0x01,                               /* bDescriptorSubtype = CallMgmt */
    0x00,                               /* bmCapabilities: no call mgmt  */
    USB_BL_IF_CDC_DATA,                 /* bDataInterface = IF1          */

    /* CDC ACM functional descriptor (4) */
    4,                                  /* bFunctionLength               */
    DESC_TYPE_CS_INTERFACE,
    0x02,                               /* bDescriptorSubtype = ACM      */
    0x02,                               /* bmCapabilities: line coding +
                                         *   serial state notification   */

    /* CDC Union functional descriptor (5) */
    5,                                  /* bFunctionLength               */
    DESC_TYPE_CS_INTERFACE,
    0x06,                               /* bDescriptorSubtype = Union    */
    USB_BL_IF_CDC_COMM,                 /* bMasterInterface              */
    USB_BL_IF_CDC_DATA,                 /* bSlaveInterface0              */

    /* EP1 IN - Notification, interrupt, 16 B */
    7,                                  /* bLength                       */
    DESC_TYPE_ENDPOINT,
    0x81,                               /* bEndpointAddress = EP1 IN     */
    0x03,                               /* bmAttributes = Interrupt      */
    USB_BL_EP1_SIZE, 0,                 /* wMaxPacketSize = 16           */
    64,                                 /* bInterval = 64 ms             */

    /* ======================================================
     * Interface 1: CDC Data
     * ====================================================== */
    9,                                  /* bLength                       */
    DESC_TYPE_INTERFACE,
    USB_BL_IF_CDC_DATA,                 /* bInterfaceNumber              */
    0,                                  /* bAlternateSetting             */
    2,                                  /* bNumEndpoints = 2 (bulk pair) */
    0x0A,                               /* bInterfaceClass = CDC Data    */
    0x00, 0x00,                         /* SubClass, Protocol            */
    0,                                  /* iInterface                    */

    /* EP2 OUT - bulk, 64 B */
    7,                                  /* bLength                       */
    DESC_TYPE_ENDPOINT,
    0x02,                               /* bEndpointAddress = EP2 OUT    */
    0x02,                               /* bmAttributes = Bulk           */
    USB_BL_EP2_SIZE, 0,                 /* wMaxPacketSize = 64           */
    0,                                  /* bInterval (ignored for bulk)  */

    /* EP3 IN - bulk, 64 B */
    7,                                  /* bLength                       */
    DESC_TYPE_ENDPOINT,
    0x83,                               /* bEndpointAddress = EP3 IN     */
    0x02,                               /* bmAttributes = Bulk           */
    USB_BL_EP3_SIZE, 0,                 /* wMaxPacketSize = 64           */
    0                                   /* bInterval (ignored for bulk)  */
};

/* ============================================================
 * String descriptors
 * USB 2.0 chapter 9: strings are UTF-16LE; descriptor type 3.
 * String index 0 returns the supported language IDs.
 * ============================================================ */
const uint8_t g_bl_string_langid[4] = {
    4, DESC_TYPE_STRING,
    0x09, 0x04                          /* LANGID = 0x0409 (en-US)       */
};

/* Manufacturer "Anthropic-clean-room" (placeholder; users can edit
 * before building if they want a different name on the COM port).  */
const uint8_t g_bl_string_manufacturer[] = {
    2 + 2*7, DESC_TYPE_STRING,
    'A',0, 'V',0, 'R',0, 'D',0, 'U',0, 'B',0, 'L',0
};
const uint8_t g_bl_string_manufacturer_len = sizeof(g_bl_string_manufacturer);

/* Product "AVRDU CDC Bootloader" - 20 ASCII chars */
const uint8_t g_bl_string_product[] = {
    2 + 2*20, DESC_TYPE_STRING,
    'A',0, 'V',0, 'R',0, 'D',0, 'U',0, ' ',0,
    'C',0, 'D',0, 'C',0, ' ',0,
    'B',0, 'o',0, 'o',0, 't',0, 'l',0, 'o',0, 'a',0, 'd',0, 'e',0, 'r',0
};
const uint8_t g_bl_string_product_len = sizeof(g_bl_string_product);

/* Serial "BL00000001" - 10 ASCII chars (stable so the host caches the
 *                                       driver association)          */
const uint8_t g_bl_string_serial[] = {
    2 + 2*10, DESC_TYPE_STRING,
    'B',0, 'L',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0, '1',0
};
const uint8_t g_bl_string_serial_len = sizeof(g_bl_string_serial);
