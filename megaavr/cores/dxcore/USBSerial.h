/* USBSerial.h - Arduino Stream wrapper for the AVR DU native-USB CDC stack.
 *
 * This library is free software released under LGPL 2.1.
 * See LICENSE.md for more information.
 * This file is part of DxCore.
 *
 * Copyright (c) 2025-2026 Yusuke Shimizu (Workshop Asahi)
 *
 * The printHex()/printHexln() member family (lightweight overloads inline
 * here, heavy bodies in USBSerial.cpp) mirrors the implementation in DxCore's
 * HardwareSerial.h / UART.cpp so that UART-oriented example sketches compile
 * and behave identically against the native-USB Serial:
 *   Copyright (c) 2017-2021 Spence Konde and contributors (see UART.h).
 *
 * This file was originally distributed under the MIT license (c) Yusuke
 * Shimizu; it was relicensed to LGPL 2.1 to match DxCore when the shared
 * printHex() implementation was adopted from the UART class.
 *
 * ------------------------------------------------------------------------
 * On parts that have the USB peripheral (AVR DU family -> USB0 defined) this
 * provides the `USBSerial` instance, which the DU variant aliases to `Serial`
 * (Arduino Leonardo convention: Serial = native USB CDC, Serial1 = the first
 * hardware UART).
 *
 * The stack is fully interrupt-driven (see usb_core.c): enumeration and data
 * transfer happen in the USB ISRs, so a sketch needs no usbPoll() calls:
 *
 *     Serial.begin(115200);
 *     if (Serial) {                 // host has opened the COM port (DTR)
 *       Serial.println("Hello, USB!");
 *     }
 *
 * Included near the end of HardwareSerial.h (guarded by USB0) so the
 * Serial/USBSerial symbol is declared wherever Serial is used. It depends only
 * on the ArduinoCore-API Stream/Print classes, which HardwareSerial.h has
 * already pulled in by that point - it must NOT include <Arduino.h> (that
 * would be a circular include during core compilation).
 */
#pragma once

#include <avr/io.h>   /* ensure the USB0 macro is defined before we test for it */
#if defined(USB0)

#include <stdint.h>
#include <stddef.h>
#include "api/Stream.h"

extern "C" {
  #include "usb_core.h"   /* usbInit / usbAttach / usbPoll / usbIsConfigured */
  #include "usb_cdc.h"    /* usbCdc* ring-buffer API                          */
}

class USBSerial_ : public Stream {
public:
    USBSerial_(void) {}

    /* ---- HardwareSerial pin/mux API: compatibility shims ----
     * The native USB CDC has no remappable pins, so pins()/swap() are no-ops
     * that report "not applied" (false). They mirror the HardwareSerial
     * signatures only so that UART-oriented sketches calling Serial.swap() /
     * Serial.pins() still compile against the USB Serial. */
    bool pins(uint8_t tx, uint8_t rx)   { (void)tx; (void)rx; return false; }
    bool swap(uint8_t mux_level = 1)    { (void)mux_level;     return false; }

    /** Bring up the native USB CDC (usbInit + usbAttach). The baud argument is
     *  accepted for Arduino compatibility but ignored - USB CDC always runs at
     *  full speed. Safe to call repeatedly; heavy init runs once (guarded in
     *  the .cpp). */
    void begin(unsigned long baud = 115200);
    void begin(unsigned long baud, uint8_t config);
    void end(void);

    /** True once the host has opened the virtual COM port (DTR asserted). */
    operator bool(void) const { return usbCdcReady(); }

    /* ---- Serial_ compatibility (native-USB Arduino cores) -----------------
     * The Serial_ class of the official native-USB cores exposes the CDC
     * control-line state and the host's requested line coding. Sketches use
     * them to detect a serial monitor opening (dtr()) or to mirror the
     * host's baud onto a hardware UART (a USB-to-serial bridge is the
     * classic example). Same names, same semantics, same raw encodings as
     * ArduinoCore-avr USBAPI.h / CDC.cpp. */
    bool     dtr(void)        const { return (usbCdcLineState() & 0x01) != 0; }
    bool     rts(void)        const { return (usbCdcLineState() & 0x02) != 0; }
    uint32_t baud(void)       const { return usbCdcLineCodingBaud(); }
    uint8_t  stopbits(void)   const { return usbCdcLineCodingStopBits(); }
    uint8_t  paritytype(void) const { return usbCdcLineCodingParity(); }
    uint8_t  numbits(void)    const { return usbCdcLineCodingDataBits(); }

    /** Most recent host SEND_BREAK duration, consumed on read (-1 = none). */
    int32_t  readBreak(void)        { return usbCdcReadBreak(); }

    enum {
        ONE_STOP_BIT          = 0,
        ONE_AND_HALF_STOP_BIT = 1,
        TWO_STOP_BITS         = 2,
    };
    enum {
        NO_PARITY    = 0,
        ODD_PARITY   = 1,
        EVEN_PARITY  = 2,
        MARK_PARITY  = 3,
        SPACE_PARITY = 4,
    };

    /* ---- printHex! ---- mirrors HardwareSerial (DxCore UART.cpp) ----------
     * Lightweight casting overloads are inline; the uint8_t / uint16_t bodies
     * and the pointer/array versions live in USBSerial.cpp. */
    void printHex(const   uint8_t b);                                   // in the cpp
    void printHex(const    int8_t b)             { printHex((uint8_t )  b);            }
    void printHex(const      char b)             { printHex((uint8_t )  b);            }
    void printHex(const  uint16_t w, bool s = 0);                       // in the cpp
    void printHex(const   int16_t w, bool s = 0) { printHex((uint16_t)w, s);           }
    void printHex(const  uint32_t l, bool s = 0) { _prtHxdw((uint8_t *) &l, s);        } // this lets all three 4-byte datatypes
    void printHex(const   int32_t d, bool s = 0) { _prtHxdw((uint8_t *) &d, s);        } // share the body
    void printHex(const     float f, bool s = 0) { _prtHxdw((uint8_t *) &f, s);        }
    void printHex(const    double f, bool s = 0) { _prtHxdw((uint8_t *) &f, s);        }

    /* ---- printHexln! - like printHex() with an added newline ---- */
    void printHexln(const     char b)             { printHex((uint8_t )  b); println(); }
    void printHexln(const   int8_t b)             { printHex((uint8_t )  b); println(); }
    void printHexln(const  uint8_t b)             { printHex(            b); println(); }
    void printHexln(const  int16_t w, bool s = 0) { printHex((uint16_t)w, s); println(); }
    void printHexln(const uint16_t w, bool s = 0) { printHex(         w, s); println(); }
    void printHexln(const    float f, bool s = 0) { _prtHxdw((uint8_t *) &f, s); println(); }
    void printHexln(const   double f, bool s = 0) { _prtHxdw((uint8_t *) &f, s); println(); }
    void printHexln(const  int32_t d, bool s = 0) { _prtHxdw((uint8_t *) &d, s); println(); }
    void printHexln(const uint32_t l, bool s = 0) { _prtHxdw((uint8_t *) &l, s); println(); }

    /* Pointer/array versions: gnaw off len elements and return the advanced
     * pointer (typically used to dump a peripheral register block). Bodies in
     * the cpp; the ln-variants wrap them like HardwareSerial does. */
    uint8_t  *          printHex(          uint8_t* p, uint8_t len, char sep = 0            );
    uint16_t *          printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t  * printHex(volatile  uint8_t* p, uint8_t len, char sep = 0            );
    volatile uint16_t * printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);

    uint8_t  *          printHexln(          uint8_t* p, uint8_t len, char sep = 0            ) {
      uint8_t* ret = printHex(p, len, sep);
      println(); return ret;
    }
    uint16_t *          printHexln(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0) {
      uint16_t* ret = printHex(p, len, sep, s);
      println(); return ret;
    }
    volatile uint8_t  * printHexln(volatile  uint8_t* p, uint8_t len, char sep = 0            ) {
      volatile uint8_t* ret = printHex(p, len, sep);
      println(); return ret;
    }
    volatile uint16_t * printHexln(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0) {
      volatile uint16_t* ret = printHex(p, len, sep, s);
      println(); return ret;
    }

    /* ---- Stream / Print API ---- */
    int availableForWrite(void) override { return (int)usbCdcTxFree(); }
    int available(void) override { return (int)usbCdcAvailable(); }
    int read(void) override      { return usbCdcRead(); }
    int peek(void) override      { return -1; }   /* ring buffer has no peek */

    void flush(void) override {
        /* TX is drained by the USB ISR in the background; wait until the ring
         * is empty and the last packet has gone out (bounded spin). */
        for (uint32_t guard = 0; guard < 2000000UL; guard++) {
            if (!usbCdcReady()) return;   /* port closed: nothing to wait for */
            if (usbCdcTxIdle())  return;  /* all data sent                    */
        }
    }
    size_t write(uint8_t b) override {
        return usbCdcWriteByte(b) ? 1 : 0;            /* enqueue; ISR sends it */
    }
    size_t write(const uint8_t *buffer, size_t size) override {
        return usbCdcWrite(buffer, (uint16_t)size);   /* enqueue; ISR sends    */
    }

    using Print::write;   /* pull in write(const char*), write(str,len), etc. */

private:
    /* Internal printHex for a 4-byte datatype: reads it as bytes and prints.
     * Spelled to line up with the public overloads (mirrors UART.cpp). */
    void _prtHxdw(uint8_t* p, bool s = 0);
};

extern USBSerial_ USBSerial;

/* Bring up the native USB CDC at boot (Arduino Leonardo-style USBDevice.attach()).
 * Defined in USBSerial.cpp; idempotent. Called from main() so even sketches that
 * never touch Serial (e.g. Blink) still enumerate as the COM port. */
extern "C" void usb_auto_init(void);

#endif /* USB0 */
