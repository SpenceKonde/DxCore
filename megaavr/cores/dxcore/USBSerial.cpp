/* USBSerial.cpp - global instance + USB bring-up for the AVR DU native-USB
 * CDC stack, plus the printHex()/printHexln() bodies.
 *
 * This library is free software released under LGPL 2.1.
 * See LICENSE.md for more information.
 * This file is part of DxCore.
 *
 * Copyright (c) 2025-2026 Yusuke Shimizu (Workshop Asahi)
 *
 * The printHex()/_prtHxdw() bodies below mirror DxCore's UART.cpp so the USB
 * Serial formats hex output identically to a hardware UART:
 *   Copyright (c) 2017-2021 Spence Konde and contributors (see UART.h).
 *
 * Originally MIT (c) Yusuke Shimizu; relicensed to LGPL 2.1 to match DxCore
 * when the shared printHex() implementation was adopted from the UART class.
 */
#include <avr/io.h>   /* defines USB0 on parts that have USB; must precede the guard */
#if defined(USB0)

/**
 * The USB stack is INTERRUPT-DRIVEN: usbInit() enables USB0_BUSEVENT
 * and USB0_TRNCOMPL, whose ISRs (in usb_core.c) service bus reset, SETUP,
 * and all endpoint transactions. The sketch needs NO usbPoll() calls and
 * no yield() hook - enumeration and data transfer happen in the background,
 * even while the sketch sits in delay().
 *
 * On startup, main() calls usb_auto_init() (declared extern "C" below) so the
 * device enumerates at boot regardless of whether the sketch ever calls
 * Serial.begin() - matching Arduino Leonardo / Micro / UNO R4, where the
 * native USB CDC stays present (and the COM port stays visible) on every
 * sketch including Blink. usb_auto_init() is idempotent, so Serial.begin()
 * remains safe.
 */
#include "USBSerial.h"

extern "C" {
  #include "usb_core.h"
  #include "usb_cdc.h"
}

USBSerial_ USBSerial;

/* ============================================================
 *  usb_auto_init  -  bring up native USB CDC (idempotent)
 * ============================================================
 *  Called automatically from main() after init()/initVariant() but
 *  before sei() on USB-capable parts (USB0 defined). Also called by
 *  USBSerial_::begin() so explicit init from the sketch is harmless.
 *  usbInit() configures OSCHF SOF autotune + the VUSB regulator, sets
 *  up the endpoint table, and enables the USB peripheral; usbAttach()
 *  connects the D+ pull-up so the host starts enumeration.
 */
extern "C" void usb_auto_init(void) {
    static bool s_usb_started = false;
    if (s_usb_started) return;
    s_usb_started = true;
    usbInit();
    usbAttach();
}

void USBSerial_::begin(unsigned long baud) {
    (void)baud;
    usb_auto_init();
}

void USBSerial_::begin(unsigned long baud, uint8_t config) {
    (void)baud;
    (void)config;
    usb_auto_init();
}

void USBSerial_::end(void) {
    /* Leave the peripheral attached; a full detach (usbDetach) would drop
     * the COM port mid-run, which is rarely what a sketch wants. */
}

/* ============================================================
 *  printHex family  -  mirrors DxCore HardwareSerial (UART.cpp)
 * ============================================================
 *  Bodies only depend on write()/println(), so they are byte-for-byte
 *  equivalent to the UART versions; only the class name differs. */
void USBSerial_::printHex(const uint8_t b) {
    char x = (b >> 4) | '0';
    if (x > '9')
        x += 7;
    write(x);
    x = (b & 0x0F) | '0';
    if (x > '9')
        x += 7;
    write(x);
}

void USBSerial_::printHex(const uint16_t w, bool swaporder) {
    uint8_t *ptr = (uint8_t *) &w;
    if (swaporder) {
        printHex(*(ptr++));
        printHex(*(ptr));
    } else {
        printHex(*(ptr + 1));
        printHex(*(ptr));
    }
}

void USBSerial_::_prtHxdw(uint8_t * ptr, bool swaporder) {
    if (swaporder) {
        printHex(*(ptr++));
        printHex(*(ptr++));
        printHex(*(ptr++));
        printHex(*(ptr));
    } else {
        ptr += 3;
        printHex(*(ptr--));
        printHex(*(ptr--));
        printHex(*(ptr--));
        printHex(*(ptr));
    }
}

uint8_t * USBSerial_::printHex(uint8_t* p, uint8_t len, char sep) {
    for (uint8_t i = 0; i < len; i++) {
        if (sep && i) write(sep);
        printHex(*p++);
    }
    println();
    return p;
}

uint16_t * USBSerial_::printHex(uint16_t* p, uint8_t len, char sep, bool swaporder) {
    for (uint8_t i = 0; i < len; i++) {
        if (sep && i) write(sep);
        printHex(*p++, swaporder);
    }
    println();
    return p;
}

volatile uint8_t * USBSerial_::printHex(volatile uint8_t* p, uint8_t len, char sep) {
    for (uint8_t i = 0; i < len; i++) {
        if (sep && i) write(sep);
        uint8_t t = *p++;
        printHex(t);
    }
    return p;
}

volatile uint16_t * USBSerial_::printHex(volatile uint16_t* p, uint8_t len, char sep, bool swaporder) {
    for (uint8_t i = 0; i < len; i++) {
        if (sep && i) write(sep);
        uint16_t t = *p++;
        printHex(t, swaporder);
    }
    return p;
}

#endif /* USB0 */
