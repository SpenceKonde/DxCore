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

#include "core_parts.h"
#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C"{
#endif

// Constant checks error handler
void badArg(const char*) __attribute__((error("")));
void badCall(const char*) __attribute__((error("")));

inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
{
  if(!__builtin_constant_p(pin))
    badArg("Fast digital pin must be a constant");
}

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

// Defines something as an ADC channel, as opposed to a digital pin number
// This is the same convention that ATTinyCore uses, with high bit indicating
// that a value is a channel number not a pin number.
#define ADC_CH(ch)         (0x80 | (ch))


// DACREFn MUXPOS currently missing from the headers!!
#define ADC_DAC0           ADC_CH(ADC_MUXPOS_DAC0_gc)
#define ADC_GROUND         ADC_CH(ADC_MUXPOS_GND_gc)
#define ADC_DACREF0        ADC_CH(0x49)
#ifdef AC1 // Always either 1 AC or 3, never 2.
  #define ADC_DACREF1      ADC_CH(0x4A)
  #define ADC_DACREF2      ADC_CH(0x4B)
#endif
#define ADC_TEMPERATURE    ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
#ifdef MVIO
  #define ADC_VDDDIV10     ADC_CH(ADC_MUXPOS_VDDDIV10_gc)
  #define ADC_VDDIO2DIV10  ADC_CH(ADC_MUXPOS_VDDIO2DIV10_gc)
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()

/* inlining of a call to delayMicroseconds() would throw it off */
__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us);

bool digitalPinHasPWMNow(uint8_t p);
uint8_t digitalPinToTimerNow(uint8_t p);


void init_millis();                       // called by init_timers() to set up millis for the first time.
void stop_millis();                       // stop the timer being used for millis, and disable the interrupt.
void restart_millis();                    // After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.
void set_millis(uint32_t newmillis);      // Sets the millisecond timer to the specified number of milliseconds.
void takeOverTCA0();                      // Can be used to tell core not to use TCA0 for any API calls - user has taken it over.
void takeOverTCA1();                      // Can be used to tell core not to use TCA1 for any API calls - user has taken it over.
void takeOverTCD0();                      // Can be used to tell core not to use TCD0 for any API calls - user has taken it over.
void resumeTCA0();                        // Restores core-mediated functionality that uses TCA0 after reconfiguring it.
void resumeTCA1();                        // Restores core-mediated functionality that uses TCA0 after reconfiguring it.

// These are in here so that - should it be necessary - library functions or user code could override these.
void init_ADC0()   __attribute__((weak)); // this is called to initialize ADC0 - it also i
//   init_DAC0()                          // no init_DAC0() - all that the core does is call DACReference().
void init_timers() __attribute__((weak)); // this function is expected to configure all timers for PWM. init_millis() is called after this.
void init_clock()  __attribute__((weak)); // this is called first, to initiate the system clock.
void init_TCA0()   __attribute__((weak)); // called by init_timers()
void init_TCA1()   __attribute__((weak)); // called by init_timers()
void init_TCBs()   __attribute__((weak)); // called by init_timers()
void init_TCD0()   __attribute__((weak)); // called by init_timers()

int32_t analogReadEnh( uint8_t pin,              uint8_t res, uint8_t gain);
int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain);

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
  #define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);
uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

/* Timers and Timer-like-things
 * These are used for two things: Identifying the timer on a pin in
 * digitalPinToTimer(), and for the MILLIS_TIMER define that users can test to
 * verify which timer is being used for millis.
 *
 * Prior to 1.3.x TCAs were all 0x1_, TCBs 0x2_. But in order to make the
 * take-over tracking work efficiently I needed a dedicated bit for each TCA.
 * so that we can just do (PeripheralControl | TIMERA0) to test if user has
 * taken over the timer. Hence, all the "big" timers (those which have a
 * takeOverTCxn() function and which PORTMUX moves en masse instead of one at
 * a time) have their own bit within these macros.
 * Note also that the digital_pin_to_timer table doesn't list these anymore.\
 * There are two functions within the core that need to know this:
 * AnalogWrite, and turnOffPWM. These each carry their own implementation of
 * logic to identify the timer and channel; the only other cases in which these
 * pins need ro be identified are within user code, where the pin mapping can
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
 * DxCore, but soon).
 *****************************************************************************/

#define NOT_ON_TIMER  0x00
#define TIMERA0       0x10
#define TIMERA1       0x08 // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERB0       0x20
#define TIMERB1       0x21
#define TIMERB2       0x22
#define TIMERB3       0x23
#define TIMERB4       0x24
#define TIMERD0       0x40 /* in PWM context will show up as (TIMERD0 | 0-7), ex 0x40, 0x42 etc - that part is the mux value. nothng that s not TCD0 will have bit 6 set. */
#define TIMERRTC      0x90
#define DACOUT        0x80


// These are lookup tables to find pin parameters from Arduino pin numbers
// They are defined in the variant's pins_arduino.h

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];

/* Yes, I (Spence Konde) have realized that on 64k and 128k parts, these end
 * up in RAM since they aren't declared PROGMEM. That means that an amount of
 * work could save up to 224 bytes of RAM on 64-pin parts, 164 bytes on 48-pin
 * parts, 108b on 32-pin parts and 92b on 28-pin parts for the parts with 64k
 * and 128k of flash (32k flash parts are __AVR_ARCH__ == 103, so they have
 * all flash memory mapped and const variables are automatically stored only
 * in flash. However there are several design decisions, and small speed and
 * flash penalty here, and you need to apply it only to the 64/128k parts.
 * Have more important fish to fry atm than making a couple hundred bytes of
 * RAM for parts with 8 or 16k of RAM. */

/* PORT names and the NOT_A_* definitions - used EVERYWHERE! */

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

#define digitalPinToPort(pin)               ((pin  < NUM_TOTAL_PINS ) ? digital_pin_to_port[pin]         : NOT_A_PIN)
#define digitalPinToBitPosition(pin)        ((pin  < NUM_TOTAL_PINS ) ? digital_pin_to_bit_position[pin] : NOT_A_PIN)
#define digitalPinToBitMask(pin)            ((pin  < NUM_TOTAL_PINS ) ? digital_pin_to_bit_mask[pin]     : NOT_A_PIN)
#define digitalPinToTimer(pin)              ((pin  < NUM_TOTAL_PINS ) ? digital_pin_to_timer[pin]        : NOT_ON_TIMER)
#define portToPortStruct(port)              ((port < NUM_TOTAL_PORTS) ? ((PORT_t *)  & PORTA + port)                  : NULL)
#define digitalPinToPortStruct(pin)         ((pin  < NUM_TOTAL_PINS ) ? ((PORT_t *)  & PORTA + digitalPinToPort(pin)) : NULL)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_position[pin] : NOT_A_PIN)
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_mask[pin]     : NOT_A_PIN)
#define getPINnCTRLregister(port, bit_pos)  (((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL)
#define digitalPinToInterrupt(P) (P)

/*#define portToDigitalPinZero(port)           (in variant - needed to get from port number for TCA mux to the pins to turnOffPWM() on them).
// If hardware has PORTx,
if ((digitalPinToPort(portToDigitalPinZero(digitalPinToPort(some_pin)) + bit_position) \
                        == digitalPinToPort(some_pin)) is true if and only if there is a PIN n within the same port.   */

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P)  ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P)   ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )


#define CORE_HAS_FASTIO 1                /* DxCore has the digitalReadFast() and digitalWriteFast()              */
#define CORE_HAS_OPENDRAIN 1             /* DxCore has openDrain() and openDrainFast()                           */
#define CORE_HAS_PINCONFIG 0             /* pinConfig is not yet implemented anywhere, but will be soon.         */
#define CORE_HAS_TIMER_TAKEOVER 1        /* DxCore has takeOverTCAn() and takeOverTCD0()                         */
#define CORE_HAS_TIMER_RESUME 1          /* DxCore has resumeTCAn() and resumeTCD0()                             */
#define SUPPORT_LONG_TONES 1             /* tone()s specifying duration are timed by counting the oscillations.
 * Frequency is in Hz, while duration is in ms, so (2 * frequency * duration)/1000 is the number of transitions
 * before it should write the pin low and turn off the timer. Obviously the 2 can be factored, but it will still
 * overflow when frequency * duration > 4.2b. A high-pitched tone of 20 kHz would overflow if a delay of longer
 * than around 7 minutes was requested (prior to this update, the maximum was a factor of two lower than that)
 * On parts like the Dx-series where there's no problem with flash space, we now, when duration > (2^16) ms (a
 * necessary precondition for overflow to occur) do ((frequency / 5) * (duration/100)) at cost of ~100b flash .  */
#define ADC_DIFFERENTIAL 1               /* Basic modern-AVR differential ADC         */
#define CORE_HAS_ANALOG_ENH 0            /* DxCore does not have analogReadEnh()  - yet! Coming by 1.4.0!        */
#define CORE_HAS_ANALOG_DIFF 0           /* DxCore does not have analogReadDiff() - yet! Coming by 1.4.0!        */
// #define MAX_OVERSAMPLED_RESOLUTION 15 /* DxCore will have max res. of 15 bits via oversampling & decimation   */

#ifdef OPAMP0
  #ifndef ADC_MAXIMUM_GAIN
    #define ADC_MAXIMUM_GAIN -1            /* DB-series can use their OPAMPs as a PGA         */
  #endif
  #define PIN_OPAMP0_INP PIN_PD1
  #define PIN_OPAMP0_OUT PIN_PD2
  #define PIN_OPAMP0_INN PIN_PD3
  #ifdef OPAMP1
    #define PIN_OPAMP1_INP PIN_PD4
    #define PIN_OPAMP1_OUT PIN_PD5
    #define PIN_OPAMP1_INN PIN_PD7
  #endif
  #ifdef OPAMP2
    #define PIN_OPAMP2_INP PIN_PE1
    #define PIN_OPAMP2_OUT PIN_PE2
    #define PIN_OPAMP2_INN PIN_PE3
  #endif
#else
  #define ADC_MAXIMUM_GAIN 0                /* DA and DD series don't have any                */
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  #include "UART.h"
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
#endif

#include "pins_arduino.h"


// If not otherwise specified, we will assume the DAC outputs on PD6 - No product has
// been announced with it anywhere else, nor has any product been announced with more than 1.
#ifdef DAC0
  #ifndef PIN_DACOUT
    #define PIN_DACOUT PIN_PD6
  #endif
#endif

// A little bit of trickery - this allows Serial to be defined as something other than USART0
// Use case is for boards where the main serial port is not USART0 to be used without the user
// having to find/replace Serial with Serial2 or whatever on their existing code if that's where
// the monitor is.
#ifndef Serial
  #define Serial Serial0
#endif



/* Moved from pins_arduino.h to reduce code duplication - also made better decisions */
// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.

// First, we allow a define to be passed (lkely from boards.txt - though it could
// come from pins_arduino, I suppose) to force a certain port here. The plan is
// that I will pass defines from board definitions specifying this for the Curiosity
// Nano boards, in order to improve the user experience there - though this may be
// obliviated by the Serial/Serial0 thing just above, which has the advantage of
// transparently adapting user code as well.

#if !defined(SERIAL_PORT_MONITOR)
  #if defined(SERIAL_PORT_BOOT)
    #define SERIAL_PORT_MONITOR       SERIAL_PORT_BOOT
  #else
    #define SERIAL_PORT_MONITOR       Serial
  #endif
#endif

// Following example in the Arduino Mega, where despite having 4 serial ports, they
// use Serial for both SERIAL_PORT_MONITOR and SERIAL_PORT_HARDWARE, I will not
// try to spread around the ports we use for these defines - if we're
// going to declare some other port to be the "main" serial port, with the monitor
// on it and all, we should be consistent about that, right? *shrug*
#define SERIAL_PORT_HARDWARE      SERIAL_PORT_MONITOR

// If we have USART2 (ie, we are not a DD-series) we will declare that to be
// SERIAL_PORT_HARDWARE_OPEN, so that on a DB-series, libraries are less likely to
// squabble over the more powerful USART1 - USART1 being more powerful because it
// is on PORTC, the MVIO port.

#if !defined(SERIAL_PORT_MONITOR)
  #if defined(USART2) && (SERIAL_PORT_MONITOR != Serial2)
    #define SERIAL_PORT_HARDWARE_OPEN Serial2
  #else
    #if (SERIAL_PORT_MONITOR != Serial0)
      #define SERIAL_PORT_HARDWARE_OPEN Serial0
    #else
      #define SERIAL_PORT_HARDWARE_OPEN Serial1
    #endif
  #endif
#endif

#if !defined(SERIAL_PORT_MVIO) && defined(MVIO)
// DD-series parts with 20-or-fewer pins will not have a PC0 for the TX line of
// Serial1, so it can't be their MVIO serial port (without involving the event
// system, of course) - but they can get a serial port on MVIO pins with USART0
// and an alternate mapping. So for those parts only, Serial is their MVIO port.
// For everyone else it's Serial1, and for non-DD parts, that is the only port
// that could be used with MVIO (again, short of rerouting signals with
// the event system)
  #if defined(DD_14_PINS) || defined(DD_20_PINS)
    #define SERIAL_PORT_MVIO Serial0
  #else
    #define SERIAL_PORT_MVIO Serial1
  #endif
#endif

// Spence Konde: This is a bit silly - but it does have some utility. I am of the
// opinion that anything that needs to use a serial port or other peripheral of
// which a chip may have several, and it needs to be sure to pick the "right" one
// it should ASK THE USER - what good does it do that the GPS picked the first
// open serial port, if the user tied it to a different port? Or thought they
// were going to use software serial "like they always did" (*shudder*)

#endif
