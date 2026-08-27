/**
 * usb_ep_types.h - PluggableUSB endpoint type & transfer-flag macros
 *
 * The standard Arduino HID / Keyboard / Mouse libraries (and mheironimus
 * DynamicHID) reference these constants. They are AVR-32U4-specific in the
 * upstream Arduino core, where they encode bits of the UECFG0X register.
 *
 * On the AVR DU the USB peripheral is completely different (a per-endpoint
 * descriptor table in SRAM rather than per-EP I/O registers), so we redefine
 * the symbolic values to a compact encoding that USBCore_DU.cpp decodes when
 * it programs the EP descriptor table at SET_CONFIGURATION:
 *
 *   bit 7      = direction (1 = IN, 0 = OUT)
 *   bits [1:0] = transfer type (0 ctrl, 1 iso, 2 bulk, 3 interrupt)
 *
 * The actual numeric values don't matter to the libraries - they treat them
 * as opaque - they only have to come back unchanged through PluggableUSB's
 * `endpointType[]` array.
 */
#pragma once
#if defined(USB0)

/* Endpoint type encoding (see file header) */
#define EP_TYPE_CONTROL                 0x00
#define EP_TYPE_BULK_IN                 0x82
#define EP_TYPE_BULK_OUT                0x02
#define EP_TYPE_INTERRUPT_IN            0x83
#define EP_TYPE_INTERRUPT_OUT           0x03
#define EP_TYPE_ISOCHRONOUS_IN          0x81
#define EP_TYPE_ISOCHRONOUS_OUT         0x01

/* Default endpoint max-packet size used by PluggableUSB modules.
 * HID reports are small (<=8 B) so 64 is comfortable. */
#ifndef USB_EP_SIZE
#define USB_EP_SIZE                     64
#endif

/* Transfer-control flags for USB_SendControl / USB_Send (must match Arduino) */
#define TRANSFER_PGM                    0x80
#define TRANSFER_RELEASE                0x40
#define TRANSFER_ZERO                   0x20

#endif /* USB0 */

/* ============================================================
 *  Minimal Arduino-AVR USBCore.h shims for the bundled HID library.
 *
 *  The standard HID / Keyboard / Mouse libraries (and friends like
 *  mheironimus DynamicHID, Adafruit's NeoPixel-HID demos, etc.) rely on
 *  symbols that live in the AVR core's cores/arduino/USBCore.h. DxCore's
 *  vendored api/ * does not provide them. We supply the strict subset that
 *  HID.cpp/.h actually reference, in the same shape so no library patch
 *  is required.
 * ============================================================ */

#ifndef USBCORE_COMPAT_DEFINED
#define USBCORE_COMPAT_DEFINED
#include <stdint.h>

#ifndef _AVR_TYPEDEFS_U8_U16
#define _AVR_TYPEDEFS_U8_U16
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
#endif

/* Class codes referenced by HID library */
#define USB_DEVICE_CLASS_COMMUNICATIONS    0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE   0x03

/* Endpoint helpers used by D_ENDPOINT() arguments */
#ifndef lowByte
 #define lowByte(w)  ((uint8_t)((w) & 0xff))
#endif
#ifndef highByte
 #define highByte(w) ((uint8_t)((w) >> 8))
#endif
#define USB_ENDPOINT_DIRECTION_MASK        0x80
#define USB_ENDPOINT_OUT(addr)             (lowByte((addr) | 0x00))
#define USB_ENDPOINT_IN(addr)              (lowByte((addr) | 0x80))
#define USB_ENDPOINT_TYPE_MASK             0x03
#define USB_ENDPOINT_TYPE_CONTROL          0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS      0x01
#define USB_ENDPOINT_TYPE_BULK             0x02
#define USB_ENDPOINT_TYPE_INTERRUPT        0x03

/* Standard request types used by HID class request handling */
#define REQUEST_HOSTTODEVICE               0x00
#define REQUEST_DEVICETOHOST               0x80
#define REQUEST_DIRECTION                  0x80
#define REQUEST_STANDARD                   0x00
#define REQUEST_CLASS                      0x20
#define REQUEST_VENDOR                     0x40
#define REQUEST_TYPE                       0x60
#define REQUEST_DEVICE                     0x00
#define REQUEST_INTERFACE                  0x01
#define REQUEST_ENDPOINT                   0x02
#define REQUEST_OTHER                      0x03
#define REQUEST_RECIPIENT                  0x03
#define REQUEST_DEVICETOHOST_CLASS_INTERFACE    (REQUEST_DEVICETOHOST | REQUEST_CLASS | REQUEST_INTERFACE)
#define REQUEST_HOSTTODEVICE_CLASS_INTERFACE    (REQUEST_HOSTTODEVICE | REQUEST_CLASS | REQUEST_INTERFACE)
#define REQUEST_DEVICETOHOST_STANDARD_INTERFACE (REQUEST_DEVICETOHOST | REQUEST_STANDARD | REQUEST_INTERFACE)

/* Descriptor structs used by D_INTERFACE / D_ENDPOINT */
typedef struct {
    u8  len;            /* 9 */
    u8  dtype;          /* 4 */
    u8  number;
    u8  alternate;
    u8  numEndpoints;
    u8  interfaceClass;
    u8  interfaceSubClass;
    u8  protocol;
    u8  iInterface;
} InterfaceDescriptor;

typedef struct {
    u8  len;            /* 7 */
    u8  dtype;          /* 5 */
    u8  addr;
    u8  attr;
    u16 packetSize;
    u8  interval;
} EndpointDescriptor;

typedef struct {
    u8  len;             /* 8  */
    u8  dtype;           /* 11 */
    u8  firstInterface;
    u8  interfaceCount;
    u8  functionClass;
    u8  functionSubClass;
    u8  functionProtocol;
    u8  iInterface;
} IADDescriptor;

/* Descriptor initializer macros */
#define D_INTERFACE(_n,_numEndpoints,_class,_subClass,_protocol) \
    { 9, 4, _n, 0, _numEndpoints, _class, _subClass, _protocol, 0 }

#define D_ENDPOINT(_addr,_attr,_packetSize,_interval) \
    { 7, 5, _addr, _attr, _packetSize, _interval }

#define D_IAD(_firstInterface, _count, _class, _subClass, _protocol) \
    { 8, 11, _firstInterface, _count, _class, _subClass, _protocol, 0 }

#endif /* USBCORE_COMPAT_DEFINED */
