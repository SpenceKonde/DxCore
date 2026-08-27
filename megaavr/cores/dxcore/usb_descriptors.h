/**
 * usb_descriptors.h
 * CDC-ACM + HID Composite (IAD)
 *
 *  EP map:
 *    EP0  IN/OUT  Control 64B
 *    EP1  IN      CDC ACM notification   (Interrupt 16B)
 *    EP2  OUT     CDC data host->device  (Bulk    64B)
 *    EP3  IN      CDC data device->host  (Bulk    64B)
 *    EP4  IN      HID Keyboard           (Interrupt 8B)
 *    EP5  IN      HID Mouse              (Interrupt 8B, 4B reports)
 *    EP6  IN      HID Gamepad            (Interrupt 8B, 7B reports)
 *
 *  Interface map:
 *    IF 0   CDC Comm (IAD covers IF0..IF1)
 *    IF 1   CDC Data
 *    IF 2   HID Keyboard
 *    IF 3   HID Mouse
 *    IF 4   HID Gamepad
 *
 *  VID/PID: pid.codes test PID (0x1209:0x0002)
 */
#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include <stdint.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * Device identity
 * ============================================================ */
/* Identity (override via boards.txt build flags or variant pins_arduino.h) */
#ifndef USB_VID
  #define USB_VID               0x1209
#endif
#ifndef USB_PID
  #define USB_PID               0x0002
#endif
#define USB_DEVICE_VER          0x0100

/* ============================================================
 * Endpoint configuration
 * ============================================================ */
#define USB_EP0_SIZE            64      /* Control                */
#define USB_EP1_SIZE            16      /* CDC notify (interrupt) */
#define USB_EP2_SIZE            64      /* CDC RX     (bulk)      */
#define USB_EP3_SIZE            64      /* CDC TX     (bulk)      */
#define USB_EP4_SIZE             8      /* HID Keyboard           */
#define USB_EP5_SIZE             8      /* HID Mouse buffer       */
#define USB_EP6_SIZE             8      /* HID Gamepad buffer     */

/* Logical report sizes (what we put in CNT, what the descriptor advertises) */
#define HID_KB_REPORT_SIZE       8
#define HID_MOUSE_REPORT_SIZE    4
#define HID_GAMEPAD_REPORT_SIZE  7

#define USB_MAXEP               6       /* Highest endpoint number used */
#define USB_NUM_EP              (USB_MAXEP + 1)

/* ============================================================
 * Interface numbers
 * ============================================================ */
#define CDC_COMM_INTERFACE      0
#define CDC_DATA_INTERFACE      1
#define HID_KEYBOARD_INTERFACE  2
#define HID_MOUSE_INTERFACE     3
#define HID_GAMEPAD_INTERFACE   4
#define USB_NUM_INTERFACES      2  /* Phase 1: CDC only (HID gone, dynamic in Phase 2) */

/* ============================================================
 * Descriptor types
 * ============================================================ */
#define DESC_TYPE_DEVICE        0x01
#define DESC_TYPE_CONFIG        0x02
#define DESC_TYPE_STRING        0x03
#define DESC_TYPE_INTERFACE     0x04
#define DESC_TYPE_ENDPOINT      0x05
#define DESC_TYPE_IAD           0x0B
#define DESC_TYPE_CS_INTERFACE  0x24    /* CDC class-specific Interface */
#define DESC_TYPE_HID           0x21
#define DESC_TYPE_HID_REPORT    0x22

/* ============================================================
 * HID report descriptor lengths
 * ============================================================ */

/* ============================================================
 * Configuration descriptor layout
 *   0   ( 9)  Configuration
 *   9   ( 8)  Interface Association (CDC IF0+IF1)
 *  17   ( 9)  CDC Comm IF
 *  26   ( 5)  CDC Header functional desc
 *  31   ( 5)  CDC Call Management functional desc
 *  36   ( 4)  CDC ACM functional desc
 *  40   ( 5)  CDC Union functional desc
 *  45   ( 7)  EP1 IN (notify)
 *  52   ( 9)  CDC Data IF
 *  61   ( 7)  EP2 OUT (RX)
 *  68   ( 7)  EP3 IN  (TX)
 *  75   ( 9)  HID Keyboard IF
 *  84   ( 9)    HID class                <- KB HID at offset 84
 *  93   ( 7)    EP4 IN
 * 100   ( 9)  HID Mouse IF
 * 109   ( 9)    HID class                <- Mouse HID at offset 109
 * 118   ( 7)    EP5 IN
 * 125   ( 9)  HID Gamepad IF
 * 134   ( 9)    HID class                <- GP HID at offset 134
 * 143   ( 7)    EP6 IN
 *  Total: 150
 * ============================================================ */
#define CONFIG_TOTAL_LEN                 75   /* Phase 1: CDC + IAD only */

#define HID_KB_HID_DESC_OFFSET            84
#define HID_MOUSE_HID_DESC_OFFSET        109
#define HID_GAMEPAD_HID_DESC_OFFSET      134

/* ============================================================
 * External descriptor data
 * ============================================================ */
extern const uint8_t g_device_descriptor[18] PROGMEM;
extern const uint8_t g_config_descriptor[CONFIG_TOTAL_LEN] PROGMEM;
extern const uint8_t g_string_langid[4] PROGMEM;
extern const uint8_t g_string_manufacturer[2 + 14 * 2] PROGMEM;
extern const uint8_t g_string_product[2 + 9 * 2] PROGMEM;
extern const uint8_t g_string_serial[2 + 8 * 2] PROGMEM;

#define g_string_manufacturer_len  ((uint8_t)sizeof(g_string_manufacturer))
#define g_string_product_len  ((uint8_t)sizeof(g_string_product))
#define g_string_serial_len  ((uint8_t)sizeof(g_string_serial))

#ifdef __cplusplus
}
#endif

#endif /* USB_DESCRIPTORS_H */
