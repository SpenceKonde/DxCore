/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#ifdef __cplusplus
extern "C"{
#endif

#if (defined(__AVR_AVR128DA64__) || defined(__AVR_AVR128DA48__) || defined(__AVR_AVR128DA32__) || defined(__AVR_AVR128DA28__))
  // Their errata sheet indicates that both are in circulation for the 128k size. Big difference it makes, since they didn't fix any of the errata - or maybe they fixed things they didn't want to mention in the errata, or things to do with yield/etc.
  #define HAS_ADC_BUG (SYSCFG.REVID == 0x16 || SYSCFG.REVID == 0x17)
#elif (defined __AVR_DA__) //only A3 of these has made the rounds
  #define HAS_ADC_BUG (SYSCFG.REVID == 0x13)
#else
  #define HAS_ADC_BUG (0)
#endif

/* Analog reference options */

/* Change in mega4809: two places to define analog reference
 - VREF peripheral defines internal reference
 - analog peripherals define internal/Vdd/external
*/

 // internal from VREF

 /* Values shifted to avoid clashing with ADC REFSEL defines
  Will shift back in analog_reference function
  */

#define INTERNAL1V024     VREF_REFSEL_1V024_gc
#define INTERNAL2V048     VREF_REFSEL_2V048_gc
#define INTERNAL4V096     VREF_REFSEL_4V096_gc
#define INTERNAL2V5       VREF_REFSEL_2V500_gc
#define DEFAULT           VREF_REFSEL_VDD_gc
#define VDD               VREF_REFSEL_VDD_gc
#define EXTERNAL          VREF_REFSEL_VREFA_gc

// DACREFn MUXPOS currently missing from the headers!!
#define ADC_DAC0          (0x80 | ADC_MUXPOS_DAC0_gc)
#define ADC_DACREF0       (0x80 | 0x49)
#define ADC_DACREF1       (0x80 | 0x4A)
#define ADC_DACREF2       (0x80 | 0x4B)
#define ADC_TEMPERATURE   (0x80 | ADC_MUXPOS_TEMPSENSE_gc)
#ifdef MVIO
#define ADC_VDDDIV10      (0x80 | ADC_MUXPOS_VDDDIV10_gc)
#define ADC_VDDIO2DIV10   (0x80 | ADC_MUXPOS_VDDIO2DIV10_gc)
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()

void analogReadResolution(uint8_t res);


uint8_t digitalPinToTimerNow(uint8_t p);


void init_millis();
void stop_millis();
void restart_millis();
void set_millis(uint32_t newmillis);

// These are in here so that - should it be necessary - library functions or user code could override these.
void init_ADC0() __attribute__((weak));       // this is called to initialize ADC0 - it also i
//   init_DAC0()                              // no init_DAC0() - all that the core does is call DACReference().
void init_timers() __attribute__((weak));     // this function is expected to configure all timers for PWM. init_millis() is called after this.
void init_clock() __attribute__((weak));      // this is called first, to initiate the system clock.
void init_TCA0() __attribute__((weak));       // called by init_timers()
void init_TCA1() __attribute__((weak));       // called by init_timers()
void init_TCBs() __attribute__((weak));       // called by init_timers()
void init_TCD0() __attribute__((weak));       // called by init_timers()



// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
  #define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);
uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define PG 6
#define NUM_TOTAL_PORTS 7

/* THIS IS NOT AN API FUNCTION
 * WE MAKE NO PROMISES OF STABILITY FOR
 * digitalPortToPin0() - it is NOT even a
 * vaguely plausible function for anything other
 * than AVR Dx with 28+ pins
 */

extern const uint8_t digital_port_to_pin0[];

// These are used for two things: identifying the timer on a pin
// and for the MILLIS_TIMER define that the uses can test which timer
// actually being used for millis is actually being used
// Reasoning these constants are what they are:
// Low nibble is the number of that peripheral
// high nibble specify the type of timer
// TCA=0x08, TCB=0x10, TCD=0x10 (leaving room in case Microchip ever decides to release a TCC)
// Things that aren't hardware timers with output compare are after that
// DAC output isn't a timer, but has to be treated as such by PINMODE
// RTC timer is a tiner, but certainly not that kind of timer
#define NOT_ON_TIMER  0x00
#define TIMERA0       0x10
#define TIMERA1       0x08 // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERB0       0x20
#define TIMERB1       0x21
#define TIMERB2       0x22
#define TIMERB3       0x23
#define TIMERB4       0x24
#define TIMERD0       0x40
#define TIMERRTC      0x90
#define DACOUT        0x80

#define digitalPinToPort(pin)               ( (pin  < NUM_TOTAL_PINS)  ? digital_pin_to_port[pin] : NOT_A_PIN )
#define digitalPinToBitPosition(pin)        ( (pin  < NUM_TOTAL_PINS)  ? digital_pin_to_bit_position[pin] : NOT_A_PIN )
#define digitalPinToBitMask(pin)            ( (pin  < NUM_TOTAL_PINS)  ? digital_pin_to_bit_mask[pin] : NOT_A_PIN )
#define digitalPinToTimer(pin)              ( (pin  < NUM_TOTAL_PINS)  ? digital_pin_to_timer[pin] : NOT_ON_TIMER )
#define digitalPortToPin0(port)             ( (port < NUM_TOTAL_PORTS) ? digital_port_to_pin0[port] : NOT_A_PIN)
#define portToPortStruct(port)              ( (port < NUM_TOTAL_PORTS) ? ((PORT_t *)  & PORTA + port) : NULL)
#define digitalPinToPortStruct(pin)         ( (pin  < NUM_TOTAL_PINS)  ? ((PORT_t *)  & PORTA + digitalPinToPort(pin)) : NULL)
#define analogPinToBitPosition(pin)         ( (digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_position[pin] : NOT_A_PIN )
#define analogPinToBitMask(pin)             ( (digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_mask[pin] : NOT_A_PIN )
#define getPINnCTRLregister(port, bit_pos)  ( ((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL )
#define digitalPinToInterrupt(P) (P)

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P)  ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P)   ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )

#if (__AVR_ARCH__ == 104)
  #define MAPPED_PROGMEM __attribute__ (( __section__(".FLMAP_SECTION3")))
#elif (__AVR_ARCH__ == 102)
  #define MAPPED_PROGMEM __attribute__ (( __section__(".FLMAP_SECTION1")))
#else
  // __AVR_ARCH__ == 103, so all of the flash is memory mapped, and the linker
  // will automatically leave const variables in flash.
  #define MAPPED_PROGMEM
#endif
/*
 * Compatibility - General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
 * They now appear to have decided they don't like either of these conventions, and are grouping them under a "General Purpose Register"
 * "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any I/O occurring here (ofc they were referring
 * to the IN and OUT instructions, which can be used on these), but I certainly wouldn't have changed a convention like this. And if I
 * really had to... I would be too ashamed to do it again in just a couple of years because I realized I didn't like the first change
 * much either. Then again, maybe this just brings to mind that old line about talking cookware...
 */

// Pre-Dx-series parts call them GPIORn instead of GPR.GPRn/GPR_GPRn .
#ifndef GPIOR0
  #define GPIOR0 (GPR_GPR0)
  #define GPIOR1 (GPR_GPR1)
  #define GPIOR2 (GPR_GPR2)
  #define GPIOR3 (GPR_GPR3)
#endif
// For a while, these were called GPIO in the i/o headers...
#ifndef GPIO0
  #define GPIO0 (GPR_GPR0)
  #define GPIO1 (GPR_GPR1)
  #define GPIO2 (GPR_GPR2)
  #define GPIO3 (GPR_GPR3)
#endif

#define AVR128DA    0x40
#define AVR128DB    0x48
#define AVR64DA     0x20
#define AVR64DB     0x28
#define AVR32DA     0x10
#define AVR32DB     0x18
#define AVR64DD     0xC0
#define AVR32DD     0xA0
#define AVR16DD     0x90
#define HAS_14_PINS 0x01
#define HAS_20_PINS 0x02
#define HAS_28_PINS 0x03
#define HAS_32_PINS 0x04
#define HAS_48_PINS 0x05
#define HAS_64_PINS 0x06
#define IS_AVR_DB   0x08
#define IS_AVR_DD   0x80

//#defines to identify part families
#if defined(__AVR_AVR128DA64__) || defined(__AVR_AVR64DA64__)
#define DA_64_PINS
#define Dx_64_PINS
#define DXCORE_ID_LOW HAS_64_PINS
#define __AVR_DA__
#elif defined(__AVR_AVR128DA48__) || defined(__AVR_AVR64DA48__) || defined(__AVR_AVR32DA48__)
#define DA_48_PINS
#define Dx_48_PINS
#define DXCORE_ID_LOW HAS_48_PINS
#define __AVR_DA__
#elif defined(__AVR_AVR128DA32__) || defined(__AVR_AVR64DA32__) || defined(__AVR_AVR32DA32__)
#define DA_32_PINS
#define Dx_32_PINS
#define DXCORE_ID_LOW HAS_32_PINS
#define __AVR_DA__
#elif defined(__AVR_AVR128DA28__) || defined(__AVR_AVR64DA28__) || defined(__AVR_AVR32DA28__)
#define DA_28_PINS
#define Dx_28_PINS
#define DXCORE_ID_LOW HAS_28_PINS
#define __AVR_DA__
#elif defined(__AVR_AVR128DB64__) || defined(__AVR_AVR64DB64__)
#define DB_64_PINS
#define Dx_64_PINS
#define DXCORE_ID_LOW HAS_64_PINS | IS_AVR_DB
#define __AVR_DB__
#elif defined(__AVR_AVR128DB48__) || defined(__AVR_AVR64DB48__) || defined(__AVR_AVR32DB48__)
#define DB_48_PINS
#define Dx_48_PINS
#define DXCORE_ID_LOW HAS_48_PINS | IS_AVR_DB
#define __AVR_DB__
#elif defined(__AVR_AVR128DB32__) || defined(__AVR_AVR64DB32__) || defined(__AVR_AVR32DB32__)
#define DB_32_PINS
#define Dx_32_PINS
#define DXCORE_ID_LOW HAS_32_PINS | IS_AVR_DB
#define __AVR_DB__
#elif defined(__AVR_AVR128DB28__) || defined(__AVR_AVR64DB28__) || defined(__AVR_AVR32DB28__)
#define DB_28_PINS
#define Dx_28_PINS
#define DXCORE_ID_LOW HAS_28_PINS | IS_AVR_DB
#define __AVR_DB__
#elif defined(__AVR_AVR64DD32__) || defined(__AVR_AVR32DD32__) || defined(__AVR_AVR16DD32__)
#define DD_32_PINS
#define Dx_32_PINS
#define DXCORE_ID_LOW HAS_32_PINS | IS_AVR_DD
#define __AVR_DD__
#elif defined(__AVR_AVR64DD28__) || defined(__AVR_AVR32DD28__) || defined(__AVR_AVR16DD28__)
#define DD_28_PINS
#define Dx_28_PINS
#define DXCORE_ID_LOW HAS_28_PINS | IS_AVR_DD
#define __AVR_DD__
#elif defined(__AVR_AVR64DD20__) || defined(__AVR_AVR32DD20__) || defined(__AVR_AVR16DD20__)
#define DD_20_PINS
#define Dx_20_PINS
#define DXCORE_ID_LOW HAS_20_PINS | IS_AVR_DD
#define __AVR_DD__
#elif defined(__AVR_AVR64DD14__) || defined(__AVR_AVR32DD14__) || defined(__AVR_AVR16DD14__)
#define DD_14_PINS
#define Dx_14_PINS
#define DXCORE_ID_LOW HAS_14_PINS | IS_AVR_DD
#define __AVR_DD__
#else
#error "Can't-happen: unknown chip somehow being used"
#endif
#ifdef __AVR_DD__
#error "The AVR DD series is not supported yet because the datasheet is not available. It should not be possible to see this message, as when boards.txt entries are added, this message would be removed"
#endif


/*
#define DXCORE "1.2.0-dev"
#define DXCORE_MAJOR 1UL
#define DXCORE_MINOR 2UL
#define DXCORE_PATCH 0UL
#define DXCORE_RELEASED 0
*/
#define DXCORE_NUM ((DXCORE_MAJOR<<24)+(DXCORE_MINOR<<16)+(DXCORE_PATCH<<8)+DXCORE_RELEASED)



#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "UART.h"

#endif

#include "pins_arduino.h"
#endif
