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
 * pinswap.h if it relates to PORTMUX. Most importantly, core_devices
 * is where we dumped the 3000 lines of #ifdefs needed to make sure
 * the new toolchains worked. At least they did bump the major version!
 * All in all, core_devices is full of all the crap that you don't care
 * to read through.
 *
 * That includes the declarations for all functions that the core supplies
 * which the user has business calling. Plus several that they have
 * no business calling,
 *************************************************************/

#ifndef Arduino_h
#define Arduino_h

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
void badArg(const char*)    __attribute__((error("")));
void badCall(const char*)   __attribute__((error("")));

// The fast digital I/O functions only work when the pin is known at compile time.
inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
{
  if(!__builtin_constant_p(pin))
    badArg("Fast digital pin must be a constant");
}


/* Analog reference options - Configuring these is very simple, unlike tinyAVR 0/1
   and megaAVR 0, and like tinyAVR 2-series. */
#define INTERNAL1V024            (VREF_REFSEL_1V024_gc)
#define INTERNAL2V048            (VREF_REFSEL_2V048_gc)
#define INTERNAL4V096            (VREF_REFSEL_4V096_gc)
#define INTERNAL4V1              (VREF_REFSEL_4V096_gc)
#define INTERNAL2V5              (VREF_REFSEL_2V500_gc)
#define INTERNAL2V500            (VREF_REFSEL_2V500_gc)
#define DEFAULT                  (VREF_REFSEL_VDD_gc)
#define VDD                      (VREF_REFSEL_VDD_gc)
#define EXTERNAL                 (VREF_REFSEL_VREFA_gc)

// Defines something as an ADC channel, as opposed to a digital pin number
// This is the same convention that ATTinyCore uses, with high bit indicating
// that a value is a channel number not a pin number.
#define ADC_CH(ch)               (0x80 | (ch))


// DACREFn MUXPOS currently missing from the headers!!
#define ADC_DAC0                 ADC_CH(ADC_MUXPOS_DAC0_gc)
#define ADC_GROUND               ADC_CH(ADC_MUXPOS_GND_gc)
#define ADC_DACREF0              ADC_CH(0x49)
#ifdef AC1 // Always either 1 AC or 3, never 2.
  #define ADC_DACREF1            ADC_CH(0x4A)
  #define ADC_DACREF2            ADC_CH(0x4B)
#endif
#define ADC_TEMPERATURE          ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
#ifdef MVIO
  #define ADC_VDDDIV10           ADC_CH(ADC_MUXPOS_VDDDIV10_gc)
  #define ADC_VDDIO2DIV10        ADC_CH(ADC_MUXPOS_VDDIO2DIV10_gc)
#endif
#define ADC_ACC2        (0x81)
#define ADC_ACC4        (0x82)
#define ADC_ACC8        (0x83)
#define ADC_ACC16       (0x84)
#define ADC_ACC32       (0x85)
#define ADC_ACC64       (0x86)
#define ADC_ACC128      (0x87)
#define VCC_5V0                  (2)
#define VCC_3V3                  (1)
#define VCC_1V8                  (0)

#define interrupts()             sei()
#define noInterrupts()           cli()


#define ADC_ERROR_BAD_PIN_OR_CHANNEL                (-32765)
#define ADC_ERROR_DISABLED                          (-32767)
#define ADC_ERROR_BUSY                              (-32766)
#define ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL       (-2100000000)

// positive channel is not (0x80 | valid_channel) nor a digital pin number
// referring to a pin with analog input.
#define ADC_ENH_ERROR_BUSY                     (-2100000001)
// The ADC is currently performing another conversion in the background (either
// in free-running mode or a long-running burst conversion; taking a burst
// accumulated reading and then calling a specified function when the result
// was finally ready may be supported in a future version.
#define ADC_ENH_ERROR_RES_TOO_LOW              (-2100000003)
// analogReadEnh() must not be called with a resolution lower than 8-bits.
// you can right-shift as well as the library can, without wasting flash for everyone who doesn't need to.
#define ADC_ENH_ERROR_RES_TOO_HIGH             (-2100000004)
// Only resonlutions that can be generated through accumulator oversample
// + decimation are supported, maximum is 13, 15, or 17 bits. This will
// also be returned if a larger raw accumulated result is requested.
#define ADC_DIFF_ERROR_BAD_NEG_PIN             (-2100000005)
// Analog pin given as negative pin is not a valid negative mux pin
#define ADC_ENH_ERROR_NOT_DIFF_ADC             (-2100000006)
// analogReadDiff() called from a part without a differential ADC;
// Never actually returned, because we give compile error here
#define ADC_ENH_ERROR_DISABLED                 (-2100000007)
// The ADC is not currently enabled. This error is disabled currently - if analogReadEnh encounters a disabled ADC, it will enable it, take the reading, and disable it again.
#define ADC_ERROR_INVALID_CLOCK                     (-32255)
// Returned by analogClockSpeed if the value in the register is currently unknown, or if an invalid frequency is requested.


// only returned by analogCheckError()
#define ADC_IMPOSSIBLE_VALUE                        (-127)


#if (!defined(TCB_CLKSEL2_bm))
  // This means it's a tinyAVR 0/1-series, or a megaAVR 0-series.
  // Their TCB_CLKSEL enums use different names for the clock settings, for reasons unclear.
  // To align with the future, we use the Dx-series names for these.
  #define TCB_CLKSEL_DIV2_gc TCB_CLKSEL_CLKDIV2_gc
  #define TCB_CLKSEL_DIV1_gc TCB_CLKSEL_CLKDIV1_gc
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()


// NON-STANDARD API


void init_ADC0(void); /* Called by init() after clock is set */
#if defined(ADC1)
  void init_ADC1(void); /* Never called automatically, but must be called manuaklkly in order to use the ADC1 functions. */
#endif
void init_clock(void);/* called by init() first  */
void init_millis();   /* called by init() last   */
void init_timers();   /* called by init()        */
void init_TCA0();     /* called by init_timers() */
void init_TCD0();     /* called by init_timers() */

// callbacks normally empty and optimized away.
void onPreMain();
void onBeforeInit();
uint8_t onAfterInit();
void initVariant();


// Peripheral takeover
// These will remove things controlled by
// these timers from analogWrite()/turnOffPWM()
// 0x40 - TCD0, 0x10 - TCA0
void takeOverTCA0();
void takeOverTCD0();

// millis() timer control
void stop_millis();                   // Disable the interrupt and stop counting millis.
void restart_millis();                // Reinitialize the timer and start counting millis again
void set_millis(uint32_t newmillis);  // set current millis time.
/* Expected usage:
 * uint32_t oldmillis=millis();
 * stop_millis();
 * user_code_that_messes with timer
 * set_millis(oldmillis+estimated_time_spent_above)
 * restart millis();
 *
 * Also, this might at times be appropriate
 * set_millis(millis() + known_offset);
 * after doing something that we know will block too long for millis to keep time
 * see also:
 */


/* inlining of a call to delayMicroseconds() would throw it off */
void _delayMicroseconds(unsigned int us) __attribute__((noinline));


/*******************************************************
 * Extended API - Enhanced DxCore features             *
 * These are functions users might call from the       *
 * sketch, or in some cases, override.                 *
 * 1. Timer + millis() control.                        *
 * 2. initialization routines to override              *
 * 3. Advanced Analog functionality                    *
 * 4. Advanced Digital functionality                   *
 ******************************************************/
// stop, restart and set millis intended for switching to RTC for millis timekeeping while sleeping.
void stop_millis();                          // stop the timer being used for millis, and disable the interrupt.
void restart_millis();                       // After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.
void set_millis(uint32_t newmillis);         // Sets the millisecond timer to the specified number of milliseconds. DO NOT CALL with a number lower than the current millis count if you have any timeouts ongoing.
                                             // they may expire instantly.
// void nudge_millis(uint16_t nudgemillis);  // Sets the millisecond timer forward by the specified number of milliseconds. Currently only implemented for TCB, TCA implementation will be added. This allows a clean
// Not yet implemented, debating if          // way to advance the timer without needing to read the current millis yourself, and without a few other risks. (added becauise *I* needed it, but simple enough).
// this is the right thing to implement      // The intended use case is when you know you're disabling millis for a long time, and know exactly how long that is (ex, to update neopixels), and want to nudge the timer
                                             // forward by a given amount; I added this when in a pinch because *I* needed that functionality.

// Allows for user to mark a timer "do not touch" for purposes of analogWrite and the like, so you can take over a timer and reconfigure it, and not worry about digitalWrite() flipping a CMPEN bit.
// On megaTinyCore this also prevents the situation where PWM is remapped, but then when the user is digitalWrite'ing pins that default to having PWM, it would turn off the PWM now coming from another pin
// This is not an issue because we fully support portmux (can't spare the flash overhead on the tinies, people already complain that the core uses too much flash)
void takeOverTCA0();                         // Can be used to tell core not to use TCA0 for any API calls - user has taken it over.
void takeOverTCA1();                         // Can be used to tell core not to use TCA1 for any API calls - user has taken it over.
void takeOverTCD0();                         // Can be used to tell core not to use TCD0 for any API calls - user has taken it over.
void resumeTCA0();                           // Restores core-mediated functionality that uses TCA0 and restores default TCA0 configuration.
void resumeTCA1();                           // Restores core-mediated functionality that uses TCA1 and restores default TCA1 configuration.
// bool digitalPinHasPWM(uint8_t p);         // Macro. Returns true if the pin can currently output PWM using analogWrite(), regardless of which timer is used and considering current PORTMUX setting
uint8_t digitalPinToTimerNow(uint8_t p);     // Returns the timer that is associated with the pin now (considering PORTMUX)

// These are in here so that - should it be necessary - library functions or user code could override these.
void init_clock()     __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()      __attribute__((weak)); // this is called to initialize ADC0
//   init_DAC0()                             // no _init_DAC0() - all that the core does is call DACReference().
void init_TCA0()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA0 will not function.
void init_TCA1()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA1 will not function, nor will the TCBs unless the clock source is changed.
void init_TCBs()      __attribute__((weak)); // called by init_timers()
void init_TCD0()      __attribute__((weak)); // called by init_timers()
void init_millis()    __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.

void onClockFailure() __attribute__((weak)); // called by the clock failure detection ISR. Default action is a blink code with 4 blinks.
void onClockTimeout() __attribute__((weak)); // called if we try to switch to external clock, but it doesn't work. Default action is a blink code with 3 blinks.

#ifndef CORE_ATTACH_OLD
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
void         pinModeFast(uint8_t pinNumber,  uint8_t mode);
void       openDrainFast(uint8_t pinNumber,   uint8_t val);
void        pinConfigure(uint8_t pinNumber, uint16_t mode);
void          turnOffPWM(uint8_t pinNumber               );

// Not a function, still important
#define digitalPinHasPWMNow(p)            (digitalPinToTimerNow(p) != NOT_ON_TIMER)

uint8_t PWMoutputTopin(uint8_t timer, uint8_t channel);
// Realized we're not going to be able to make generic code without this.

// Again as above, but this time with the unwieldy 8-byte integer datatype as the base
// avr-libc defines _NOP() since 1.6.2
// Really? Better tell avr-gcc that, it seems to disagree...
#ifndef _NOP
  #define _NOP()    __asm__ __volatile__ ("nop")
#endif
#ifndef _NOP2
  #define _NOP2()   __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOPNOP
  #define _NOPNOP() __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOP8
  #define _NOP8()   __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* 2 clk jump over next instruction */ \
                                          "ret"       "\n\t"   /* 4 clk return "wha? why here?" */    \
                                          "rcall .-4" "\n\t" ) /* 2 clk "Oh, I see. We jump over a return (2 clock) call it, and then immediately return." */
#endif

/*
Not enabled. Ugly ways to get delays at very small flash cost.
#ifndef _NOP6
  #define _NOP6()   __asm__ __volatile__ ("rcall lonereturn") // 2 bytes of flash.  2+4=6 clk only works if you've got _LONE_RETURN() somewhere. Only guaranteed to work on 8k and smaller parts.
  #define _NOP7()   __asm__ __volatile__ ("call lonereturn")  // 4 bytes of flash.  3+4=7 clk and see above, except that this will only work w/>8k flash.
  #define _LONE_RETURN() __asm__ __volatile__ ("rjmp .+2"    "\n\t"  \  // 4 bytes of flash overhead, but must exist once and only once for NOP6/7 (but not any others) . Don't trip over thr ret. Note that if you're writing inline assembly with ret elsewhere, just proceed
                                               "lonereturn:" "\n\t"  \  // it with a a label and jump to it to save 2 bytes vs this methodMust exist somwehere for
                                                 "ret"         "\n\t" )
  // It could be put into it's own function and marked with the "used" attribute. This allows elimination of the initial rjmp, at the cost of making an ugly hack even uglier.
  // Or even worse, you have other inline assembly, and you just stick the label right before the return!
  // Really, these are things you shoudnt do unless you have your back against the flash/RAM limits and a gun to your head.
  #endif
*/
#ifndef _NOP14
  #define _NOP14()  __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* same idea as above. */ \
                                          "ret"       "\n\t"   /* Except now it's no longer able to beat the loop if it has a free register. */ \
                                          "rcall .-4" "\n\t"   /* so this is unlikely to be better, ever. */ \
                                          "rcall .-6" "\n\t" )
#endif
/* Beyond this, just use a loop.
 * If you don't need submicrosecond accuracy, just use delayMicroseconds(), which uses very similar methods. See Ref_Timers
 * (eg, ldi (any upper register), n; dec r0; brne .-4)
 * and pad with rjmp or nop if needed to get target delay.
 * Simplest form takes a uint8_t and runs for 3n cycles in 3 words. Padded with `nop` or `rjmp .+0`for 3n + 1 or 3n + 2 if outside the loop, 4n or 5n if padded inside the loop
 * And so on. You will likely end up doing something like
 *
                    #define CLOCKS_PER_US   (F_CPU / 1000000);    // preprocessed away
                    #define DELAYCLOCKS     (0.8 * CLOCKS_PER_US) // say we wanted a 0.8 us delay.
                    uint8_t x = DELAYCLOCKS / 3;                  // preprocessed into a constant
                    __asm__ __volatile__ ("dec %0"      "\n\t"    // before this, an ldi is used to load x into the input operand %0
                                          "brne .-4"    "\n\t"
                      #if (DELAYCLOCKS % 3 == 2)                  // 2 clocks extra needed at end
                                          "rjmp .+0"    "\n\t"
                      #elif (DELAYCLOCKS % 3 == 1)                // 1 clock extra needed at end
                                          "nop"         "\n\t"
                      #endif
                                          : "+d"((uint8_t)(x));
 *
 * The above will take very close to 0.8us under most any conditions.  Notice how all the calculation was moved to the preprocessor.
 *
 *
 * You can extend the length of the iterations by adding nop between the dec and brne, and branching 2 bytes further. that makes it 4 clocks per iteration.
 * You can go for much longer by using 16-bits:
 *                  uint16_t x = 2000;    * overall takes 8 bytrsd
 *                  __asm__ __volatile__ ("sbiw %0,1"    "\n\t"  // Line is preceded by 2 implied LDI's to fill that upper register pair, Much higher chance of having to push and pop.
 *                                        \"brne .-4      \"\n\t\"  // SBIW takes 2 clocks. branch takes 2 clocks unless it doesn't branch, when it only takes one
 *                                        : +w"((uint16_t)(x))  // hence this takes 4N+1 clocks (4 per iteration, except for last one which is only 3, plus 2 for the pair of LDI's)
 *
 */


// The fastest way to swap nybbles
#ifndef _SWAP
  #define _SWAP(n) __asm__ __volatile__ ("swap %0"  "\n\t" :"+r"((uint8_t)(n)));
#endif
// internally used - fast multiply by 0x20 that assumes x < 8, so you can add it to a uint8_t* to PORTA or member of PORTA,
// and get the corresponding value for the other port, or equivalently it can be added to 0x0400 which is the address of PORTA.
// Valid only with a valid port number, which must be verified first
// This exists to sidestep inefficiency of compiler generated code when you only know the port at runtime, for the very common task of
// addressing a port register by port number and offset. Trashes the variable you pass it
/*
#define _WRITE_VALUE_TO_PORT_OFFSET(p,o,v) ({
          __asm__ __volatile__ (
            "swap %0A"        "\n\t" // start with a 16-bit pointer register
            "add %0A, %0A "   "\n\t" // low byte for port register is port * 0x20 - so swap nybbles and leftshift to do in 2 cycles.
            "ldi %0B, 0x04"   "\n\t" // high byte for all port registers is 0x0400
            "add %0A, %1"     "\n\t" // add the offset within the port
            "st %a0, %2"      "\n\t" // write the value
           :"+e"(uint16_t)(p)
           :"r"  (uint8_t)(o),
            "r"  (uint8_t)(v)
          );
})
*/
uint16_t clockCyclesPerMicrosecond();
uint32_t clockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToClockCycles(uint32_t microseconds);

// Currently DxCore has no cases where the millis timer isn't derived from system clock, but that will change
/* This becomes important when we support other timers for timekeeping. The Type D timer can be faster, requiring:
 * These talk about the timebase from which millis is derived, not the actual timer counting frequency.
 * That is, it's used to calculqte the values that are we multipliy by the prescaler to get the timekeeping stuff.
 * These can be different from the above only when the millis timer isn't running from CLK_PER.
     For example, if we run it from a TCD clocked from internal HF but we are running on a crystal.
     That's a strange way to use the TCD, but
 * megaTinyCore has these, and we will have wsomething analogous.

uint16_t millisClockCyclesPerMicrosecond();
uint32_t millisClockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToMillisClockCycles(uint32_t microseconds);

 * the time and sleep library will require some things like this.
 */


// Copies of above for internal use, and for the really exotic use cases that want this instead of system clocks (basically never in user-land)
uint16_t millisClockCyclesPerMicrosecond();
unsigned long millisClockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToMillisClockCycles(unsigned long microseconds);
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
/* More may be implemented here in the future */

#define NOT_ON_TIMER    (0x00)
#define TIMERA0         (0x10) // A "simple" type A timer mapping doesn't get constants for the WO channels.
#define TIMERA1         (0x08) // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERB0         (0x20) // TCB0
#define TIMERB1         (0x21) // TCB1
#define TIMERB2         (0x22) // TCB2
#define TIMERB3         (0x23) // TCB3
#define TIMERB4         (0x24) // TCB4
#define TIMERD0         (0x70) // If any of these bits match it's potentially on TCD0
#define DACOUT          (0x80)
/* The above are all used in the digitalPinToTimer() macro and appear in the timer table, in addition to being how we identify millis timer.
 * For the millis timer, there's nothing weird here.
 * But the timer table constants contain more information than that for these. When user code interprets the timer table entries it is critical to do it right:
 *  1. If 0x40 is set, TCD0 can output here. bits 4 and 5 contain information on what channel, and bits 0-2 specify what the PORTMUX must be set to.
 *  2. If 0x20 us set, there is a TCB can output PWM there.
 *    2a. If 0x20 is set, check 0x10 - if that's set, it's the alt pin mapping. This is currently not returned by the table, and I assess it to be unlikely to be of use
 *  4. If 0x10 is set, it's a TCA0 pin. This is never used in the timer table, but digitalPinToTimerNow() can return it.
 *  5. If 0x08 is set, it's a TCA1 pin. This is never used in the timer table, but digitalPinToTimerNow() can return it.
 * Ergo, use bitwise ands
 */

#define TIMERRTC        (0x90) // RTC with internal osc
#define TIMERRTC_XTAL   (0x91) // RTC with crystal
#define TIMERRTC_CLK    (0x92) // RTC with ext clock

/* Not used in table */
#define TIMERA0_MUX0    (0x10) // Mapping0 (PORTA 0-5)
#define TIMERA0_MUX1    (0x11) // Mapping1 (PORTB 0-5)
#define TIMERA0_MUX2    (0x12) // Mapping2 (PORTC 0-5)
#define TIMERA0_MUX3    (0x13) // Mapping3 (PORTD 0-5)
#define TIMERA0_MUX4    (0x14) // Mapping4 (PORTE 0-5)
#define TIMERA0_MUX5    (0x15) // Mapping5 (PORTF 0-5)
#define TIMERA0_MUX6    (0x16) // Mapping6 (PORTG 0-5)
#define TIMERA0_MUX7    (0x17) // Mapping7 (PORTA 0-5)
#define TIMERA1_MUX0    (0x08) // Mapping0 (PORTB 0-5)
#define TIMERA1_MUX1    (0x09) // Mapping1 (PORTC 4-6) - only three channels available.
#define TIMERA1_MUX2    (0x0A) // Mapping2 (PORTE 4-6) - only three channels available.
#define TIMERA1_MUX3    (0x0B) // Mapping3 (PORTG 0-5) - DB-series only due to errata.
#define TIMERA1_MUX4    (0x0C) // Mapping4 (PORTA 4-6) - only three channels available.
#define TIMERA1_MUX5    (0x0D) // Mapping5 (PORTD 4-6) - only three channels available.

/* Not used in table or at all, yet */
#define TIMERB0_ALT     (0x30) // TCB0 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB1_ALT     (0x31) // TCB1 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB2_ALT     (0x32) // TCB2 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB3_ALT     (0x33) // TCB3 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB4_ALT     (0x34) // TCB4 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB0_ALT     (0x30) // TCB0 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB1_ALT     (0x31) // TCB1 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB2_ALT     (0x32) // TCB2 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB3_ALT     (0x33) // TCB3 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB4_ALT     (0x34) // TCB4 with alternate pin mapping - DANGER: NOT YET USED BY CORE.


 // 0b01MC 0mmm - the 3 lowest bits refer to the PORTMUX.
//                            bit C specifies whether it's channel A (0) or B (1). If M is 1 it is WOC outputting chan A or WOB outputting D.
//                            WOD outputting A or WOC outputting B is not supported by the core. WOB outputting A or WOA outputting B is not supported by the hardware.
//                            Hence, PORTMUX.TCDROUTEA == (timer table entry) & (0x07)
//                            and any table entry > 0x40 but less than 0x80 could be a TCD
//
#define TIMERD0_0WOA      (0x40) // PORTA
#define TIMERD0_0WOB      (0x50)
#define TIMERD0_0WOC      (0x60)
#define TIMERD0_0WOD      (0x70)
#define TIMERD0_1WOA      (0x41) // PORTB
#define TIMERD0_1WOB      (0x51)
#define TIMERD0_1WOC      (0x61)
#define TIMERD0_1WOD      (0x71)
#define TIMERD0_2WOA      (0x42) // PORTF
#define TIMERD0_2WOB      (0x52)
#define TIMERD0_2WOC      (0x62)
#define TIMERD0_2WOD      (0x72)
#define TIMERD0_3WOA      (0x43) // PORTG
#define TIMERD0_3WOB      (0x53)
#define TIMERD0_3WOC      (0x63)
#define TIMERD0_3WOD      (0x73)
#define TIMERD0_4WOA      (0x44) // this is PA4, duplicates mux 0.
#define TIMERD0_4WOB      (0x54) // this is PA5, duplicates mux 0.
#define TIMERD0_4WOC      (0x64) // second half is PORTD
#define TIMERD0_4WOD      (0x74)
/*
// For future use
#define TIMERD0_5WOA      (0x45) // hypothetical TCD0 WOA ALT5
#define TIMERD0_5WOB      (0x55) // hypothetical TCD0 WOB ALT5
#define TIMERD0_5WOC      (0x65) // hypothetical TCD0 WOC ALT5
#define TIMERD0_5WOD      (0x75) // hypothetical TCD0 WOD ALT5
#define TIMERD0_6WOA      (0x46) // hypothetical TCD0 WOA ALT6
#define TIMERD0_6WOB      (0x56) // hypothetical TCD0 WOB ALT6
#define TIMERD0_6WOC      (0x66) // hypothetical TCD0 WOC ALT6
#define TIMERD0_6WOD      (0x76) // hypothetical TCD0 WOD ALT6
#define TIMERD0_7WOA      (0x47) // hypothetical TCD0 WOA ALT7
#define TIMERD0_7WOB      (0x57) // hypothetical TCD0 WOB ALT7
#define TIMERD0_7WOC      (0x67) // hypothetical TCD0 WOC ALT7
#define TIMERD0_7WOD      (0x77) // hypothetical TCD0 WOD ALT7
*/

__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us);


// Get the bit location within the hardware port of the given virtual pin.
// This comes from the arrays in the  pins_arduino.c file for the active
// board configuration.
// These perform slightly better as macros compared to inline functions

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
// These mostly take the same numeric value, but used for improved code readability
#define NOT_A_PIN         (255) // When you try to get a pin number, make sure it's not NOT_A_PIN before proceeding
#define NOT_A_PORT        (255) // as above for ports.
#define NOT_AN_INTERRUPT  (255) // As above, for interrupts
#define NOT_A_CHANNEL     (255) // for channel identification on ea-series
#define NOT_A_MUX         (255) //
// Notice the different value!
#define MUX_NONE          (128)
// When cast to int8_t these are -1, but it is critical to define them as 255, not -1 because we check if they're less than the number of something

//for PWMchannelToPin() utility function only:
#define TIMER_NOT_CONNECTED (254) // If we search and find that there is currently NO pin that could (ever) output the specified PWM, we return this
#define TIMER_BROKEN_ERRATA (253) // On DA and DB parts lacking the fix, TCD routing options other than 0 are hopelessly broken

#define PA (0)
#define PB (1)
#define PC (2)
#define PD (3)
#define PE (4)
#define PF (5)
#define PG (6)
#define NUM_TOTAL_PORTS (7) /* one could argue that this should be 6 except on 64-pin parts, and that parts that don't
have ports shoulod have those Px constants defined as NOT_A_PORT. I think that would cause problems rather than solve them, though */

#define PERIPHERAL_IN_USE (254) // Returned when a rare few functions are asked about a peripheral that is not configured for that use.
// This is only currently used by digitalPinToTimerNow(pin) which returns the timer that can output PWM on a given pin, considering the current PORTMUX settings.
// This will be returned when the pin is driven by a TCB not configured for PWM. It will not otherwise be returned - so you can't use it to test whether your code
// has stomped on the configuration of TCA/TCD timers such that they aren't able to output PWM, consistent with the guiding principles that the core was written
// in accordance with. (Namely, that if you're setting registers directly, you're responsible for your own actions, and for tracking them. Since tone() and the
// millis timekeeping can render the TCBs unavailable for PWM. You should use takeOverTCA0/TCA1/TCD0() if reconfiguring the timers in this way, which will also
// cause digitalPinToTimerNow() to return NOT_ON_TIMER

/*******************************************************************
 * PIN CONFIGURE Set any or all pin settings, easily invert, etc   *
 *******************************************************************
// These are used as the second argument to pinConfigure(pin, configuration)
// You can bitwise OR as many of these as you want, or just do one. Very
// flexible function; not the world's fastest though. Directives are handled
// in the order they show up on this list, by pin function:
// PIN_DIR      Direction
// PIN_OUT      Output value
// PIN_ISC      Enable and interrupt mode. If interrupts are turned on w/out the ISR, it will trigger dirty reset.
// PIN_PULLUP   Pullups
// PIN_INLVL    Input levels (MVIO parts only - everything else is schmitt trigger only, except on I2C pins acting as I2C with SMBus levels enabled. )
// PIN_INVERT   Invert pin
//
// Systematically named constants can be made by combining those names with the postfixes here
// except for PIN_ISC which is not a non-binary option. Valid values are listed below.
// _SET, _CLR, and _TGL can be used as a postfix on all binary options.
// _TOGGLE and _TGL are interchangeable as well.
// Additional names are defined where they might be easier to remember.
// It's not an accident that the PORT options have PIN_(name of register in PORTx)
// as an alias.
// Microchip can add one more binary option >.>

/* normal PORT binary options */
#define PIN_DIR_SET          (0x0001) // OUTPUT
#define PIN_DIRSET           (0x0001) // alias
#define PIN_DIR_OUTPUT       (0x0001) // alias
#define PIN_DIR_OUT          (0x0001) // alias
#define PIN_DIR_CLR          (0x0002) // INPUT
#define PIN_DIRCLR           (0x0002) // alias
#define PIN_DIR_INPUT        (0x0002) // alias
#define PIN_DIR_IN           (0x0002) // alias
#define PIN_DIR_TGL          (0x0003) // TOGGLE INPUT/OUTPUT
#define PIN_DIRTGL           (0x0003) // alias
#define PIN_DIR_TOGGLE       (0x0003) // alias
#define PIN_OUT_SET          (0x0004) // HIGH
#define PIN_OUTSET           (0x0004) // alias
#define PIN_OUT_HIGH         (0x0004) // alias
#define PIN_OUT_CLR          (0x0008) // LOW
#define PIN_OUTCLR           (0x0008) // alias
#define PIN_OUT_LOW          (0x0008) // alias
#define PIN_OUT_TGL          (0x000C) // CHANGE/TOGGLE
#define PIN_OUTTGL           (0x000C) // alias
#define PIN_OUT_TOGGLE       (0x000C) // alias
// reserved                  (0x0010) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0020) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0030) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0040) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0050) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0060) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0070) // reserved - couldn't be combined with the ISC options
/* Interrupt and input enable nybble is: 0b1nnn to set to option n, or 0b0xxx to not, and ignore those bits. */
#define PIN_ISC_ENABLE       (0x0080) // No interrupts and enabled.
#define PIN_INPUT_ENABLE     (0x0080) // alias
#define PIN_ISC_CHANGE       (0x0090) // CHANGE
#define PIN_INT_CHANGE       (0x0090) // alias
#define PIN_ISC_RISE         (0x00A0) // RISING
#define PIN_INT_RISE         (0x00A0) // alias
#define PIN_ISC_FALL         (0x00B0) // FALLING
#define PIN_INT_FALL         (0x00B0) // alias
#define PIN_ISC_DISABLE      (0x00C0) // DISABLED
#define PIN_INPUT_DISABLE    (0x00C0) // alias
#define PIN_ISC_LEVEL        (0x00D0) // LEVEL
#define PIN_INT_LEVEL        (0x00D0) // alias
/* PINnCONFIG binary options */
#define PIN_PULLUP_ON        (0x0100) // PULLUP ON
#define PIN_PULLUP           (0x0100) // alias
#define PIN_PULLUP_SET       (0x0100) // alias
#define PIN_PULLUP_OFF       (0x0200) // PULLUP OFF
#define PIN_PULLUP_CLR       (0x0200) // alias
#define PIN_NOPULLUP         (0x0200) // alias
#define PIN_PULLUP_TGL       (0x0300) // PULLUP TOGGLE
#define PIN_PULLUP_TOGGLE    (0x0300) // alias
// reserved                  (0x0400) // reserved
// reserved                  (0x0800) // reserved
// reserved                  (0x0C00) // reserved
#define PIN_INLVL_TTL        (0x1000) // TTL INPUT LEVELS - DD, DB, EA (maybe all future non-tinies!)
#define PIN_INLVL_ON         (0x1000) // alias
#define PIN_INLVL_SET        (0x1000) // alias
#define PIN_INLVL_SCHMITT    (0x2000) // SCHMITT INPUT LEVELS
#define PIN_INLVL_OFF        (0x2000) // alias
#define PIN_INLVL_CLR        (0x2000) // alias
// reserved                  (0x3000) // INLVL TOGGLE - not supported. If you tell me a reasonable use case
// I'll do it. But when would you ever just want to switch the inlvl from whatever it is now? Don't you want to put it to either schmitt or ttl and know which one you want?each possible value is handled separately, slowing it down, and I don't think this would get used.
#define PIN_INVERT_ON        (0x4000) // PIN INVERT ON
#define PIN_INVERT_SET       (0x4000) // alias
#define PIN_INVERT_OFF       (0x8000) // PIN INVERT OFF
#define PIN_INVERT_CLR       (0x8000) // alias
#define PIN_INVERT_TGL       (0xC000) // PIN_INVERT_TOGGLE
#define PIN_INVERT_TOGGLE    (0xC000) // alias



/*
Supplied by Variant file:
#define digitalPinToAnalogInput(p)      // Given digital pin (p), returns the analog channel number, or NOT_A_PIN if the pin does not suipport analog input.
#define analogChannelToDigitalPin(p)    // Inverse of above. Given analog chanbel number (p) in raw form not ADC_CH() form, returns the digital pin number corresponding to it.
#define analogInputToDigitalPin(p)      // Similar to previous. Given analog input number (p) with the high bit set, returns the digital pin number corresponding to it)
#define digitalOrAnalogPinToDigital(p)  // Given either an analog input number (with high bit set) or a digital pin number (without it set), returns the digital pin number.
Yes, these are poorky named and do not use analog input, analog pin, and analog channel consistently. Unfortunately the names of some of these were set in stone by virtue of their being preexisting macros used in code in the wild.
See Ref_Analog.md for more information of the representations of "analog pins". I blame Arduino for the original sin of "analog pins" as a concept in the first place.
*/

#define digitalPinToPort(pin)               (((pin)     < NUM_TOTAL_PINS ) ?                          digital_pin_to_port[pin]                 : NOT_A_PIN)
#define digitalPinToBitPosition(pin)        (((pin)     < NUM_TOTAL_PINS ) ?                  digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define digitalPinToBitMask(pin)            (((pin)     < NUM_TOTAL_PINS ) ?                      digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?   digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?       digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#define digitalPinToTimer(pin)              (((pin)     < NUM_TOTAL_PINS ) ?                         digital_pin_to_timer[pin]                 : NOT_ON_TIMER)
#define portToPortStruct(port)              (((port)    < NUM_TOTAL_PORTS) ?                   (((PORT_t *)  &PORTA) + (port))                 : NULL)
#define digitalPinToPortStruct(pin)         (((pin)     < NUM_TOTAL_PINS ) ?    (((PORT_t *) &PORTA) + digitalPinToPort( pin))                 : NULL)
#define getPINnCTRLregister(port, bit_pos)  ((((port) != NULL) && (bit_pos < 8)) ? (((volatile uint8_t *) &(port->PIN0CTRL)) + bit_pos)        : NULL)
#define digitalPinToInterrupt(P)            (P)
// Remember to test for NOT_A_PORT before using thiese.
#define portOutputRegister(P) ((volatile uint8_t *)(&portToPortStruct(P)->OUT))
#define portInputRegister(P)  ((volatile uint8_t *)(&portToPortStruct(P)->IN ))
#define portModeRegister(P)   ((volatile uint8_t *)(&portToPortStruct(P)->DIR))
#if defined(PORTA_EVGENCTRL) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.
  #define portEventRegister(p)  ((volatile uint8_t *)(&portToPortStruct(P)->EVGENCTRL))
  uint8_t _setRTCEventChan(uint8_t val, uint8_t chan);
  uint8_t _setEventPin(uint8_t pin, uint8_t number); // preliminary thought - pass a pin number, it looks up port, and from there the event control register and sets it.
  //Number being 0 or 1 or 255 to pick the lowest numbered one not set. Returns event channel number TBD if that should be the EVSYS valus or 0 or 1. If "Pick unused ome" is requested but both already assigned, will return 255
  uint8_t _getPortEventConfig(uint8_t port); // just shorthand for looking up the port and returning it's EVGENCTRL value
  uint8_t _setRTCEventChan(uint8_t vail, uint8_t chan); // number is 0, 1 or 255 like above, div is log(2) of the divisor (ie, for 2^5, his would be 5).
  uint8_t _getRTCEventConfig(); //simply returns the RTC channel configuration. Will likely return 255 if called on non Ex
  uint8_t _RTCPrescaleToVal(uint16_t prescale)
#endif
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  #include "UART.h"
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int16_t analogClockSpeed(int16_t frequency = 0,  uint8_t options = 0);
#endif

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
#else
  // Allow for the possibility that someoen directly defines these when using PlatformIO or similar.
  #if !(defined(TWI_MORS) || defined(TWI_MANDS))
    #warning "No wire mode specified - either a menu option must be selected (if using Arduino IDE) or the defines manually provided (if using PlatformIO/etc) - defaulting to TWI_MORS"
    #define TWI_MORS
  #endif
#endif

#include "pins_arduino.h"
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
  #if CLOCK_SOURCE == 1
    #define NUM_INTERNALLY_USED_PINS    (2) // Extermal crystal tales PA0 amd PA1
  #elif CLOCK_SOURCE == 2
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



#include "pinswap.h"




// A little bit of trickery - this allows Serial to be defined as something other than USART0
// Use case is for boards where the main serial port is not USART0 to be used without the user
// having to find/replace Serial with Serial2 or whatever on their existing code if that's where
// the monitor is. It requires that the board be defined by a variant file
#ifndef Serial
  #define Serial Serial0 // Error here? Check for missing ; previous line in sketch.
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
#if !defined(SERIAL_PORT_MVIO) && defined(MVIO) // defined on DD snd DB.
// DD-series parts with 20-or-fewer pins will not have a PC0 for the TX line of
// Serial1. that makes it difficult to , so it can't be their MVIO serial port (without involving the event
// system, of course) - but they can get a serial port on MVIO pins with USART0
// and an alternate mapping. So for those parts only, Serial is their MVIO port.
// For everyone else it's Serial1, and for non-DD parts, that is the only serial port connected to thr mvio
// serial port.
// that could be used with MVIO (again, short of rerouting signals with
// the event system)
// Note that if MVIO is disabled, we cannot detect that.
  #if defined(DD_14_PINS) || defined(DD_20_PINS)
    #define SERIAL_PORT_MVIO Serial0
    #define SERIAL_PORT_MVIO_MUX 0x04 // TX PC1 RX PC2
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


/********************************************************************************
 * CORE AND HARDWARE FEATURE SUPPORT  *
 * CORE_HAS_FASTIO is 1 when digitalReadFast and digitalWriteFast are supplied, and 2 when openDrainFast and pinModeFast are as well.
 * CORE_HAS_OPENDRAIN
 * CORE_HAS_PINCONFIG is 1 if pinConfig is supplied. The allows full configuration of any pin. It is 2 if it accepts commas instead of bitwise or between configuration parameters (NYI)
 * CORE_HAS_FASTPINMODE is 1 if pinModeFast is supplied
 * CORE_HAS_ANALOG_ENH is 0 if no analogReadEnh is supplied, 1 if it is, and 2 if it is supplied and both core and hardware support a PGA.
 * CORE_HAS_ANALOG_DIFF is 0 if no analogReadDiff is supplied, 1 if it's DX-like (Vin < VREF), and 2 if it's a proper
 * differential ADC, supported in both hardware and software. The value -1 is also valid and indicates it's a classic AVR with a  * differential ADC, see the documentation for the core.
 * CORE_HAS_TIMER_TAKEOVER is 1 if takeOverTCxn functions are provided to tell the core not to automatically use all
 * type A and D timers.
 * CORE_HAS_TIMER_RESUME is 1 if resumeTCAn functions are provided to hand control back to the core and reinitialize them.
 * CORE_DETECTS_TCD_PORTMUX is 1 if the TCD portmux works correctly on the hardware and is used by the core, 0 if it would be if
 * the harware worked, and not defined at all if the hardware doesnt have such a feature even in theory
 * CORE_SUPPORT_LONG_TONES is 1 if the core supports the three argument tone for unreasonably long tones.
 ********************************************************************************/
#define CORE_HAS_FASTIO                 (2)
#define CORE_HAS_OPENDRAIN              (1) /* DxCore has openDrain() and openDrainFast()                           */
#define CORE_HAS_PINCONFIG              (1) /* pinConfigure is now implemented                                      */
#define CORE_HAS_FASTPINMODE            (1)
#define CORE_DETECTS_TCD_PORTMUX        (1)
                                            /* we support using it */
#define CORE_HAS_TIMER_TAKEOVER         (1)
#define CORE_HAS_TIMER_RESUME           (1)
#define CORE_SUPPORT_LONG_TONES         (1)
#define CORE_HAS_ANALOG_ENH             (1)
#define CORE_HAS_ANALOG_DIFF            (1)


/* Hardware capabilities (ADC)
 * ADC_DIFFERENTIAL is 1 for a half-way differential ADC like DX-serie has, 2 for a real one like EA-series will    *
 * ADC_MAX_OVERSAMPLED_RESOLUTION is the maximum resolution attainable by oversampling and decimation               *
 * ADC_NATIVE_RESOLUTION is the higher of the two native ADC resolutions. Either 10 or 12                           *
 * ADC_NATIVE_RESOLUTION_LOW is the lower of the two native ADC resolutions. Either 8 or 10. Can't be deduced from  *
 * above. All permutations where the "native resolution" is higher are extant somewhere                             *
 * ADC_MAXIMUM_ACCUMULATE is the maximum number of sameples that can be accumulated by the burst accumulation mode  *
 * ADC_MAXIMUM_SAMPDUR is the maximum sample duration value. Refer to the core documentation or datasheet for detail*
 * ADC_RESULT_SIZE is the size (in bits) of the registers that hold the ADC result. V2.0 ADC has 32, others have 16 *
 * ADC_MAXIMUM_GAIN is defined if there is a way to amplify the input to the ADC. If you have to use onchip opamps  *
 * and the chip has them, it's -1, otherwise it is the maximum multiplier
 * ADC_REFERENCE_SET is 1 if the references are the weird ones that tinyAVR 0 and 1 use, and 2 if they are 1.024,   *
 * 2.048, 4.096 and 2.5V like civilized parts */

#define ADC_DIFFERENTIAL                (1)
#define ADC_MAX_OVERSAMPLED_RESOLUTION (15)
#define ADC_NATIVE_RESOLUTION          (12)
#define ADC_NATIVE_RESOLUTION_LOW      (10)
#define ADC_MAXIMUM_ACCUMULATE        (128)
#define ADC_MAXIMUM_SAMPDUR          (0xFF)
#define ADC_RESULT_SIZE                (16)
#if defined(__AVR_DD__) || defined(__AVR_EA__)
  #define ADC_MAXIMUM_PIN_CHANNEL      (31)
  #define ADC_MAXIMUM_NEGATIVE_PIN     (31)
#else                                       /* negative inputs! The EA even has a decent differential ADC!          */
  #define ADC_MAXIMUM_PIN_CHANNEL      (21)
  #define ADC_MAXIMUM_NEGATIVE_PIN     (15)
#endif
#if defined(ADC0_PGACTRL)                   /* The product briefs do not mention either way                         */
  #define ADC_MAXIMUM_GAIN             (16)
#elif defined(OPAMP0)
  #ifndef ADC_MAXIMUM_GAIN
    #define ADC_MAXIMUM_GAIN           (-1)  /* DB-series can use their OPAMPs as a PGA                             */
