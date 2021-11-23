/*
  Copyright (c) 2016 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <inttypes.h>
#include "Stream.h"

// XXX: Those constants should be defined as const int / enums?
// XXX: shall we use namespaces too?

// Spence: Screw this - I realized these constants weren't working correctly.
// I had #undef'ed them in UART.h years ago, never bothered testing, and they
// couldn't have possibly worked, and the SERIAL_xPn constants wouldn't have
// been seen by the sketch... Nobody noticed or complained.
//
// We do not have flash to flush to waste, this code will be run with parts that have only 4k of flash
// so using an integer to hold settings, and then rearranging them at runtime, when they are ALL LOCATED
// NEXT TO EACH OTHER ON THE SAME BYTE is madness....

class HardwareSerial : public Stream
{
  public:
    virtual void begin(unsigned long) = 0;
    virtual void begin(unsigned long baudrate, uint16_t config) = 0;
    virtual void end() = 0;
    virtual int available(void) = 0;
    virtual int peek(void) = 0;
    virtual int read(void) = 0;
    virtual void flush(void) = 0;
    virtual size_t write(uint8_t) = 0;
    using Print::write; // pull in write(str) and write(buf, size) from Print
    virtual operator bool() = 0;
};

// XXX: Are we keeping the serialEvent API?
//extern void serialEventRun(void) __attribute__((weak));
