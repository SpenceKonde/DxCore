/* Arduino.h - Main include file for the Arduino SDK
 * Copyright (c) 2005-2013 Arduino Team.  All right reserved.
 * And presumably from then until 2018 when this was forked
 * for megaTinyCore. Copyright 2018-2021 Spence Konde
 * Part of DxCore, which adds Arduino support for the AVR DA,
 * DB, and DD-series microcontrollers from Microchip.
 * DxCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */
/*************************************************************
 * This file contains the stuff I think people are most likely
 * to need to refer to. The minutia has all been pushed into
 * core_devices.h if it's independent of pins_arduino.h or into
 * pinswap.h if it relates to PORTMUX, which is a great volume
 * of stuff nobody should have to read.
 *
 * That means functions and macros that may be used by user code
 * (except for part-feature ones - those are clearly documented
 * in the readme if they are ready for users).
 * I also try to put detailed comments in where appropriate.
 *************************************************************/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"
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

#ifdef __cplusplus
extern "C"{
#endif

/* we call badArg() when we know at compile time that one or more arguments passed to
 * the function is nonsensical or doomed to generate useless output. For example,
 * analogWrite() on a constant pin that will never support PWM, or digitalWrite() on
 * a pin number that is neither a pin nor NOT_A_PIN (which silently does nothing for
 * compatibility).
 * badCall() on the other hand is called if we know that regardless of what arguments
 * are passed, that function is nonsensical with current settings, for example, millis()
 * when millis timekeeping has been disabled */
void badArg(const char*) __attribute__((error("")));
void badCall(const char*) __attribute__((error("")));

// The fast digital I/O functions only work when the pin is known at compile time.
inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
{
  if(!__builtin_constant_p(pin))
    badArg("Fast digital pin must be a constant");
}


/* Analog reference options - Configuring these is very simple, unlike tinyAVR 0/1
   and megaAVR 0, and like tinyAVR 2-series. */
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


#define ADC_ERROR_BAD_PIN_OR_CHANNEL                -32765
#define ADC_ERROR_DISABLED                          -32767
#define ADC_ERROR_BUSY                              -32766
#define ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL       -2100000000
// positive channel is not (0x80 | valid_channel) nor a digital pin number
// referring to a pin with analog input.
#define ADC_ENH_ERROR_BUSY                     -2100000001
// The ADC is currently performing another conversion in the background (either
// in free-running mode or a long-running burst conversion; taking a burst
// accumulated reading and then calling a specified function when the result
// was finally ready may be supported in a future version.
#define ADC_ENH_ERROR_RES_TOO_LOW              -2100000003
// analogReadEnh() must not be called with a resolution lower than 8-bits.
// you can right-shift as well as the library can.
#define ADC_ENH_ERROR_RES_TOO_HIGH             -2100000004
// Only resonlutions that can be generated through accumulator oversample
// + decimation are supported, maximum is 13, 15, or 17 bits. This will
// also be returned if a larger raw accumulated result is requested.
#define ADC_DIFF_ERROR_BAD_NEG_PIN             -2100000005
// Analog pin given as negative pin is not a valid negative mux pin
#define ADC_ENH_ERROR_NOT_DIFF_ADC             -2100000006
// analogReadDiff() called from a part without a differential ADC;
// Never actually returned, because we give compile error here
#define ADC_ENH_ERROR_DISABLED                 -2100000007
// The ADC is not currently enabled. This error is disabled currently - if analogReadEnh encounters a disabled ADC, it will enable it, take the reading, and disable it again.
#define ADC_ERROR_INVALID_CLOCK                     -32764
// Returned by analogClockSpeed if the value in the register is currently unknown, or if an invalid frequency is requested.




/* inlining of a call to delayMicroseconds() would throw it off */
void _delayMicroseconds(unsigned int us) __attribute__((noinline));


/*************************************************
 * Extended API - Enhanced DxCore features
 * These are functions users might call from the
 * sketch, or in some cases, override.
 * 1. Timer + millis() control.
 * 2. initialization routines to override
 * 3. Advanced Analog functionality
 * 4. Advanced Digital functionality
 ************************************************/


// stop, restart and set millis intended for switching to RTC for millis timekeeping while sleeping.
void stop_millis();                       // stop the timer being used for millis, and disable the interrupt.
void restart_millis();                    // After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.
void set_millis(uint32_t newmillis);      // Sets the millisecond timer to the specified number of milliseconds. DO NOT CALL with a number lower than the current millis count if you have any timeouts ongoing.
                                          // they may expire instantly.
void nudge_millis(uint16_t nudgemillis);  // Sets the millisecond timer forward by the specified number of milliseconds. Currently only implemented for TCB, TCA implementation will be added. This allows a clean
                                          // way to advance the timer without needing to read the current millis yourself, and without a few other risks. (added becauise *I* needed it, but simple enough).
                                          // The intended use case is when you know you're disabling millis for a long time, and know exactly how long that is (ex, to update neopixels), and want to nudge the timer
                                          // forward by a given amount; I added this when in a pinch because *I* needed that functionality.

// Allows for user to mark a timer "do not touch" for purposes of analogWrite and the like, so you can take over a timer and reconfigure it, and not worry about digitalWrite() flipping a CMPEN bit.
// On megaTinyCore this also prevents the situation where PWM is remapped, but then when the user is digitalWrite'ing pins that default to having PWM, it would turn off the PWM now coming from another pin
// This is not an issue because we fully support portmux (can't spare the flash overhead on the tinies, people already complain that the core uses too much flash)
void takeOverTCA0();                      // Can be used to tell core not to use TCA0 for any API calls - user has taken it over.
void takeOverTCA1();                      // Can be used to tell core not to use TCA1 for any API calls - user has taken it over.
void takeOverTCD0();                      // Can be used to tell core not to use TCD0 for any API calls - user has taken it over.
void resumeTCA0();                        // Restores core-mediated functionality that uses TCA0 and restores default TCA0 configuration.
void resumeTCA1();                        // Restores core-mediated functionality that uses TCA1 and restores default TCA1 configuration.
//bool digitalPinHasPWM(uint8_t p);       // Macro. Returns true if the pin can currently output PWM using analogWrite(), regardless of which timer is used and considering current PORTMUX setting
uint8_t digitalPinToTimerNow(uint8_t p);  // Returns the timer that is associated with the pin now (considering PORTMUX)

// These are in here so that - should it be necessary - library functions or user code could override these.
void init_clock()  __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()   __attribute__((weak)); // this is called to initialize ADC0
//   init_DAC0()                          // no _init_DAC0() - all that the core does is call DACReference().
void init_TCA0()   __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA0 will not function.
void init_TCA1()   __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA1 will not function, nor will the TCBs unless the clock source is changed.
void init_TCBs()   __attribute__((weak)); // called by init_timers()
void init_TCD0()   __attribute__((weak)); // called by init_timers()
void init_millis() __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.

void onClockFailure() __attribute__((weak)); // called by the clock failure detection ISR. Default action is a blink code with 4 blinks.
void onClockTimeout() __attribute__((weak)); // called if we try to switch to external cloc, but it doesn't work. Default action is a blink code with 3 blinks.

#ifndef CORE_ATTACH_OLD
  void attachPortAEnable();
  void attachPortBEnable();
  void attachPortCEnable();
  void attachPortDEnable();
  void attachPortEEnable();
  void attachPortFEnable();
  void attachPortGEnable();
#endif

// ANALOG EXTENDED FUNCTIONS
// Covered in documentation.

int32_t           analogReadEnh(uint8_t pin,                uint8_t res,  uint8_t gain);
int32_t          analogReadDiff(uint8_t pos,  uint8_t neg,  uint8_t res,  uint8_t gain);
int16_t        analogClockSpeed(int16_t frequency, uint8_t options);
bool       analogReadResolution(uint8_t res);
bool       analogSampleDuration(uint8_t dur);
void               DACReference(uint8_t mode);

uint8_t      getAnalogReference();
uint8_t         getDACReference();
uint8_t getAnalogSampleDuration();
int8_t  getAnalogReadResolution();

//
// DIGITAL I/O EXTENDED FUNCTIONS
// Covered in documentation.
void           openDrain(uint8_t pinNumber,   uint8_t val);
int8_t   digitalReadFast(uint8_t pinNumber               );
void    digitalWriteFast(uint8_t pinNumber,   uint8_t val);
void       openDrainFast(uint8_t pinNumber,   uint8_t val);
void        pinConfigure(uint8_t pinNumber, uint16_t mode);
void          turnOffPWM(uint8_t pinNumber               );

// Not a function, still important
#define digitalPinHasPWMNow(p)            (digitalPinToTimerNow(p) != NOT_ON_TIMER)

// avr-libc defines _NOP() since 1.6.2
// Spence: Better tell avr-gcc that, it seems to disagree
#ifndef _NOP
  #define _NOP()    do { __asm__ volatile ("nop"); } while (0)
#endif
#ifndef _NOP2
  #define _NOP2()   do { __asm__ volatile ("nop"); } while (0)
#endif
#ifndef _NOPNOP
  #define _NOPNOP() do { __asm__ volatile ("rjmp .+0"); } while (0)
#endif
#ifndef _NOP8
  #define _NOP8()   do { __asm__ volatile ("rjmp .+2"  "\n\t" \
                                           "ret"       "\n\t" \
                                           "rcall .-4" "\n\t"); } while (0)
#endif
#ifndef _NOP14
  #define _NOP14()  do { __asm__ volatile ("rjmp .+2"  "\n\t" \
                                           "ret"       "\n\t" \
                                           "rcall .-4" "\n\t" \
                                           "rcall .-6" "\n\t" ); } while (0)
#endif
uint16_t clockCyclesPerMicrosecond();
uint32_t clockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToClockCycles(uint32_t microseconds);

// Currently DxCore has no cases where the millis timer isn';t derived from system clock, but that will change
/* This becomes important when we support other timers for timekeeping. The Type D timer can be faster, requiring:
 * These talk about the timebase from which millis is derived, not the actual timer counting frequency.
 * That is, it's used to calculqte the values that are we multipliy by the prescaler to get the timekeeping stuff.
 * These can be different from the above only when the millis timer isn't running from CLK_PER.
     For example, if we run it from a TCD clocked from internal HF but we are running on a crystal.
     That's a strange way to use the TCD, but
 * megaTinyCore has these, and we will have wsomething analogou.

uint16_t millisClockCyclesPerMicrosecond();
uint32_t millisClockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToMillisClockCycles(uint32_t microseconds);

 * the time and sleep library will require some things like this.
 */


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
 * DxCore, but it can on megaTinyCore, and might one day be possible on DxCore
 * though I'm not sure I want to implement it that way).
 *****************************************************************************/

#define NOT_ON_TIMER  0x00
#define TIMERA0       0x10
#define TIMERA1       0x08 // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERB0       0x20
#define TIMERB1       0x21
#define TIMERB2       0x22
#define TIMERB3       0x23
#define TIMERB4       0x24
#define TIMERD0       0x40 /* in PWM context will show up as (TIMERD0 | 0-7), ex 0x40, 0x42 etc - that part is the mux value (not the channel). nothing that is not TCD0 will have bit 6 set. */
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

#define NOT_A_PIN         (255)
// Generally, you should check for getting this when you ask for a pin  ;-)
#define NOT_A_PORT        (255)
// Same numeric value, but used for improved code readability
#define NOT_AN_INTERRUPT  (255)
#define NOT_A_MUX         (255)
// invalid portmux options
// When cast to int8_t these are -1, but it is critical to define them as 255, not -1 because we check if they're less than the number of something

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define PG 6
#define NUM_TOTAL_PORTS 7

// These are used as the second argument to pinConfigure(pin,configuration)
// You can bitwise OR as many of these as you want, or just do one. Very
// flexible function; not the world's fastest though. Directives are handled
// in the order they show up on this list.
#define PIN_DIRSET           0x0001
#define PIN_DIRCLR           0x0002
#define PIN_DIRTGL           0x0003
#define PIN_DIR_OUTPUT       0x0001 // Alias
#define PIN_DIR_OUT          0x0001 // Alias
#define PIN_DIR_INPUT        0x0002 // Alias
#define PIN_DIR_IN           0x0002 // Alias
#define PIN_DIR_TOGGLE       0x0003 // Alias
#define PIN_DIR_TGL          0x0003 // Alias
#define PIN_OUTSET           0x0004
#define PIN_OUTCLR           0x0008
#define PIN_OUTTGL           0x000C
#define PIN_OUT_HIGH         0x0004 // Alias
#define PIN_OUT_LOW          0x0008 // Alias
#define PIN_OUT_TOGGLE       0x000C // Alias
#define PIN_OUT_TGL          0x000C // Alias
#define PIN_INPUT_ENABLE     0x0080
#define PIN_INT_CHANGE       0x0090
#define PIN_INT_RISE         0x00A0
#define PIN_INT_FALL         0x00B0
#define PIN_INPUT_DISABLE    0x00C0
#define PIN_INT_LEVEL        0x00D0
#define PIN_PULLUP_ON        0x0100
#define PIN_PULLUP_OFF       0x0200
#define PIN_PULLUP_TOGGLE    0x0300 // I suppose I can see uses for this
#define PIN_PULLUP           0x0100 // Alias
#define PIN_NOPULLUP         0x0200 // Alias
#define PIN_PULLUP_TGL       0x0300 // I suppose I can see uses for this
#define PIN_INVERT_ON        0x4000
#define PIN_INVERT_OFF       0x8000
#define PIN_INVERT_TGL       0xC000 // One of the less useful ones...
#define PIN_INVERT_TOGGLE    0xC000 // One of the less useful ones...
#define PIN_INLVL_TTL        0x1000 // MVIO parts only
#define PIN_INLVL_SCHMITT    0x2000 // MVIO parts only
#define PIN_INLVL_ON         0x1000 // MVIO parts only
#define PIN_INLVL_OFF        0x2000 // MVIO parts only

#define digitalPinToPort(pin)               ((pin     < NUM_TOTAL_PINS ) ? digital_pin_to_port[pin]         : NOT_A_PIN)
#define digitalPinToBitPosition(pin)        ((pin     < NUM_TOTAL_PINS ) ? digital_pin_to_bit_position[pin] : NOT_A_PIN)
#define digitalPinToBitMask(pin)            ((pin     < NUM_TOTAL_PINS ) ? digital_pin_to_bit_mask[pin]     : NOT_A_PIN)
#define digitalPinToTimer(pin)              ((pin     < NUM_TOTAL_PINS ) ? digital_pin_to_timer[pin]        : NOT_ON_TIMER)
#define portToPortStruct(port)              (((port)  < NUM_TOTAL_PORTS) ? (((PORT_t *)  &PORTA) +                (port)) : NULL)
#define digitalPinToPortStruct(pin)         ((pin     < NUM_TOTAL_PINS ) ? (((PORT_t *)  &PORTA) + digitalPinToPort(pin)) : NULL)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_position[pin] : NOT_A_PIN)
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ? digital_pin_to_bit_mask[pin]     : NOT_A_PIN)
#define getPINnCTRLregister(port, bit_pos)  (((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL)
#define digitalPinToInterrupt(P)            (P)

/*
#define portToPinZero(port)           (in variant - needed to get from port number for TCA mux to the pins to turnOffPWM() on them).
 The main application involves taking advantage of the fact that we always number pins in order within each port and never shuffle ports around; it wound up being necessary when I was working with the automatic PWM pin remapping.
*/

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P)  ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P)   ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  #include "UART.h"
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int16_t analogClockSpeed(int16_t frequency = 0,  uint8_t options = 0);
#endif

#include "pins_arduino.h"
#include "pinswap.h"




// A little bit of trickery - this allows Serial to be defined as something other than USART0
// Use case is for boards where the main serial port is not USART0 to be used without the user
// having to find/replace Serial with Serial2 or whatever on their existing code if that's where
// the monitor is. It requires that the board be defined by a variant file
#ifndef Serial
  #define Serial Serial0 //Error here? Check for missing ; previous line in sketch.
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

#if !defined(SERIAL_PORT_MVIO) && defined(MVIO)
// DD-series parts with 20-or-fewer pins will not have a PC0 for the TX line of
// Serial1, so it can't be their MVIO serial port (without involving the event
// system, of course) - but they can get a serial port on MVIO pins with USART0
// and an alternate mapping. So for those parts only, Serial is their MVIO port.
// For everyone else it's Serial1, and for non-DD parts, that is the only port
// that could be used with MVIO (again, short of rerouting signals with
// the event system)
// Note that if MVIO is disabled, we cannot detect that.
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
