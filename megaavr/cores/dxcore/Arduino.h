/* Arduino.h - Main include file for the Arduino SDK
 * Copyright (c) 2005-2013 Arduino Team.  All right reserved.
 * And presumably from then until 2018 when this was forked
 * for megaTinyCore. Copyright 2018-2022 Spence Konde
 * Part of DxCore, which adds Arduino support for the AVR DA,
 * DB, and DD-series microcontrollers from Microchip.
 * DxCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must
 *************************************************************/

/*************************************************************
 * This file contains the stuff I think people are most likely
 * to need to refer to. The minutia has all been pushed into
 * core_devices.h if it's independent of pins_arduino.h or into
 * pinswap.h if it relates to PORTMUX, which is a great volume
 * of stuff nobody should have to read.
 *
 * That means functions and macros that may be used by user code
 * (except for some part info macros, which are described in
 * Ref_Defines.md in the documentation.
 * I also try to put detailed comments in where appropriate.
 *************************************************************/

#ifndef Arduino_h
#define Arduino_h
#include "dirty_tricks.h" //All the ugly and often dirty asm macros are here.
#include "api/ArduinoAPI.h"
#include "UART_constants.h"
#include "core_devices.h"
#include "device_timer_pins.h"
/* Gives names to all the timer pins - relies on core_devices.h being included first.*/
/* These names look like:
 * PIN_TCD0_WOC_DEFAULT
 * PIN_TCA0_WO5_ALT3
 * and so on.
 * They are #defines. Pins that don't exist are #defined as NOT_A_PIN.
 * TCA and TCD only currently */
#include <avr/pgmspace.h>
#include <avr/interrupt.h>




/* PORT names and the NOT_A_* definitions - used EVERYWHERE! */
// NOT_A_'s - 0xFF indicates a placeholder value. If you get it back for a pin, NOT_A_PIN is sometimes used to signal "no, don't set up this pin"
#define NOT_A_PIN             (0xFF)
#define NOT_A_PORT            (0xFF)
#define NOT_AN_INTERRUPT      (0xFF)
#define NOT_A_CHANNEL         (0xFF) // for channel identification on ea-series
#define NOT_A_MUX             (0xFF) // in context of peripheral swaps specified by pins, a function which got mux option from pins would return this if the pins didn't match any mux option.

#define TIMER_NOT_CONNECTED   (0xFE)
#define PERIPHERAL_IN_USE     (0xFE)
#define INVALID_PIN           (0xFE) // A distinct constant for a pin that is clearly invalid, but which we do not have to silently allow to pass through digital I/O functions.
#define TIMER_BROKEN_ERRATA   (0xFD)
#define TIMER_ALWAYS_PINLESS  (0xFC)
#define TIMER_DOES_NOT_EXIST  (0x8E)
#define NOT_A_TIMER           (0x8F) // NOT -1! That's because 0xFF we realistically might want to use for a timer, while the 8's are dedicated to things like the DAC output and the RTC channels

// One can imagine a timerToDigitalPin(uint8_t timer, uint8_t channel) function. No such function has currently been written, but it
// should be. In any case, it would need some sort of error codes.

// It would need to handle all kinds of problematic inputs - the timer channel identified does not exist at all (NOT_A_CHANNEL).
// The timer may exist, but the channel is not available because the portmux has not connected it to a pin that exists (ex, on a 14 pin part, default mux, you ask where WO2 of TCA0 is, you'd get this)
// If this channel could ever be output on a pin, this should be the error returned.
// The timer may exist, but the chip may be impacted by silicon errata impacting TCA1 (AVR128DA only) and TCD0 (all DA/DB), and while that timer should be possible to use, it's not.

// The timer may exist, but that instance of that timer can never output any pwm. It has no default or alternate pins. There may or may not be an associated portmux bitfield, but if there is
// regardless of what it is set to, this timer channel cannot output pins; the portmux full of useless options is common on low-pincount parts within a family.

// While NOT_A_PIN would seem logical to return from this, NOT_A_CHANNEL is indistinguishable from that and we want to give a different error if the channel they're asking for doesn't exist versus if the channel exists,



// for millis stuff - not timers in the usual sense (sure, the RTC is a timer, but it can't generate PWM which is what people are generally looking for). That's the only place it's currently used.
#define MUX_NONE              (128) // Very different from the above! USARTs and SPI ports have a "NONE" option which will disconnect the pins. It must be specifically requested.
// If we were certain combinations of evil, vindictive, and pedantic, we would set the PORTMUX to the NONE option when users requested a non-existent mapping.
// We instead set it to the default.
// When cast to int8_t these are -1, but it is critical to define them as 255, because these aren't always cast to int8_t's . In fact digital I/O functions do the test for whether a pin is
#define PA                    (0)
#define PB                    (1)
#define PC                    (2)
#define PD                    (3)
#define PE                    (4)
#define PF                    (5)
#define PG                    (6)
#define NUM_TOTAL_PORTS       (7) /* one could argue that this should be 6 except on 64-pin parts, and that parts that don't
have ports shoulod have those Px constants defined as NOT_A_PORT. I think that would cause problems rather than solve them, though */

#define clockCyclesPerMicrosecond() ((uint16_t)(F_CPU / 1000000L))
#define clockCyclesToMicroseconds(a) ((uint32_t)((a) / clockCyclesPerMicrosecond()))
#define microsecondsToClockCycles(a) ((uint32_t)((a) * clockCyclesPerMicrosecond()))

#define millisClockCyclesPerMicrosecond() ((uint16_t)(F_CPU / 1000000L))
#define millisClockCyclesToMicroseconds(a) ((uint32_t)((a) / clockCyclesPerMicrosecond()))
#define microsecondsToMillisClockCycles(a) ((uint32_t)((a) * clockCyclesPerMicrosecond()))



/* Timers and Timer-like-things
 * These are used for two things: Identifying the timer on a pin in
 * digitalPinToTimer(), and for the MILLIS_TIMER define that users can test to
 * verify which timer is being used for millis.
 *
 * Prior to 1.3.x TCAs were all 0x1_, TCBs 0x2_. But in order to make the
 * take-over tracking work efficiently I needed a dedicated bit for each TCA.
 * so that we can just do (__PeripheralControl | TIMERA0) to test if user has
 * taken over the timer. Hence, all the "big" timers (those which have a
 * takeOverTCxn() function and which PORTMUX moves en masse instead of one at
 * a time) have their own bit within these macros.
 * Note also that the digital_pin_to_timer table doesn't list these anymore.\
 * There are two functions within the core that need to know this:
 * AnalogWrite, and turnOffPWM. These each carry their own implementation of
 * logic to identify the timer and channel; the only other cases in which these
 * pins need to be identified are within user code, where the pin mapping can
 * be chosen freely (subject to user code needing access to other pins), so
 * it does not present the complexity of the core library which must work with
 * the pins in ANY of 7, 2, or 5 mappings (for TCA0, TCA1 and TCD0 respectively)
 *
 * Prior to this change, only the arbitrarily chosen core default timer pin-
 * mapping was supported, so this was a dramatic leap forward in capabilitt.
 *
 * The DAC is listed here because analogWrite() uses it almost exactly like
 * a PWM timer.
 * RTC can be used as a millis timekeeping source (well, not currently on
 * DxCore, but it can on megaTinyCore, and might one day be possible on DxCore
 * though I'm not sure I want to implement it that way).
 *****************************************************************************/





/* Result may be:
 * NOT_ON_TIMER - Millis is disabled.
 * NOT_A_TIMER - Millis is paused/stopped
 * MILLIS_TIMER - Millis is running normally
 * TIMER_RTC_OVF - Millis is suspended while entering or exiting standby sleep mode, from which it will wake on the OVF interrupt.
 * TIMER_RTC_CMP - Millis is suspended while entering or exiting standby sleep mode, from which it will wake on the CMP interrupt (likely using higherprescale too)
 * TIMER_RTC_PIT - Millis is suspended while entering or exiting power-down sleep mode, from which it will wake on the PIT interrupt.
 *
 * Put another way, when gCMT() != MILLIS_TIMER, millis time does not pass, and likely will not do so until execution leaves whatever function it was checked in and returns
 * to the calling code. If gCMT == MILLIS_TIMER == 0, millis is disabled and time will never pass (but delay() does work!)
 * If gCMT == NOT_A_TIMER, it's paused


 */

typedef enum _MILLIS_RTC_INT_enum
{
    _RTC_CMP = (0x02), // We should enable the CMP interrupt as we pass it back
    _RTC_OVF = (0x01), // We should enable the OVF interrupt as we pass it back
    _RTC_PIT=  (0x11), // We should enable the PIT interrupt as we pass it back
} _MILLIS_RTC_INT_t;
/* Used for the semi-internal _millisToRTC() */

#define MILLIS_RUNNING        (0x00)
#define MILLIS_PAUSED         (0x01)
#define MILLIS_ON_ALT_TIMER   (0x02)
/* High Nybble indicates timer number.
 * 0b01xx = TCD
 * 0bx01x = TCF
 * 0b





*/
#define NOT_ON_TIMER          (0x00)

#define TIMERRTC              (0x84) // RTC with internal osc
#define TIMERRTC_XTAL         (0x85) // RTC with crystal
#define TIMERRTC_CLK          (0x86) // RTC with ext clock
#define TIMER_RTC_OVF         (0x8C) // RTC used temporarily for timekeeping
#define TIMER_RTC_CMP         (0x8D) // RTC used temporarily for timekeeping
#define TIMER_RTC_PIT         (0x8E) // RTC PIT used temporarily for timekeeping

/*
TIMER AND PIN CONSTANTS
   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
0     B0  F00 F10 | D0  MUX  0  |         F20 F30     E00 A00
1     B1  F01 F11 | D0  MUX  1  |         F21 F31     E01 A01
2     B2  F02 F12 | D0  MUX  2  |         F22 F32     E02 A02
3     B3  F03 F13 | D0  MUX  3  |         F23 F33     E03 A03
4     B4  F04 F14 | D0  MUX  4  |         F24 F34     E04 A04
5     B5  F05 F15 | D0  MUX  5  |         F25 F35     E05 A05
6     B6  F06 F16 | D0  MUX  6  |         F26 F36     E06 A06
7     B7  F07 F17 | D0  MUX  7  |         F27 F37     E07 A07
8     b0  f00 f10 | D1  MUX  0  |         f20 f30     E08 A00
9     b1  f01 f11 | D1  MUX  1  |         f21 f31     E09 A01
A     b2  f02 f12 | D1  MUX  2  |         f22 f32     E0A A02
B     b3  f03 f13 | D1  MUX  3  |         f23 f33     E0B A03
C     b4  f04 f14 | D1  MUX  4  |         f24 f34     E0C A04
D     b5  f05 f15 | D1  MUX  5  |         f25 f35     E0D A05
E     b6  f06 f16 | D1  MUX  6  |         f26 f36     E0E A06
F     b7  f07 f17 | D1  MUX  7  |         f27 f37     E0F A07

*/

#define TIMERA0               (0x80) // A "simple" type A timer mapping doesn't get constants for the WO channels.
#define TIMERA1               (0x08) // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERD0               (0x40) // If any of these bits match it's potentially on TCD0
#define TIMERE0               (0x80) // I do not expect TCE and TCA to ever coexist on the same chip. TCE comes with WEX and the need for the PLL. Would they put TCA's on a part that had the infrastructre for TCEs? Doubtful
#define TIMERF0               (0x20) // TCF0 /* EB only thus far */
#define TIMERF1               (0x30) // TCF0 /* EB only thus far */
#define TIMERF2               (0xA0) // TCF0 /* EB only thus far */
#define TIMERF3               (0xB0) // TCF0 /* EB only thus far */

#define DACOUT                (0xF8) //


// 0b0001 mnnn - mapping m of TCB n
#define TIMERB0               (0x10) // TCB0
#define TIMERB0_ALT           (0x18) // TCB0 with alternate pin mapping
#define TIMERB1               (0x11) // TCB1
#define TIMERB1_ALT           (0x19) // TCB1 with alternate pin mapping
#define TIMERB2               (0x12) // TCB2
#define TIMERB2_ALT           (0x1A) // TCB2 with alternate pin mapping
#define TIMERB3               (0x13) // TCB3
#define TIMERB3_ALT           (0x1B) // TCB3 with alternate pin mapping
#define TIMERB4               (0x14) // TCB4
#define TIMERB4_ALT           (0x1C) // TCB4 with alternate pin mapping
#define TIMERB5               (0x15) // TCB5
#define TIMERB5_ALT           (0x1D) // TCB5 with alternate pin mapping
#define TIMERB6               (0x16) // TCB6
#define TIMERB6_ALT           (0x1E) // TCB6 with alternate pin mapping
#define TIMERB7               (0x17) // TCB7
#define TIMERB7_ALT           (0x1F) // TCB7 with alternate pin mapping


#if defined(TCA0)
  #define TIMERA0_MUX0        (0xC0) // Mapping0 (PORTA 0-5)
  #define TIMERA0_MUX1        (0xC1) // Mapping1 (PORTB 0-5)
  #define TIMERA0_MUX2        (0xC2) // Mapping2 (PORTC 0-5)
  #define TIMERA0_MUX3        (0xC3) // Mapping3 (PORTD 0-5)
  #define TIMERA0_MUX4        (0xC4) // Mapping4 (PORTE 0-5)
  #define TIMERA0_MUX5        (0xC5) // Mapping5 (PORTF 0-5)
  #define TIMERA0_MUX6        (0xC6) // Mapping6 (PORTG 0-5)
  #define TIMERA0_MUX7        (0xC7) // Mapping7 (PORTA 0-5)
#endif
#if defined(TCA1)
  #define TIMERA1_MUX0        (0xC8) // Mapping0 (PORTB 0-5) - 48+ pin only.
  #define TIMERA1_MUX1        (0xC9) // Mapping1 (PORTC 4-6) - only three channels available. 48+ pin only.
  #define TIMERA1_MUX2        (0xCA) // Mapping2 (PORTE 4-6) - only three channels available. DB-series only due to errata. 64-pin parts only
  #define TIMERA1_MUX3        (0xCB) // Mapping3 (PORTG 0-5) - DB-series only due to errata. 64-pin parts only.
  #define TIMERA1_MUX4        (0xCC) // Mapping4 (PORTA 4-6) - only three channels available. New on EA-series.
  #define TIMERA1_MUX5        (0xCD) // Mapping5 (PORTD 4-6) - only three channels available. New on EA-series.
#endif
#if defined(TCE0)
  #define TIMERE0_MUX0        (0xE0) // TCE/WEX mux PORTA
  #define TIMERE0_MUX1        (0xE1) // TCE/WEX mux PORTB (Assumed)
  #define TIMERE0_MUX2        (0xE2) // TCE/WEX mux PORTC
  #define TIMERE0_MUX3        (0xE3) // TCE/WEX mux PORTD
  #define TIMERE0_MUX4        (0xE4) // TCE/WEX mux PORTE (Assumed)
  #define TIMERE0_MUX5        (0xE5) // TCE/WEX mux PORTF
  #define TIMERE0_MUX6        (0xE6) // TCE/WEX mux PORTG (Assumed)
  #define TIMERE0_MUX7        (0xE7) // TCE/WEX mux (???)
  #define TIMERE0_MUX8        (0xE8) // TCE/WEX mux PORTA2
  #define TIMERE0_MUX9        (0xE9) // TCE/WEX mux PORTC2
  #define TIMERE0_MUX10       (0xEA) // TCE/WEX mux
  #define TIMERE0_MUX11       (0xEB) // TCE/WEX mux
  #define TIMERE0_MUX12       (0xEC) // TCE/WEX mux
  #define TIMERE0_MUX13       (0xED) // TCE/WEX mux
  #define TIMERE0_MUX14       (0xEE) // TCE/WEX mux
  #define TIMERE0_MUX15       (0xEF) // TCE/WEX mux
#endif
#if defined(TCE1)
  #define TIMERE1_MUX0        (0xD0)
  #define TIMERE1_MUX1        (0xD1)
  #define TIMERE1_MUX2        (0xD2)
  #define TIMERE1_MUX3        (0xD3)
  #define TIMERE1_MUX4        (0xD4)
  #define TIMERE1_MUX5        (0xD5)
  #define TIMERE1_MUX6        (0xD6)
  #define TIMERE1_MUX7        (0xD7)
  #define TIMERE1_MUX8        (0xD8)
  #define TIMERE1_MUX9        (0xD9)
  #define TIMERE1_MUX10       (0xDA)
  #define TIMERE1_MUX11       (0xDB)
  #define TIMERE1_MUX12       (0xDC)
  #define TIMERE1_MUX13       (0xDD)
  #define TIMERE1_MUX14       (0xDE)
  #define TIMERE1_MUX15       (0xDF)
#endif
#if defined(TCD0)
  #define TIMERD0_0WOA        (0x40) // PORTA
  #define TIMERD0_0WOB        (0x50)
  #define TIMERD0_0WOC        (0x60)
  #define TIMERD0_0WOD        (0x70)
  #define TIMERD0_1WOA        (0x41) // PORTB
  #define TIMERD0_1WOB        (0x51)
  #define TIMERD0_1WOC        (0x61)
  #define TIMERD0_1WOD        (0x71)
  #define TIMERD0_2WOA        (0x42) // PORTF
  #define TIMERD0_2WOB        (0x52)
  #define TIMERD0_2WOC        (0x62)
  #define TIMERD0_2WOD        (0x72)
  #define TIMERD0_3WOA        (0x43) // PORTG
  #define TIMERD0_3WOB        (0x53)
  #define TIMERD0_3WOC        (0x63)
  #define TIMERD0_3WOD        (0x73)
  #define TIMERD0_4WOA        (0x44) // this is PA4, duplicates mux 0.
  #define TIMERD0_4WOB        (0x54) // this is PA5, duplicates mux 0.
  #define TIMERD0_4WOC        (0x64) // second half is PORTD
  #define TIMERD0_4WOD        (0x74)
  #define TIMERD0_5WOA        (0x45) // hypothetical TCD0 WOA ALT5
  #define TIMERD0_5WOB        (0x55) // hypothetical TCD0 WOB ALT5
  #define TIMERD0_5WOC        (0x65) // hypothetical TCD0 WOC ALT5
  #define TIMERD0_5WOD        (0x75) // hypothetical TCD0 WOD ALT5
  #define TIMERD0_6WOA        (0x46) // hypothetical TCD0 WOA ALT6
  #define TIMERD0_6WOB        (0x56) // hypothetical TCD0 WOB ALT6
  #define TIMERD0_6WOC        (0x66) // hypothetical TCD0 WOC ALT6
  #define TIMERD0_6WOD        (0x76) // hypothetical TCD0 WOD ALT6
  #define TIMERD0_7WOA        (0x47) // hypothetical TCD0 WOA ALT7
  #define TIMERD0_7WOB        (0x57) // hypothetical TCD0 WOB ALT7
  #define TIMERD0_7WOC        (0x67) // hypothetical TCD0 WOC ALT7
  #define TIMERD0_7WOD        (0x77) // hypothetical TCD0 WOD ALT7
#endif
#if defined(TCD1)
  #define TIMERD1_0WOA        (0x48)
  #define TIMERD1_0WOB        (0x58)
  #define TIMERD1_0WOC        (0x68)
  #define TIMERD1_0WOD        (0x78)
  #define TIMERD1_1WOA        (0x49)
  #define TIMERD1_1WOB        (0x59)
  #define TIMERD1_1WOC        (0x69)
  #define TIMERD1_1WOD        (0x79)
  #define TIMERD1_2WOA        (0x4A)
  #define TIMERD1_2WOB        (0x5A)
  #define TIMERD1_2WOC        (0x6A)
  #define TIMERD1_2WOD        (0x7A)
  #define TIMERD1_3WOA        (0x4B)
  #define TIMERD1_3WOB        (0x5B)
  #define TIMERD1_3WOC        (0x6B)
  #define TIMERD1_3WOD        (0x7C)
  #define TIMERD1_4WOA        (0x4C)
  #define TIMERD1_4WOB        (0x5C)
  #define TIMERD1_4WOC        (0x6C)
  #define TIMERD1_4WOD        (0x7C)
  #define TIMERD1_5WOA        (0x4D)
  #define TIMERD1_5WOB        (0x5D)
  #define TIMERD1_5WOC        (0x6D)
  #define TIMERD1_5WOD        (0x7D)
  #define TIMERD1_6WOA        (0x4E)
  #define TIMERD1_6WOB        (0x5E)
  #define TIMERD1_6WOC        (0x6E)
  #define TIMERD1_6WOD        (0x7E)
  #define TIMERD1_7WOA        (0x4F)
  #define TIMERD1_7WOB        (0x5F)
  #define TIMERD1_7WOC        (0x6F)
  #define TIMERD1_7WOD        (0x7F)
#endif
#if defined(TCF0)
  #define TIMERF0_0WO0        (0x20) // Confirmed TCF0 WOA MUX default: PA0
  #define TIMERF0_0WO1        (0x28) // Confirmed TCF0 WOB MUX default: PA1
  #define TIMERF0_1WO0        (0x21) // Confirmed TCF0 WOA MUX ALT1: PA6
  #define TIMERF0_1WO1        (0x29) // Confirmed TCF0 WOB MUX ALT1: PA7
  #define TIMERF0_2WO0        (0x22) // Confirmed TCF0 WOA MUX ALT2: PF4
  #define TIMERF0_2WO1        (0x2A) // Confirmed TCF0 WOB MUX ALT2: PF5
  #define TIMERF0_3WO0        (0x23) // Hypothetical TCF MUX
  #define TIMERF0_3WO1        (0x2B) // Hypothetical TCF MUX
  #define TIMERF0_4WO0        (0x24) // Hypothetical TCF MUX
  #define TIMERF0_4WO1        (0x2C) // Hypothetical TCF MUX
  #define TIMERF0_5WO0        (0x25) // Hypothetical TCF MUX
  #define TIMERF0_5WO1        (0x2D) // Hypothetical TCF MUX
  #define TIMERF0_6WO0        (0x26) // Hypothetical TCF MUX
  #define TIMERF0_6WO1        (0x2E) // Hypothetical TCF MUX
  #define TIMERF0_7WO0        (0x27) // Hypothetical TCF MUX
  #define TIMERF0_7WO1        (0x2F) // Hypothetical TCF MUX
#endif
#if defined(TCF1)
  #define TIMERF1_0WO0        (0x30) // Hypothetical TCF
  #define TIMERF1_0WO1        (0x38) // Hypothetical TCF
  #define TIMERF1_1WO0        (0x31) // Hypothetical TCF
  #define TIMERF1_1WO1        (0x39) // Hypothetical TCF
  #define TIMERF1_2WO0        (0x32) // Hypothetical TCF
  #define TIMERF1_2WO1        (0x3A) // Hypothetical TCF
  #define TIMERF1_3WO0        (0x33) // Hypothetical TCF
  #define TIMERF1_3WO1        (0x3B) // Hypothetical TCF
  #define TIMERF1_4WO0        (0x34) // Hypothetical TCF
  #define TIMERF1_4WO1        (0x3C) // Hypothetical TCF
  #define TIMERF1_5WO0        (0x35) // Hypothetical TCF
  #define TIMERF1_5WO1        (0x3D) // Hypothetical TCF
  #define TIMERF1_6WO0        (0x36) // Hypothetical TCF
  #define TIMERF1_6WO1        (0x3E) // Hypothetical TCF
  #define TIMERF1_7WO0        (0x37) // Hypothetical TCF
  #define TIMERF1_7WO1        (0x3F) // Hypothetical TCF
#endif
#if defined(TCF2)
  #define TIMERF2_0WO0        (0xA0) // Hypothetical TCF
  #define TIMERF2_0WO1        (0xA8) // Hypothetical TCF
  #define TIMERF2_1WO0        (0xA1) // Hypothetical TCF
  #define TIMERF2_1WO1        (0xA9) // Hypothetical TCF
  #define TIMERF2_2WO0        (0xA2) // Hypothetical TCF
  #define TIMERF2_2WO1        (0xAA) // Hypothetical TCF
  #define TIMERF2_3WO0        (0xA3) // Hypothetical TCF
  #define TIMERF2_3WO1        (0xAB) // Hypothetical TCF
  #define TIMERF2_4WO0        (0xA4) // Hypothetical TCF
  #define TIMERF2_4WO1        (0xAC) // Hypothetical TCF
  #define TIMERF2_5WO0        (0xA5) // Hypothetical TCF
  #define TIMERF2_5WO1        (0xAD) // Hypothetical TCF
  #define TIMERF2_6WO0        (0xA6) // Hypothetical TCF
  #define TIMERF2_6WO1        (0xAE) // Hypothetical TCF
  #define TIMERF2_7WO0        (0xA7) // Hypothetical TCF
  #define TIMERF2_7WO1        (0xAF) // Hypothetical TCF
#endif
#if defined(TCF3)
  #define TIMERF3_0WO0        (0xB0) // Hypothetical TCF
  #define TIMERF3_0WO1        (0xB8) // Hypothetical TCF
  #define TIMERF3_1WO0        (0xB1) // Hypothetical TCF
  #define TIMERF3_1WO1        (0xB9) // Hypothetical TCF
  #define TIMERF3_2WO0        (0xB2) // Hypothetical TCF
  #define TIMERF3_2WO1        (0xBA) // Hypothetical TCF
  #define TIMERF3_3WO0        (0xB3) // Hypothetical TCF
  #define TIMERF3_3WO1        (0xBB) // Hypothetical TCF
  #define TIMERF3_4WO0        (0xB4) // Hypothetical TCF
  #define TIMERF3_4WO1        (0xBC) // Hypothetical TCF
  #define TIMERF3_5WO0        (0xB5) // Hypothetical TCF
  #define TIMERF3_5WO1        (0xBD) // Hypothetical TCF
  #define TIMERF3_6WO0        (0xB6) // Hypothetical TCF
  #define TIMERF3_6WO1        (0xBE) // Hypothetical TCF
  #define TIMERF3_7WO0        (0xB7) // Hypothetical TCF
  #define TIMERF3_7WO1        (0xBF) // Hypothetical TCF
#endif

/* PIN DETERMINATION
 * Check TCA0, TCA1, TCE0 programmatically (the strong portmux aware timers)
 * Get the value from the table. If it's NOT_ON_TIMER, stop.
 * if tableval & TIMERD0 -> Timer D 0b 01aa nmmm
 * n = 0 until we have a TCD1 if we ever do.
 * if tablevel & TIMERB0 -> Timer B - 0b001m


*/

/* Analog reference options - Configuring these is very simple, unlike tinyAVR 0/1
   and megaAVR 0, and like tinyAVR 2-series.  The available references are the same
   On Dx and Ex*/
#define INTERNAL1V024            (VREF_REFSEL_1V024_gc)
#define INTERNAL2V048            (VREF_REFSEL_2V048_gc)
#define INTERNAL4V096            (VREF_REFSEL_4V096_gc)
#define INTERNAL4V1              (VREF_REFSEL_4V096_gc)
#define INTERNAL2V5              (VREF_REFSEL_2V500_gc)
#define INTERNAL2V500            (VREF_REFSEL_2V500_gc)
#define DEFAULT                  (VREF_REFSEL_VDD_gc)
#define VDD                      (VREF_REFSEL_VDD_gc)
#define EXTERNAL                 (VREF_REFSEL_VREFA_gc)
#define AC_REF_1V024             (VREF_AC0REFSEL_1V024_gc)
#define AC_REF_2V048             (VREF_AC0REFSEL_2V048_gc)
#define AC_REF_2V5               (VREF_AC0REFSEL_2V5_gc)
#define AC_REF_4V096             (VREF_AC0REFSEL_4V096_gc)
#define AC_REF_VDD               (VREF_AC0REFSEL_AVDD_gc)
#define AC_REF_4V1               AC_REF_4V096/* Alias */

#if !(F_CPU >= 32000000)
  #define TIMEBASE_1US        (((F_CPU + 999999UL)/1000000UL) << ADC_TIMEBASE_gp)
#else
  #define TIMEBASE_1US        (31 << ADC_TIMEBASE_gp)
#endif

// Defines something as an ADC channel, as opposed to a digital pin number
// This is the same convention that ATTinyCore uses, with high bit indicating
// that a value is a channel number not a pin number.
#define ADC_CH(ch)                (0x80 | (ch))

#if !defined(ADC0_TEMP2) // Dx-series
  #if defined(DAC0)
    #define ADC_DAC0          ADC_CH(ADC_MUXPOS_DAC0_gc)
  #endif
  #define ADC_DACREF0       ADC_CH(0x49)
  #ifdef AC1 // Always either 1 AC or 3 until the Ex-series
    #define ADC_DACREF1     ADC_CH(0x4A)
  #endif
  #ifdef AC2
    #define ADC_DACREF2     ADC_CH(0x4B)
  #endif
  #define ADC_GROUND        ADC_CH(ADC_MUXPOS_GND_gc)
  #define ADC_TEMPERATURE   ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)

  #ifdef MVIO
    #define ADC_VDDDIV10    ADC_CH(ADC_MUXPOS_VDDDIV10_gc)
    #define ADC_VDDIO2DIV10 ADC_CH(ADC_MUXPOS_VDDIO2DIV10_gc)
  #endif

  /* end special channels, start options */
  #define ADC_ACC2                (0x81)
  #define ADC_ACC4                (0x82)
  #define ADC_ACC8                (0x83)
  #define ADC_ACC16               (0x84)
  #define ADC_ACC32               (0x85)
  #define ADC_ACC64               (0x86)
  #define ADC_ACC128              (0x87)
  #define ADC_ACC256        badArg(">128 sample accumulation is only supported on Ex-series")
  #define ADC_ACC512        badArg(">128 sample accumulation is only supported on Ex-series")
  #define ADC_ACC1024       badArg(">128 sample accumulation is only supported on Ex-series")
  #define ADC_ACC2S         badArg(">128 sample accumulation is only supported on Ex-series")
  #define ADC_ACC4S         badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC8S         badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC16S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC32S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC64S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC128S       badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC256S       badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC512S       badArg("Sign chopping is only supported on Ex-series")
  #define ADC_ACC1024S      badArg("Sign chopping is only supported on Ex-series")
  //Decimation
  #define ADC_RES13               (0x0D)
  #define ADC_RES14               (0x0E)
  #define ADC_RES15               (0x0F)
  #define ADC_RES16         badArg("The maximum resolution via decimation is 15 bits on Dx")
  #define ADC_RES17         badArg("The maximum resolution via decimation is 15 bits on Dx")
  //Sign Chopping Decimation
  #define ADC_RES13S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_RES14S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_RES15S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_RES16S        badArg("Sign chopping is only supported on Ex-series")
  #define ADC_RES17S        badArg("Sign chopping is only supported on Ex-series")
  //power options
  #define ADC_ENABLE              (0x20)
  #define ADC_DISABLE             (0x30)
  #define ADC_STANDBY_ON          (0xC0)
  #define ADC_STANDBY_OFF         (0x90)
#else // Ex-series
  #define ADC_TEMPERATURE   ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
  #define ADC_GROUND        ADC_CH(ADC_MUXPOS_GND_gc)
  #if defined(DAC0)
    #define ADC_DAC0        ADC_CH(0x38)
  #endif
  #define ADC_DACREF0       ADC_CH(0x39)
  #if defined(AC1) // Always either 1 AC or 3 until the Ex-series
    #define ADC_DACREF1     ADC_CH(0x3A)
  #endif

  #define ADC_VDDDIV10     ADC_CH(ADC_MUXPOS_VDDDIV10_gc)
  //Accumulation
  #define ADC_ACC2                (0x81)
  #define ADC_ACC4                (0x82)
  #define ADC_ACC8                (0x83)
  #define ADC_ACC16               (0x84)
  #define ADC_ACC32               (0x85)
  #define ADC_ACC64               (0x86)
  #define ADC_ACC128              (0x87)
  #define ADC_ACC256              (0x88)
  #define ADC_ACC512              (0x89)
  #define ADC_ACC1024             (0x8A)
  //Sign Chopping Accumulation
  #define ADC_ACC2S               (0xA1)
  #define ADC_ACC4S               (0xA2)
  #define ADC_ACC8S               (0xA3)
  #define ADC_ACC16S              (0xA4)
  #define ADC_ACC32S              (0xA5)
  #define ADC_ACC64S              (0xA6)
  #define ADC_ACC128S             (0xA7)
  #define ADC_ACC256S             (0xA8)
  #define ADC_ACC512S             (0xA9)
  #define ADC_ACC1024S            (0xAA)
//Decimation
  #define ADC_RES13               (0x0D)
  #define ADC_RES14               (0x0E)
  #define ADC_RES15               (0x0F)
  #define ADC_RES16               (0x10)
  #define ADC_RES17               (0x11)
//Sign Chopping Decimation
  #define ADC_RES13S              (0x4D)
  #define ADC_RES14S              (0x4E)
  #define ADC_RES15S              (0x4F)
  #define ADC_RES16S              (0x50)
  #define ADC_RES17S              (0x51)
// Power Options
  #define LOW_LAT_ON              (0x03) // deprecated
  #define LOW_LAT_OFF             (0x02) // deprecated
  #define ADC_LOWLAT_ON           (0x03)
  #define ADC_LOWLAT_OFF          (0x02)
  #define PGA_KEEP_ON             (0x08)
  #define PGA_AUTO_OFF            (0x0C)
  #define PGA_OFF_ONCE            (0x04)
  #define ADC_ENABLE              (0x20)
  #define ADC_DISABLE             (0x30)
  #define ADC_STANDBY_ON          (0xC0)
  #define ADC_STANDBY_OFF         (0x90)
#endif



#define ADC_ERROR_DISABLED                          (-32767) /* ADC is disabled. Please enable it or use analogReadEnh(), or don't take analog readings. */
#define ADC_ERROR_BUSY                              (-32766) /* Not currently generated */
#define ADC_ERROR_BAD_PIN_OR_CHANNEL                (-32765) /* Pin is not a valid ADC pin */
#define ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL       (-2100000000) /* Pin, or Positive pin for differential, is not a valid ADC pin */
#define ADC_ENH_ERROR_BUSY                     (-2100000002) /* Not currently generated */
#define ADC_ENH_ERROR_RES_TOO_LOW              (-2100000003) /* Requested resolution is less than 8 bits */
#define ADC_ENH_ERROR_RES_TOO_HIGH             (-2100000004) /* Requested resolution exceeds what can be generated using builtin accumulation and decimation */
#define ADC_DIFF_ERROR_BAD_NEG_PIN             (-2100000005) /* See datasheet or Analog reference, not all pins can be negative. */
#define ADC_ENH_ERROR_NOT_DIFF_ADC             (-2100000006) /* Likely can't ever be returned*/
#define ADC_ENH_ERROR_DISABLED                 (-2100000007) /* Not Used - we re-enable, take the reading, and turn off again if analogReadEnh() is called when ADC off. */
#define ADC_ERROR_INVALID_CLOCK                     (-32255)


// only returned by analogCheckError()
#define ADC_IMPOSSIBLE_VALUE                        (-127)





/*
Supplied by Variant file:
#define digitalPinToAnalogInput(p)      // Given digital pin (p), returns the analog channel number, or NOT_A_PIN if the pin does not suipport analog input.
#define analogChannelToDigitalPin(p)    // Inverse of above. Given analog chanbel number (p) in raw form not ADC_CH() form, returns the digital pin number corresponding to it.
#define analogInputToDigitalPin(p)      // Similar to previous. Given analog input number (p) with the high bit set, returns the digital pin number corresponding to it)
#define digitalOrAnalogPinToDigital(p)  // Given either an analog input number (with high bit set) or a digital pin number (without it set), returns the digital pin number.
Yes, these are poorly named and do not use analog input, analog pin, and analog channel consistently. Unfortunately the names of some of these were set in stone by virtue of their being preexisting macros used in code in the wild.
See Ref_Analog.md for more information of the representations of "analog pins". I blame Arduino for the original sin of "analog pins" as a concept in the first place.
*/


#define digitalPinToPort(pin)               ((((uint8_t)(pin))      < NUM_TOTAL_PINS ) ?                          digital_pin_to_port[pin]                 : NOT_A_PIN)
#define digitalPinToBitPosition(pin)        ((((uint8_t)(pin))      < NUM_TOTAL_PINS ) ?                  digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define digitalPinToBitMask(pin)            ((((uint8_t)(pin))      < NUM_TOTAL_PINS ) ?                      digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?               digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?                   digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#if !defined(digitalPinToTimer)
  // Allow variants to provide their own definition of digitalPinToTimer.
  #define digitalPinToTimer(pin)            ((((uint8_t)(pin))     < NUM_TOTAL_PINS ) ?                          digital_pin_to_timer[pin]                 : NOT_ON_TIMER)
#endif
#define portToPortStruct(port)              ((((uint8_t)(port))    < NUM_TOTAL_PORTS) ?                   (((PORT_t *)  &PORTA) + (port))                  : NULL)
#define digitalPinToPortStruct(pin)         ((((uint8_t)(pin))     < NUM_TOTAL_PINS ) ?    (((PORT_t *) &PORTA) + digitalPinToPort( pin))                  : NULL)
#define getPINnCTRLregister(port, bit_pos)  ((((port) != NULL) && (bit_pos < 8))      ? (((volatile uint8_t *) &(port->PIN0CTRL)) + bit_pos)               : NULL)
#define digitalPinToInterrupt(P)            (P)
// Remember to test for NOT_A_PORT before using thiese.
#define portOutputRegister(P) ((volatile uint8_t *)(&portToPortStruct(P)->OUT))
#define portInputRegister(P)  ((volatile uint8_t *)(&portToPortStruct(P)->IN ))
#define portModeRegister(P)   ((volatile uint8_t *)(&portToPortStruct(P)->DIR))
#if defined(PORTA_EVGENCTRLA) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.
  #define portEventRegister(p)  ((volatile uint8_t *)(&portToPortStruct(P)->EVGENCTRL))
#endif
#define digitalPinHasPWMNow(p)            (digitalPinToTimerNow(p) != NOT_ON_TIMER)

#define interrupts() sei()
#define noInterrupts() cli()


#include "pins_arduino.h"
/* The Variant file must do one of the following */
/* 1. Use the same pin order as this core's default pin mapping (recommended)
 * 2. Number each pin (port * 8) + bit_position, and define HYPERRATIONAL_PIN_NUMBERS (also recommended)
 * 3. Define NONCANONICAL_PIN_NUMBERS and use any pin numbering. (recommended if you must use a layout that departs significantly from the above)
 * 4. Define SPECIAL_PIN_NUMBERS, and provide a _digitalPinToCanon(pin) macro that takes an Arduino pin number, and returns (port * 8) + bit_position
 *    (Only if you can do it better than the standard noncanonical implementation - that implementation is not grotesque, but it's also not great.
 *    each table lookup takes the form lds lds add adc ld, 7 words and 10 clocks, so the whole thing is probably on the order of 20 and 26)
 * This change permits underlying logic to be written with a single byte to represent a pin, which is present in some obscure parts of the code
 * mostly involving interrupts.
 * Note that for constant pins known at compile time, these should all be able to be constant folded, it's only compile time unknown pins
 * where this applies. And only for the rare cases where we end up doing this, often interrupt related.
 * A lot of this comes back to the question of whether to leave "holes" for missing pins in the numbering. There are two forces pulling in
 * opposite directions here: each ghost pin takes up 4b for it's entries in the pin table (and it's sort of absurd for a 28-pin part to have
 * 47 logical pins because they were missing PB, PE, and 4 pins of PC and PF, but they make this sort of conversion (and a number of
 * similar ones) much easier.
 */

#if defined(NONCANONICAL_PIN_NUMBERS)
  #define _digitalPinToCanon(pin) (((pin) < NUM_TOTAL_PINS) ? ((digital_pin_to_port[pin] << 3) + digital_pin_to_bit_position[pin] ) : NOT_A_PIN)
#elif defined(HYPERRATIONAL_PIN_NUMBERS) /* Variant must number pins in order, and must skip numbers of pins not present on the chip. */
  #define _digitalPinToCanon(pin) (((pin) < NUM_TOTAL_PINS) ? (pin) : NOT_A_PIN)
#elif !defined(SPECIAL_PIN_NUMBERS)
  #if _AVR_PINCOUNT == 64
    #define _digitalPinToCanon(pin) (((pin) < NUM_TOTAL_PINS) ? (((pin) < PIN_PG0) ? (pin) : (((pin) > PIN_PG7) ? (pin) - 8 : (pin) + 2 )) : NOT_A_PIN)
  #elif _AVR_PINCOUNT == 48
    #define _digitalPinToCanon(pin) (((pin) < NUM_TOTAL_PINS) ? (((pin) < PIN_PC0) ? (pin) : (pin) + 2 ) : NOT_A_PIN)
  #elif _AVR_PINCOUNT == 32
    #define _digitalPinToCanon(pin) (((pin) < NUM_TOTAL_PINS) ? (((pin) <= PIN_PA7) ? (pin) : (((pin) < PIN_PD0) ? (pin) + 8 : (((pin) < PIN_PF0) ? (pin) + 12 : (pin) + 20 ))) : NOT_A_PIN)
  #elif _AVR_PINCOUNT == 28
    #define _digitalPinToCanon(pin) (((pin) <= PIN_PF1) ? (((pin) <= PIN_PA7) ? (pin) : (((pin) < PIN_PD0) ? (pin) + 8 : (((pin) < PIN_PF0) ? (pin) + 12 : (pin) + 20 ))) : (((pin) < NUM_TOTAL_PINS) ? (pin) + 16 : NOT_A_PIN))
  #elif _AVR_PINCOUNT == 20 || _AVR_PINCOUNT == 14
    #define _digitalPinToCanon(pin) (((pin) < PIN_PF6) ? (((pin) <= PIN_PC0) ? (pin) : (((pin) < PIN_PD0) ? (pin) + 8 : (pin) + 12)) : (((pin) < NUM_TOTAL_PINS) ? (pin) + 26 : NOT_A_PIN))
  #endif
#else
  #if !defined(_digitalPinToCanon)
    #error "Your custom variant says it provides a _digitalPinToCanon (SPECIAL_PIN_NUMBERS defined) but you don't provide one. \n Define NONCANONICAL_PIN_NUMBERS instead to use a possibly slower handler for the general case"
  #endif
#endif
  // this stuff used to be in the variants.
#if !defined(NUM_DIGITAL_PINS)
/* Despite the name, this actually is a number 1 higher than the highest valid number for a digital pin
 * that is, it's the first integer which does not refer to a pin, and the number of digital pins if there
 * were no gaps in the numbering. Almost every pin mapping has gaps.
 * Tests like if (pin >= NUM_DIGITAL_PIN) return; are ubiquitous.
 * So we need to make our NUM_DIGITAL_PINS work like that.
 */
  #if defined(PIN_PG7) // if there's a PORTG, that's the last pin. Add 1 to get the first non-pin
    #define NUM_DIGITAL_PINS            (PIN_PG7 + 1)
  #elif defined(PIN_PF7)  // if the UPDI pin, PF7 is defined (ie, UPDI can be prorgrammed as GPIO)
    #define NUM_DIGITAL_PINS            (PIN_PF7 + 1)
  #elif defined(PIN_PF6)  // otherwise it should be the reset pin, PG6.
    #define NUM_DIGITAL_PINS            (PIN_PF6 + 1)
  #else
    #error "The variant file is incorrect, as it indicates no PG7, PF7 or PF6. All supported and announced parts have one or more of those pins."
  #endif
#endif
#if !defined(NUM_RESERVED_PINS)
  #define NUM_RESERVED_PINS             (0)
#endif
#if !defined(NUM_INTERNALLY_USED_PINS)
  #if ((CLOCK_SOURCE & 0x03) == 1)
    #define NUM_INTERNALLY_USED_PINS    (2) // External crystal takes PA0 and PA1
  #elif ((CLOCK_SOURCE & 0x03) == 2)
    #define NUM_INTERNALLY_USED_PINS    (1) // External clock takes out PA0
  #else
    #define NUM_INTERNALLY_USED_PINS    (0)
  #endif
#endif
#if !defined(NUM_I2C_PINS)
  #define NUM_I2C_PINS                  (2) // per I2C port in use - this number is nonsensical without qualification is is only for compatibility.
#endif
#if !defined(NUM_SPI_PINS)
  #define NUM_SPI_PINS                  (3) // per SPI port in use - this number is nonsensical without qualification is is only for compatibility.
#endif
#if !defined(NUM_TOTAL_FREE_PINS)
  #define NUM_TOTAL_FREE_PINS           (PINS_COUNT - NUM_INTERNALLY_USED_PINS)
#endif
#if !defined(NUM_TOTAL_PINS)
  #define NUM_TOTAL_PINS                (NUM_DIGITAL_PINS) /* Used the same way as NUM_DIGITAL_PINS. so it doesn't mean what it's named  - I didn't make the convention*/
#endif
#define CHANNEL0_UNCHANGED    (0x40)
#define CHANNEL1_UNCHANGED    (0x41)

#ifdef __cplusplus
  extern "C"{
#endif


extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];



/* we call badArg() when we know at compile time that one or more arguments passed to
 * the function is nonsensical or doomed to generate useless output. For example,
 * analogWrite() on a constant pin that will never support PWM, or digitalWrite() on
 * a pin number that is neither a pin nor NOT_A_PIN (which silently does nothing for
 * compatibility).
 * badCall() on the other hand is called if we know that regardless of what arguments
 * are passed, that function is nonsensical with current settings, for example, millis()
 * when millis timekeeping has been disabled */

#if !defined(LTODISABLED)
  void badArg(const char*)    __attribute__((error("")));
  void badCall(const char*)   __attribute__((error("")));
  // The fast digital I/O functions only work when the pin is known at compile time.
  inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
  {
    if(!__builtin_constant_p(pin))
      badArg("Fast digital pin must be a constant");
  }
#else
  void badArg(const char*);
  void badCall(const char*);
  void check_constant_pin(pin_size_t pin);
  #if defined(ARDUINO_MAIN) // We need to make sure these substitutes for the badArg and badCall functions are generated once and only once.
    // They must not attempt to actually detect any error if LTO is disabled
    void badArg(__attribute__((unused))const char* c) {
      return;
    }
    void badCall(__attribute__((unused))const char* c) {
      return;
    }
    void check_constant_pin(__attribute__((unused))pin_size_t pin) {
      return;
    }
  #endif

  // Intentionally outside of the above #if so that your console gets fucking spammed with this warning.
  // The linker errors you turned off LTO to better understand will still be at the bottom.
  #warning "LTO is disabled. digitalWriteFast(), digitalReadFast(), pinModeFast() and openDrainFast() are unavailable, delayMicroseconds() for short delays and delay() with millis timing disabled is less accuratetest. Unsupported forms of 'new' compile without errors (but always return a NULL pointer). Additionally, functions which normally generate a compile error when passed a value that is known to be invalid at compile time will not do so. The same is true of functions which are not valid with the currently selected tools submenu options."
  #warning "This mode is ONLY for debugging LINK-TIME ERRORS that are reported by the linker as being located at .text+0, and you can't figure out where the bug is from other information it provides. As noted above, while this may make compilation succeed, it will only turn compile-time errors into incorrect runtime behavior, which is much harder to debug. As soon as the bug that forced this to be used is fixed, switch back to the standard platform.txt!"
  #warning "UART implementation is forcibly downgraded, Flash.h writes are replaced with NOPs, and pin interrupts are downgraded to the old (less efficient) implementation. All uploading is disabled, and behavior of exported binaries may vary from normal behavior arbitrarily."
#endif




inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN) {
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      badArg("Digital pin is constant, but not a valid pin");
    }
    #if (((CLOCK_SOURCE & 0x03) == 2))
      #if defined(MEGATINYCORE)
        if (pin == PIN_PA3) {
          badArg("Constant digital pin PIN_PA3 is used for the external osc, and is not available for other uses.");
        }
      #else
        if (pin == PIN_PA0) {
          badArg("Constant digital pin PIN_PA0 is used for the external osc, and is not available for other uses.");
        }
      #endif
    #elif ((CLOCK_SOURCE & 0x03) == 1)
      if (pin < 2) {
        badArg("Pin PA0 and PA1 cannot be used for digital I/O because those are used for external crystal clock.");
      }
    #elif defined(XTAL_PINS_HARDWIRED)
      if (pin < 2) {
        badArg("On the selected board, PA0 and PA1 are hardwired to the crystal. They may not be used for other purposes.");
      }
    #endif
    }
}


/*******************************************************
 * Extended API - Enhanced DxCore features             *
 * These are functions users might call from the       *
 * sketch, or in some cases, override.                 *
 * 1. Timer + millis() control.                        *
 * 2. initialization routines to override              *
 * 3. Advanced Analog functionality                    *
 * 4. Advanced Digital functionality                   *
 ******************************************************/
// callbacks normally empty and optimized away.
void onPreMain();
void onBeforeInit();
uint8_t onAfterInit();
void initVariant();
/* inlining of a call to delayMicroseconds() would throw it off */

void _delayMicroseconds(unsigned int us) __attribute__((noinline));
// stop, restart and set millis intended for switching to RTC for millis timekeeping while sleeping.
void stop_millis();                          // stop the timer being used for millis, and disable the interrupt.
void restart_millis();                       // After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.
void set_millis(uint32_t newmillis);         // Sets the millisecond timer to the specified number of milliseconds. DO NOT CALL with a number lower than the current millis count if you have any timeouts ongoing.
                                             // they may expire instantly.
void nudge_millis(uint16_t nudgemillis);     // Sets the millisecond timer forward by the specified number of milliseconds.

uint8_t _getCurrentMillisTimer();

/* semi-internal and subject to change */
/* We needed a bit more core integration to make sleepTime work. */
uint32_t _millisToRTC(_MILLIS_RTC_INT_t RTCmode);
uint8_t _millisFromRTC(uint32_t m);

uint8_t digitalPinToTimerNow(uint8_t p);     // Returns the timer that is associated with the pin now (considering PORTMUX)
// Allows for user to mark a timer "do not touch" for purposes of analogWrite and the like, so you can take over a timer and reconfigure it, and not worry about digitalWrite() flipping a CMPEN bit.
// On megaTinyCore this also prevents the situation where PWM is remapped, but then when the user is digitalWrite'ing pins that default to having PWM, it would turn off the PWM now coming from another pin
// Only type A, D, and E timers can be "taken over". Only Type A and E timers can be resumed.
// analogWrite() and turnOffPWM() will not attempt to reconfigure a TCB or TCF not set as PWM - so that is the only signal needed.
void takeOverTCA0();                         // Can be used to tell core not to use TCA0 for any API calls - user has taken it over.
void takeOverTCA1();                         // Can be used to tell core not to use TCA1 for any API calls - user has taken it over.
void takeOverTCD0();                         // Can be used to tell core not to use TCD0 for any API calls - user has taken it over.
void takeOverTCE0();                         // *Take over? You and what army? Where are your TCAs? TCE - make this prisoner talk!* "If he's not singin' like a bird..." *Use the dead-time generator if he tries anything* "Yes, my master"
void resumeTCA0();                           // Restores core-mediated functionality that uses TCA0 and restores default TCA0 configuration.
void resumeTCA1();                           // Restores core-mediated functionality that uses TCA1 and restores default TCA1 configuration.
void resumeTCE0();                           // Restores core-mediated functionality that uses TCE0 and restores default TCE0 configuration.
// bool digitalPinHasPWM(uint8_t p);         // Macro. Returns true if the pin can currently output PWM using analogWrite(), regardless of which timer is used and considering current PORTMUX setting
void init_ADC0(void); /* Called by init() after clock is set */
#if defined(ADC1)
  void init_ADC1(void); /* Never called automatically, but must be called manuaklkly in order to use the ADC1 functions. */
#endif
void init_timers();   /* called by init()        */
// These are in here so that - should it be necessary - library functions or user code could override these.
void init_clock()     __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()      __attribute__((weak)); // this is called to initialize ADC0
#if defined(ADC1)
  void init_ADC1();   __attribute__((weak)); /* Never called automatically, but must be called manuaklkly in order to use the ADC1 functions. */
#endif
//   init_DAC0()                             // no _init_DAC0() - all that the core does is call DACReference!
void init_TCA0()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA0 will not function.
void init_TCA1()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA1 will not function, nor will the TCBs unless the clock source is changed.
void init_TCE0()      __attribute__((weak)); // Even ne'er do well's that associate with villains like WEX Luther need initiation! "You mean initialization" "I'm not sure I do..."
void init_TCF0()      __attribute__((weak)); // called by init_timers()
void init_TCBs()      __attribute__((weak)); // called by init_timers()
void init_TCD0()      __attribute__((weak)); // called by init_timers()
void init_millis()    __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.
void onClockFailure() __attribute__((weak)); // called by the clock failure detection ISR. Default action is a blink code with 4 blinks.
void onClockTimeout() __attribute__((weak)); // called if we try to switch to external clock, but it doesn't work. Default action is a blink code with 3 blinks.

// DIGITAL I/O EXTENDED FUNCTIONS
// Covered in documentation.
void                  openDrain(uint8_t pinNumber,   uint8_t val);
#if !defined(LTODISABLED)
  int8_t        digitalReadFast(uint8_t pinNumber               );
  void         digitalWriteFast(uint8_t pinNumber,   uint8_t val);
  void              pinModeFast(uint8_t pinNumber,  uint8_t mode);
  void            openDrainFast(uint8_t pinNumber,   uint8_t val);
#endif


void                 turnOffPWM(uint8_t pinNumber               );
uint8_t          PWMoutputTopin(uint8_t timer, uint8_t channel);
// Get the bit location within the hardware port of the given virtual pin.
// This comes from the arrays in the  pins_arduino.c file for the active
// board configuration.
#if !defined(CORE_ATTACH_OLD)
// The old attachInterrupt did not require any calls to be made to enable a port.
// It would just grab every port, and take over every port's pin interrupt ISR, so nobody could define one themselves.
// which wouldn't be so bad... except that attachInterrupt interrupts are slow as hell
  void attachPortAEnable();
  void attachPortBEnable();
  void attachPortCEnable();
  void attachPortDEnable();
  void attachPortEEnable();
  void attachPortFEnable();
  void attachPortGEnable();
#endif
#if defined(PORTA_EVGENCTRLA) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.
  uint8_t   _getPortEventConfig(uint8_t port); // just shorthand for looking up the port and returning it's EVGENCTRL value
  uint8_t    _getRTCEventConfig(); //simply returns the RTC channel configuration. Will likely return 255 if called on non Ex
  uint8_t     _RTCPrescaleToVal(uint16_t prescale);
  uint8_t          _setEventPin(uint8_t pin,   uint8_t number); // preliminary thought - pass a pin number, it looks up port, and from there the event control register and sets it.
  uint8_t      _setRTCEventChan(uint8_t val,   uint8_t chan); // number is 0, 1 or 255 like above, div is log(2) of the divisor (ie, for 2^5, his would be 5).
  uint8_t      _setRTCEventChan(uint8_t val,   uint8_t chan);
#endif
// ANALOG EXTENDED FUNCTIONS
// Covered in documentation.

int32_t           analogReadEnh(uint8_t pin,                uint8_t res,  uint8_t gain);
int32_t          analogReadDiff(uint8_t pos,  uint8_t neg,  uint8_t res,  uint8_t gain);
int16_t        analogClockSpeed(int16_t frequency, uint8_t options);
bool       analogReadResolution(uint8_t res);
bool       analogSampleDuration(uint8_t dur);
void               DACReference(uint8_t mode);

int8_t  getAnalogReadResolution();
uint8_t         getDACReference();
uint8_t      getAnalogReference();
uint8_t getAnalogSampleDuration();




#ifdef __cplusplus
} // extern "C"
#endif





 #ifdef __cplusplus
  #include "UART.h"

  //uint8_t digitalPinToTimerNow(uint8_t p);=
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int16_t analogClockSpeed(int16_t frequency = 0,  uint8_t options = 0);
#endif


#if defined(LTODISABLED) && defined(FAKE_FAST_IO)
  // Should be enabled if, and only if, you are debugging something that you can't debug with LTO enabled, AND
  // your code makes use of the fast functions. Note that this drastically alters behavior of code that calls them, taking, in some cases, two orders of magnitude longer
  #warning "FAKE_FAST_IO code should **never** be expected towork correctly running on hardware. It is just here to provide a way to get past missing definition errors when you are forced to disable LTO for debugging."
  #define      digitalReadFast(pinNumber)         digitalRead(pinNumber)
  #define     digitalWriteFast(pinNumber, val)    digitalWrite(pinNumber, val)
  #define          pinModeFast(pinNumber, mode)   pinMode(pinNumber, mode)
  #define        openDrainFast(pinNumber, val)    openDrain(pinNumber, val)
#endif

/*******************************************************************
 * PIN CONFIGURE Set any or all pin settings, easily invert, etc   *
 *******************************************************************
 * These are used as the second argument to pinConfigure(pin, configuration)
 * You can bitwise OR as many of these as you want, or just do one. Very
 * flexible function; not the world's fastest though. Directives are handled
 * in the order they show up on this list, by pin function:
 * PIN_DIR      Direction
 * PIN_OUT      Output value
 * PIN_ISC      Enable and interrupt mode. If interrupts are turned on w/out the ISR, it will trigger dirty reset.
 * PIN_PULLUP   Pullups
 * PIN_INLVL    Input levels (MVIO parts only - everything else is schmitt trigger only, except on I2C pins acting as I2C with SMBus levels enabled. )
 * PIN_INVERT   Invert pin
 *
 * Systematically named constants can be made by combining those names with the postfixes here
 * except for PIN_ISC which is not a non-binary option. Valid values are listed below.
 * _SET, _CLR, and _TGL can be used as a postfix on all binary options.
 * _TOGGLE and _TGL are interchangeable as well.
 * Additional names are defined where they might be easier to remember.
 * It's not an accident that the PORT options have PIN_(name of register in PORTx)
 * as an alias.
 * Microchip can add one more binary option >.>                    */


/* External definitions */
/* Actual implementation is in wiring_extra.cpp
 * Because of the incrutable rules of C++ scoping, you can define an inline function or a template function in a header....
 * and not in the body of a separate file, while the opposite is true for ANY OTHER KIND OF FUNCTION. */

void __pinconfigure(const uint8_t digital_pin, uint16_t pin_config);
void _pinconfigure(uint8_t pin, uint16_t pin_config);
void pinConfigure(uint8_t digital_pin, uint16_t pin_config);

#ifdef __cplusplus
  typedef enum : uint16_t {
   // OUTPUT
    PIN_DIR_SET        = 0x0001,
    PIN_DIRSET         = 0x0001,
    PIN_DIR_OUTPUT     = 0x0001,
    PIN_DIR_OUT        = 0x0001,
   // INPUT
    PIN_DIR_CLR        = 0x0002,
    PIN_DIRCLR         = 0x0002,
    PIN_DIR_INPUT      = 0x0002,
    PIN_DIR_IN         = 0x0002,
   // TOGGLE INPUT/OUTPUT
    PIN_DIR_TGL        = 0x0003,
    PIN_DIRTGL         = 0x0003,
    PIN_DIR_TOGGLE     = 0x0003,
   // HIGH
    PIN_OUT_SET        = 0x0004,
    PIN_OUTSET         = 0x0004,
    PIN_OUT_HIGH       = 0x0004,
   // LOW
    PIN_OUT_CLR        = 0x0008,
    PIN_OUTCLR         = 0x0008,
    PIN_OUT_LOW        = 0x0008,
  // CHANGE/TOGGLE
    PIN_OUT_TGL        = 0x000C,
    PIN_OUTTGL         = 0x000C,
    PIN_OUT_TOGGLE     = 0x000C,
  //Interrupt disabled but input buffer enabled
    PIN_ISC_ENABLE     = 0x0080,
    PIN_INPUT_ENABLE   = 0x0080,
   // Interrupt on change
    PIN_ISC_CHANGE     = 0x0090,
    PIN_INT_CHANGE     = 0x0090,
  // Interrupt on rising edge
    PIN_ISC_RISE       = 0x00A0,
    PIN_INT_RISE       = 0x00A0,
  // Interrupt on falling edge
    PIN_ISC_FALL       = 0x00B0,
    PIN_INT_FALL       = 0x00B0,
  // Interrupt and input buffer disabled
    PIN_ISC_DISABLE    = 0x00C0,
    PIN_INPUT_DISABLE  = 0x00C0,
  // Interrupt enabled with sense on low level
    PIN_ISC_LEVEL      = 0x00D0,
    PIN_INT_LEVEL      = 0x00D0,
  // PULLUP ON
    PIN_PULLUP_ON      = 0x0100,
    PIN_PULLUP         = 0x0100,
    PIN_PULLUP_SET     = 0x0100,
  // PULLUP OFF
    PIN_PULLUP_OFF     = 0x0200,
    PIN_PULLUP_CLR     = 0x0200,
  // PULLUP TOGGLE
    PIN_PULLUP_TGL     = 0x0300,
    PIN_PULLUP_TOGGLE  = 0x0300,
    PIN_NOPULLUP       = 0x0200,
  // Pin Input Level Control
    PIN_INLVL_TTL      = 0x1000,
    PIN_INLVL_ON       = 0x1000,
    PIN_INLVL_SET      = 0x1000,
    PIN_INLVL_SCHMITT  = 0x2000,
    PIN_INLVL_OFF      = 0x2000,
    PIN_INLVL_CLR      = 0x2000, // alias
  // PIN INVERT ON
    PIN_INVERT_ON      = 0x4000,
    PIN_INVERT_SET     = 0x4000,
  // PIN INVERT OFF
    PIN_INVERT_OFF     = 0x8000,
    PIN_INVERT_CLR     = 0x8000,
  // PIN_INVERT_TOGGLE
    PIN_INVERT_TGL     = 0xC000,
    PIN_INVERT_TOGGLE  = 0xC000
  } pin_configure_t;

/**
 * @brief Helper functions to catch the last argument in the pincfg recursion loop
 *
 * @param mode Mode parameter
 * @return pin_configure_t
 */

  inline pin_configure_t _pincfg(const pin_configure_t mode) {
    return mode;
  }

  /**
   * @brief Helper functions to catch the nth in the pincfg recursion loop
   *
   * @param digital_pin Arduino pin
   * @param mode First "mode" parameter
   * @param modes Nth "mode" parameter
   * @return uint16_t pin configuration or'ed together
   */
  template <typename... MODES>
  uint16_t _pincfg(const pin_configure_t mode, const MODES&... modes) {
    return mode | _pincfg(modes...);
  }



  //void        pinConfigure(const uint8_t pinNumber, const uint16_t mode, const MODES&... modes);

  /**
   * @brief Variadic template function for configuring a pin
   *
   * @param digital_pin Arduino pin number
   * @param mode First "mode" parameter
   * @param modes Nth "mode" parameter
   */
  template <typename... MODES>
  void pinConfigure(const uint8_t digital_pin, const pin_configure_t mode, const MODES&... modes) {
    // Or-ing together the arguments using recursion
    uint16_t pin_config = _pincfg(mode, modes...);
    _pinconfigure(digital_pin, pin_config);
  }
#endif // end

/* If you understand the preceding code, studies show you are more likely than the average developer to
 * know C++, with a statistically significant relationship between how well you know the language and your likelihood of
 * understanding that code. I got distracted after the first + and have yet to comprehend the above implementation.
 * But it lets you separate arguments with commas instead of pipes, which is far more tasteful and graceful
 * and makes it look like an API instead of something knocked together by a C programmer.
 */



/* PERIPHERAL INDEX PICKING AND PORTMUX STUFF */
#include "pinswap.h"

#if defined(TWI_MORS_SINGLE)
  #define TWI_MORS
#elif defined(TWI_MANDS_SINGLE)
  #define TWI_MANDS
#elif defined(TWI_MORS_BOTH)
  #define TWI_MORS
  #define TWI_USING_WIRE1
#elif defined(TWI_MANDS_BOTH)
  #define TWI_MANDS
  #define TWI_USING_WIRE1
#else// Allow for the possibility that someoen directly defines these when using PlatformIO or similar.
  #if !(defined(TWI_MORS) || defined(TWI_MANDS))
    #warning "No wire mode specified - either a menu option must be selected (if using Arduino IDE) or the defines manually provided (if using PlatformIO/etc) - defaulting to TWI_MORS"
    #define TWI_MORS
  #endif
#endif

/* Serial Port Categorization
 * A little bit of trickery - this allows Serial to be defined as something other than USART0
 * Use case is for boards where the main serial port is not USART0 to be used without the user
 * having to find/replace Serial with Serial2 or whatever on their existing code if that's where
 * the monitor is. It requires that the board be defined by a variant file */
#ifndef Serial
  #define Serial Serial0 // Error here? Check for missing ; previous line in sketch.
#endif

/* Moved from pins_arduino.h to reduce code duplication - also made better decisions
 * These serial port names are intended to allow libraries and architecture-neutral
 * sketches to automatically default to the correct port name for a particular type
 * of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
 * the first hardware serial port whose RX/TX pins are not dedicated to another use.
 *
 * SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
 *
 * SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
 *
 * SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
 *
 * SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
 *
 * SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
 *                            pins are NOT connected to anything by default.
 *
 * First, we allow a define to be passed (lkely from boards.txt - though it could
 * come from pins_arduino, I suppose) to force a certain port here. The plan is
 * that I will pass defines from board definitions specifying this for the Curiosity
 * Nano boards, in order to improve the user experience there - though this may be
 * obliviated by the Serial/Serial0 thing just above, which has the advantage of
 * transparently adapting user code as well. */
#if !defined(SERIAL_PORT_MONITOR)
  #if defined(SERIAL_PORT_BOOT)
    #define SERIAL_PORT_MONITOR       SERIAL_PORT_BOOT
  #else
    #define SERIAL_PORT_MONITOR       Serial
  #endif
#endif
/* Following example in the Arduino Mega, where despite having 4 serial ports, they
 * use Serial for both SERIAL_PORT_MONITOR and SERIAL_PORT_HARDWARE, I will not
 * try to spread around the ports we use for these defines - if we're
 * going to declare some other port to be the "main" serial port, with the monitor
 * on it and all, we should be consistent about that, right? *shrug* */
#if !defined(SERIAL_PORT_HARDWARE)
  #define SERIAL_PORT_HARDWARE      SERIAL_PORT_MONITOR
#endif
/* If we have USART2 (ie, we are not a DD-series) we will declare that to be
 * SERIAL_PORT_HARDWARE_OPEN, so that on a DB-series, libraries are less likely to
 * squabble over the more powerful USART1 - USART1 being more powerful because it
 * is on PORTC, the MVIO port. */
#if !defined(SERIAL_PORT_MONITOR)
  #if defined(USART2) && (SERIAL_PORT_MONITOR != Serial2) && (!defined(SERIAL_PORT_BOOT) || SERIAL_PORT_BOOT != Serial2)
    #define SERIAL_PORT_HARDWARE_OPEN Serial2
  #else
    #if (SERIAL_PORT_MONITOR != Serial0 ) && (!defined(SERIAL_PORT_BOOT) || SERIAL_PORT_BOOT != Serial0)
      #define SERIAL_PORT_HARDWARE_OPEN Serial0
    #else
      #define SERIAL_PORT_HARDWARE_OPEN Serial1
    #endif
  #endif
#endif
/* DD-series parts with 20-or-fewer pins will not have a PC0 for the TX line of
 * Serial1. that makes it difficult to , so it can't be their MVIO serial port (without involving the event
 * system, of course) - but they can get a serial port on MVIO pins with USART0
 * and an alternate mapping. So for those parts only, Serial is their MVIO port.
 * For everyone else it's Serial1, and for non-DD parts, that is the only serial port connected to thr mvio
 * serial port.
 * that could be used with MVIO (again, short of rerouting signals with
 * the event system)
 * Note that if MVIO is disabled, we cannot detect that. */
#if !defined(SERIAL_PORT_MVIO) && defined(MVIO) // defined on DD snd DB.
  #if defined(DD_14_PINS) || defined(DD_20_PINS)
    #define SERIAL_PORT_MVIO Serial0
    #define SERIAL_PORT_MVIO_MUX (0x04) // TX PC1 RX PC2
  #else
    #define SERIAL_PORT_MVIO Serial1
  #endif
#endif


/* Spence Konde: This is a bit silly - but it does have some utility. I am of the
 * opinion that anything that needs to use a serial port or other peripheral of
 * which a chip may have several, and it needs to be sure to pick the "right" one
 * it should ASK THE USER - what good does it do that the GPS picked the first
 * open serial port, if the user tied it to a different port? Or thought they
 * were going to use software serial "like they always did" (*shudder*) */


#endif
