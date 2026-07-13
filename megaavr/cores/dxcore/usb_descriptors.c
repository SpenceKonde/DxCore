/* AVR DU native-USB stack. Compiled only on parts with the USB peripheral. */
#include <avr/io.h>   /* defines USB0 on parts that have USB; must precede the guard */
#if defined(USB0)
/**
 * usb_descriptors.c
 * USB descriptor data: CDC-ACM + HID Composite (Keyboard + Mouse + Gamepad)
 *
 * Reference: USB 2.0, USB CDC 1.20, HID 1.11, USB IAD ECN
 */
#include "usb_descriptors.h"

/* ============================================================
 * Device Descriptor
 *   bDeviceClass    = 0xEF (Miscellaneous)
 *   bDeviceSubClass = 0x02 (Common Class)
 *   bDeviceProtocol = 0x01 (Interface Association Descriptor)
 *   Required for Windows to recognize multiple class interfaces
 *   tied together by IAD.
 * ============================================================ */
const uint8_t g_device_descriptor[18] PROGMEM = {
    18,                             /* bLength */
    DESC_TYPE_DEVICE,               /* bDescriptorType */
    0x00, 0x02,                     /* bcdUSB = 2.00 */
    0xEF, 0x02, 0x01,               /* class/subclass/protocol = IAD */
    USB_EP0_SIZE,                   /* bMaxPacketSize0 */
    (uint8_t)(USB_VID & 0xFF),
    (uint8_t)((USB_VID >> 8) & 0xFF),
    (uint8_t)(USB_PID & 0xFF),
    (uint8_t)((USB_PID >> 8) & 0xFF),
    (uint8_t)(USB_DEVICE_VER & 0xFF),
    (uint8_t)((USB_DEVICE_VER >> 8) & 0xFF),
    0x01,                           /* iManufacturer */
    0x02,                           /* iProduct */
    0x03,                           /* iSerialNumber */
    0x01                            /* bNumConfigurations */
};

/* ============================================================
 * Configuration Descriptor (150 bytes)
 * ============================================================ */
const uint8_t g_config_descriptor[CONFIG_TOTAL_LEN] PROGMEM = {
    /* === Configuration Descriptor (9) === */
    9, DESC_TYPE_CONFIG,
    (uint8_t)(CONFIG_TOTAL_LEN & 0xFF), (uint8_t)(CONFIG_TOTAL_LEN >> 8),
    USB_NUM_INTERFACES,             /* 2 interfaces (CDC Comm + CDC Data) */
    1,                              /* bConfigurationValue */
    0,                              /* iConfiguration */
    0xA0,                           /* bmAttributes: bus-powered, remote-wakeup */
    50,                             /* bMaxPower = 100mA */

    /* === Interface Association Descriptor (IAD) for CDC (8) ===
     * Tells the host: IF0 and IF1 form one CDC ACM function. */
    8, DESC_TYPE_IAD,
    CDC_COMM_INTERFACE,             /* bFirstInterface */
    2,                              /* bInterfaceCount (CDC=2 IFs) */
    0x02,                           /* bFunctionClass    = CDC */
    0x02,                           /* bFunctionSubClass = ACM */
    0x01,                           /* bFunctionProtocol = AT commands */
    0,                              /* iFunction */

    /* ======================================================
     * Interface 0: CDC Communication
     * ====================================================== */
    9, DESC_TYPE_INTERFACE,
    CDC_COMM_INTERFACE, 0,          /* bInterfaceNumber, bAlternateSetting */
    1,                              /* bNumEndpoints = 1 (notify) */
    0x02, 0x02, 0x01,               /* CDC / ACM / AT commands */
    0,                              /* iInterface */

    /* CDC Header functional descriptor (5) */
    5, DESC_TYPE_CS_INTERFACE, 0x00,
    0x10, 0x01,                     /* bcdCDC = 1.10 */

    /* CDC Call Management functional descriptor (5) */
    5, DESC_TYPE_CS_INTERFACE, 0x01,
    0x00,                           /* bmCapabilities: no call mgmt */
    CDC_DATA_INTERFACE,             /* bDataInterface */

    /* CDC ACM functional descriptor (4) */
    4, DESC_TYPE_CS_INTERFACE, 0x02,
    0x02,                           /* bmCapabilities: line coding + serial state */

    /* CDC Union functional descriptor (5) */
    5, DESC_TYPE_CS_INTERFACE, 0x06,
    CDC_COMM_INTERFACE,             /* bMasterInterface */
    CDC_DATA_INTERFACE,             /* bSlaveInterface0 */

    /* EP1 IN (notify) - interrupt, 16B, 64ms poll */
    7, DESC_TYPE_ENDPOINT,
    0x81,                           /* bEndpointAddress = 1 IN */
    0x03,                           /* bmAttributes = Interrupt */
    USB_EP1_SIZE, 0,                /* wMaxPacketSize = 16 */
    64,                             /* bInterval = 64ms */

    /* ======================================================
     * Interface 1: CDC Data
     * ====================================================== */
    9, DESC_TYPE_INTERFACE,
    CDC_DATA_INTERFACE, 0,
    2,                              /* bNumEndpoints = 2 (RX + TX) */
    0x0A, 0x00, 0x00,               /* CDC Data class */
    0,

    /* EP2 OUT (RX from host) - bulk, 64B */
    7, DESC_TYPE_ENDPOINT,
    0x02,                           /* bEndpointAddress = 2 OUT */
    0x02,                           /* bmAttributes = Bulk */
    USB_EP2_SIZE, 0,
    0,                              /* bInterval (ignored for bulk) */

    /* EP3 IN (TX to host) - bulk, 64B */
    7, DESC_TYPE_ENDPOINT,
    0x83,                           /* bEndpointAddress = 3 IN */
    0x02,                           /* bmAttributes = Bulk */
    USB_EP3_SIZE, 0,
    0,
    /* === Total: 9+8 + 9+5+5+4+5+7 + 9+7+7 = 75 bytes (matches CONFIG_TOTAL_LEN) === */
};

/* ============================================================
 * String Descriptors (UTF-16LE)
 * ============================================================ */
const uint8_t g_string_langid[4] PROGMEM = { 4, DESC_TYPE_STRING, 0x09, 0x04 };

const uint8_t g_string_manufacturer[2 + 14 * 2] PROGMEM = {
    2 + 14 * 2, DESC_TYPE_STRING,
    'D', 0, 'x', 0, 'C', 0, 'o', 0, 'r', 0, 'e', 0, ' ', 0, '-', 0,
    ' ', 0, 'A', 0, 'V', 0, 'R', 0, 'D', 0, 'U', 0
};
/* g_string_manufacturer_len replaced by a #define in usb_descriptors.h (sizeof at every call site, zero RAM). */

const uint8_t g_string_product[2 + 9 * 2] PROGMEM = {
    2 + 9 * 2, DESC_TYPE_STRING,
    'A', 0, 'V', 0, 'R', 0, 'D', 0, 'U', 0, ' ', 0, 'C', 0, 'D', 0, 'C', 0
};
/* g_string_product_len replaced by a #define in usb_descriptors.h (sizeof at every call site, zero RAM). */

const uint8_t g_string_serial[2 + 8 * 2] PROGMEM = {
    2 + 8 * 2, DESC_TYPE_STRING,
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '1', 0
};
/* g_string_serial_len replaced by a #define in usb_descriptors.h (sizeof at every call site, zero RAM). */

#endif /* USB0 */
