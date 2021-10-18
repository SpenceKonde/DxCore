/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/
#include "wiring_private.h"
#include "util/delay.h"
#if (CLOCK_SOURCE == 1 || CLOCK_SOURCE == 2)
  void asmDelay(uint16_t us);
  void blinkCode(uint8_t blinkcount);
#endif

#ifndef F_CPU
  #error "F_CPU not defined"
#endif

/* Evwn when millis is off, we should still bhve access to the clock cycle counting maqcros. That's the  only way ww can get time estimates there!
*/
  inline uint16_t clockCyclesPerMicrosecond(){
    // no exception for TCD0 here, since internal osc speed isn't fixed to oine of 2 settings.
    return ((F_CPU) / 1000000L);
  }


  inline unsigned long clockCyclesToMicroseconds(unsigned long cycles){
    return (cycles / clockCyclesPerMicrosecond());
  }

  inline unsigned long microsecondsToClockCycles(unsigned long microseconds){
    return (microseconds * clockCyclesPerMicrosecond());
  }



#ifndef MILLIS_USE_TIMERNONE

#ifdef MILLIS_USE_TIMERRTC_XTAL
  #define MILLIS_USE_TIMERRTC
#endif

//volatile uint16_t microseconds_per_timer_overflow;
//volatile uint16_t microseconds_per_timer_tick;

#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) //Now TCB as millis source does not need fraction
  volatile uint32_t timer_millis = 0; //That's all we need to track here

#elif !defined(MILLIS_USE_TIMERRTC) //all of this stuff is not used when the RTC is used as the timekeeping timer
  static uint16_t timer_fract = 0;
  uint16_t fract_inc;
  volatile uint32_t timer_millis = 0;
  #define FRACT_MAX  (1000)
  #define FRACT_INC  (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) % 1000);
  #define MILLIS_INC (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) / 1000);
  volatile uint32_t timer_overflow_count = 0;
#else
  volatile uint16_t timer_overflow_count = 0;
#endif

//overflow count is tracked for all timer options, even the RTC, except for TCBs where it is equal to millis.
#if !defined(MILLIS_USE_TIMERRTC)



  #if defined(MILLIS_USE_TIMERD0)
    #ifndef TCD0
      #error "Selected millis timer, TCD0, does not exist on this part"
    #endif
  #elif !defined(MILLIS_USE_TIMERA0) && !defined(MILLIS_USE_TIMERA1)
    static volatile TCB_t* _timer =
    #if defined(MILLIS_USE_TIMERB0)
      &TCB0;
    #elif defined(MILLIS_USE_TIMERB1)
      #ifndef TCB1
        #error "Selected millis timer, TCB1 does not exist on this part."
      #endif
      &TCB1;
    #elif defined(MILLIS_USE_TIMERB2)
      #ifndef TCB2
        #error "Selected millis timer, TCB2 does not exist on this part."
      #endif
      &TCB2;
    #elif defined(MILLIS_USE_TIMERB3)
      #ifndef TCB3
        #error "Selected millis timer, TCB3 does not exist on this part."
      #endif
      &TCB3;
    #elif defined(MILLIS_USE_TIMERB4)
      #ifndef TCB4
        #error "Selected millis timer, TCB4 does not exist on this part."
      #endif
      &TCB4;
    #else  //it's not TCB0, TCB1, TCD0, TCA0, TCA1, or RTC
      #error "No millis timer selected, but not disabled - can't happen!".
    #endif
  #else
    #if !defined(TCA1) && defined(MILLIS_USE_TIMERA1)
      #error "Selected millis timer, TCA1 does not exist on this part."
    #endif
  #endif
#endif //end #if !defined(MILLIS_USE_TIMERRTC)


#if defined(MILLIS_USE_TIMERA0)
  ISR(TCA0_HUNF_vect)
#elif defined(MILLIS_USE_TIMERA1)
  ISR(TCA1_HUNF_vect)
#elif defined(MILLIS_USE_TIMERD0)
  ISR(TCD0_OVF_vect)
#elif defined(MILLIS_USE_TIMERRTC)
  ISR(RTC_CNT_vect)
#elif defined(MILLIS_USE_TIMERB0)
  ISR(TCB0_INT_vect)
#elif defined(MILLIS_USE_TIMERB1)
  ISR(TCB1_INT_vect)
#elif defined(MILLIS_USE_TIMERB2)
  ISR(TCB2_INT_vect)
#elif defined(MILLIS_USE_TIMERB3)
  ISR(TCB3_INT_vect)
#elif defined(MILLIS_USE_TIMERB4)
  ISR(TCB4_INT_vect)
#else
  #error "no millis timer selected"
#endif
{
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  #if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
    #if(F_CPU>1000000)
      timer_millis++; //that's all we need to do!
    #else //if it's 1<Hz, we set the millis timer to only overflow every 2 milliseconds, intentionally sacrificing resolution.
      timer_millis += 2;
    #endif
  #else
    #if !defined(MILLIS_USE_TIMERRTC) //TCA0 or TCD0
      uint32_t m = timer_millis;
      uint16_t f = timer_fract;
      m += MILLIS_INC;
      f += FRACT_INC;
      if (f >= FRACT_MAX) {

        f -= FRACT_MAX;
        m += 1;
      }
      timer_fract = f;
      timer_millis = m;
    #endif
    //if RTC is used as timer, we only increment the overflow count
    timer_overflow_count++;
  #endif
  /* Clear flag */
  #if defined(MILLIS_USE_TIMERA0)
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
  #elif defined(MILLIS_USE_TIMERA1)
    TCA1.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
  #elif defined(MILLIS_USE_TIMERD0)
    TCD0.INTFLAGS = TCD_OVF_bm;
  #elif defined(MILLIS_USE_TIMERRTC)
    RTC.INTFLAGS = RTC_OVF_bm;
  #else //timerb
    _timer->INTFLAGS = TCB_CAPT_bm;
  #endif
}

unsigned long millis()
{
  //return timer_overflow_count; //for debugging timekeeping issues where these variables are out of scope from the sketch
  unsigned long m;

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  uint8_t oldSREG = SREG;
  cli();
  #if defined(MILLIS_USE_TIMERRTC)
    m = timer_overflow_count;
    if (RTC.INTFLAGS & RTC_OVF_bm) { //there has just been an overflow that hasn't been accounted for by the interrupt
      m++;
    }
    SREG = oldSREG;
    m = (m << 16);
    m += RTC.CNT;
    //now correct for there being 1000ms to the second instead of 1024
    m = m - (m >> 5) - (m >> 6);
  #else
    m = timer_millis;
    SREG = oldSREG;
  #endif
  return m;
}

#ifndef MILLIS_USE_TIMERRTC

  unsigned long micros() {

    unsigned long overflows, microseconds;

    #if !defined(MILLIS_USE_TIMERA0) && !defined(MILLIS_USE_TIMERA1)
      uint16_t ticks;
    #else
      uint8_t ticks;
    #endif
    uint8_t flags;
    /* Save current state and disable interrupts */
    uint8_t oldSREG = SREG;
    cli();



    /* Get current timer count and check for OVF flag
       Do this ASAP after disabling interrupts */

    #if defined(MILLIS_USE_TIMERA0)
      ticks = TCA0.SPLIT.HCNT;
      flags = TCA0.SPLIT.INTFLAGS;
    #elif defined(MILLIS_USE_TIMERA1)
      ticks = TCA1.SPLIT.HCNT;
      flags = TCA1.SPLIT.INTFLAGS;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLE = TCD_SCAPTUREA_bm;
      while (!(TCD0.STATUS & TCD_CMDRDY_bm)); //wait for sync - should be only one iteration of this loop
      flags = TCD0.INTFLAGS;
      ticks = TCD0.CAPTUREA;
    #else
      ticks = _timer->CNT;
      flags = _timer->INTFLAGS;
    #endif //end getting ticks
    /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
    ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
    so we shouldn't increment overflows, or interrupts are disabled and micros isn't expected to work so it doesn't matter

    Next, get the current number of overflows
    */
    #if defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0)
      overflows = timer_overflow_count;
    #else
      overflows = timer_millis;
    #endif

    /* Turn interrupts back on, assuming they were on when micros was called. */
    SREG = oldSREG;

    #if defined(MILLIS_USE_TIMERD0)
      if ((flags & TCD_OVF_bm) && (ticks < 0x07)) {
    #elif defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA1)
      ticks = (TIME_TRACKING_TIMER_PERIOD) - ticks;
      if ((flags & TCA_SPLIT_HUNF_bm) && (ticks < 0x4)){
    #else //timerb
      if ((flags & TCB_CAPT_bm) && !(ticks & 0xFF00)) {
    #endif
    #if ((defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) && (F_CPU <= 1000000))
      overflows += 2;
    #else
      overflows++;
    #endif
    }

    //end getting ticks

    #if defined(MILLIS_USE_TIMERD0)
      #error "Timer D is not supported as a millis source on the AVR DA or DB series."
    #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
        // The below bitshifts implement dividing by (1/2 clocks per us) since TCB's can only prescale by 2. So we at say 36 MHz, it counts to 17999 for each ms, so  we need to divide the ticks by 1000us.
        // Oddball clock speeds
      #if   (F_CPU == 44000000UL) // Extreme overclocking (might work w/external clock)
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) - (ticks >> 5) + (ticks >> 7)); // Extremely close, and rounding will tend to help
      #elif (F_CPU == 36000000UL) // Overclocked but likely to work w/external clock or xtal
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - (ticks >> 3) + (ticks >> 6)); // Very close
      #elif (F_CPU == 28000000UL)
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 3) + (ticks >> 6) + (ticks >> 8)); // Extremely close, and rounding will tend to help.
      #elif (F_CPU == 14000000UL) /* not supported by the core! */
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks + (ticks >> 3) + (ticks >> 6) + (ticks >> 8)); // Extremely close, and rounding will tend to help.
      #elif (F_CPU == 30000000UL)
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 3) - (ticks >> 4) + (ticks >> 8)); // Damned near perfect.
      #elif (F_CPU == 25000000UL)
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 5)); // - (ticks >> 7)
        // Multiples of 12
        // + (ticks >> 3) - (ticks >> 5) is better than + (ticks >> 4) + (ticks >> 5) - same average, but alternating + and - gives less rounding error.
      #elif (F_CPU == 48000000UL) // Extreme overclocking (sometimes works w/ext. clock on E-spec parts!)
        ticks = ticks >> 5;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
      #elif (F_CPU == 24000000UL)
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
      #elif (F_CPU == 12000000UL)
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
        // multiples of 10
      #elif (F_CPU == 40000000UL) // appears to actually work!
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU == 20000000UL)
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU == 10000000UL)
        ticks = ticks >> 2;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU ==  5000000UL)
        ticks = ticks >> 1;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
        // powers of 2
      #elif (F_CPU  == 32000000UL || F_CPU > 24000000UL)
        microseconds = overflows * 1000 + (ticks >> 4);
      #elif (F_CPU  == 16000000UL || F_CPU > 12000000UL)
        microseconds = overflows * 1000 + (ticks >> 3);
      #elif (F_CPU  ==  8000000UL || F_CPU >  6000000UL)
        microseconds = overflows * 1000 + (ticks >> 2);
      #elif (F_CPU  ==  4000000UL || F_CPU >  3000000UL)
        microseconds = overflows * 1000 + (ticks >> 1);
      #else //(F_CPU == 1000000UL || F_CPU == 2000000UL) - here clock is running at system clock instead of half system clock.
            // also works at 2MHz, since we use CLKPER for 1MHz vs CLKPER/2 for all others.
        microseconds   = overflows * 1000 + ticks;
      #endif
      #if !(F_CPU == 48000000UL || F_CPU == 44000000UL || F_CPU == 36000000UL || F_CPU == 28000000UL || \
            F_CPU == 40000000UL || F_CPU == 30000000UL || F_CPU == 25000000UL || F_CPU == 20000000UL || \
            F_CPU == 10000000UL || F_CPU ==  5000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || \
            F_CPU == 32000000UL || F_CPU == 16000000UL || F_CPU ==  8000000UL || F_CPU ==  4000000UL || \
            F_CPU ==  2000000UL || F_CPU ==  1000000UL || F_CPU == 14000000UL )
        #warning "Millis timer (TCBn) at this frequency unsupported, micros() will return totally bogus values."
      #endif
    #else //TCA
      #if   (F_CPU == 48000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 5) + ((ticks >> (uint8_t) 2) + (ticks >> (uint8_t) 4) + (ticks >> (uint8_t) 5));
      #elif (F_CPU == 44000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 6) - ((ticks >> (uint8_t) 3) + (ticks >> (uint8_t) 4));
      #elif (F_CPU == 40000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 6) + ((ticks >> (uint8_t) 1) - (ticks >> (uint8_t) 3) + (ticks >> (uint8_t) 5));
      #elif (F_CPU == 36000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 7) + ((ticks >> (uint8_t) 3) - (ticks >> (uint8_t) 6));
      #elif (F_CPU == 30000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 8) + ((ticks >> (uint8_t) 1) + (ticks >> (uint8_t) 5) + (ticks >> (uint8_t) 7));
      #elif (F_CPU == 28000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 2) + ((ticks >> (uint8_t) 2) + (ticks >> (uint8_t) 5));
      #elif (F_CPU == 25000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 2) + ((ticks >> (uint8_t) 1) + (ticks >> (uint8_t) 4));
      #elif (F_CPU == 24000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 3) - ((ticks >> (uint8_t) 1) - (ticks >> (uint8_t) 3) - (ticks >> (uint8_t) 5) - (ticks >> (uint8_t) 7));
      #elif (F_CPU == 20000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 3) + ((ticks >> (uint8_t) 2) - (ticks >> (uint8_t) 4) + (ticks >> (uint8_t) 6));
      #elif (F_CPU == 12000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 5) + ((ticks >> (uint8_t) 2) + (ticks >> (uint8_t) 4) + (ticks >> (uint8_t) 5));
      #elif (F_CPU == 10000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 6) + ((ticks >> (uint8_t) 1) - (ticks >> (uint8_t) 3) + (ticks >> (uint8_t) 5));
      #elif (F_CPU ==  5000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 16)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 3) + ((ticks >> (uint8_t) 2) - (ticks >> (uint8_t) 4) + (ticks >> (uint8_t) 6));
      #else
        #if (TIME_TRACKING_TIMER_DIVIDER%(F_CPU/1000000))
          #warning "Millis timer (TCAn) at this frequency unsupported, micros() will return bogus values."
        #endif
        microseconds = ((overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
          + (ticks * (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/TIME_TRACKING_TIMER_PERIOD)));
      #endif
    #endif //end of timer-specific part of micros calculations
    return microseconds;
  }
#endif //end of non-RTC micros code


#endif //end of non-MILLIS_USE_TIMERNONE code

#if   (!(defined(MILLIS_USE_TIMERNONE) || defined(MILLIS_USE_TIMERRTC))) //delay implementation when we do have micros()
void delay(unsigned long ms)
{
  uint32_t start = micros();

  while (ms > 0) {
    yield();
    while (ms > 0 && (micros() - start) >= 1000) {
      ms--;
      start += 1000;
    }
  }
}

#else //delay implementation when we do not
void delay(unsigned long ms)
{
  _delay_ms(ms);
}
#endif

inline __attribute__((always_inline)) void delayMicroseconds(unsigned int us) {
  // This function gets optimized away, but to what depends on whether us is constant.
  if (__builtin_constant_p(us)) {
    _delay_us(us); // Constant microseconds use the avr-libc _delay_us() which is highly accurate for all values and efficient.
  } else { // If it is not, we have to use the Arduino style implementation.
    _delayMicroseconds(us);
  }
}
/* Delay for the given number of microseconds. This is UGLY AS SIN and explicitly depends on function call overhead for
 * very short delays.
 * First, we get 1 us in and return if that's what was asked for.
 * Then we use a minimal number of bitshifts to calculate the number of passes through the delay loop
 * and subtract the number of loop-cycles of time we burned doing so. But need to be careful that sane values
 * don't get so much bigger that they overflow the unsigned int we're storing it in. To that end, we use
 * a longer loop at faster clock speeds.
 * In the inline assembly, when a delay of 8 clocks or longer is required, we save flash with a clever trick:
 *  "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
 *  "ret" "\n\t"          // 4 cycles - rjmped over initially...
 *  "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
 * This exploits the fact that return is a 4-clock instruction (even on AVRxt) by first hopping over a return
 * then immediately calling that return instruction - 8 clocks in 3 words. Once the ret is there, additional
 * rcall instructions can get 6 clocks in a single word, though we only get to take advantage of that once for the 30 MHz case.
 */

#if F_CPU >= 48000000L
  // 16 MHz math, 12-cycle loop
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 40000000L
  // 16 MHz math, 10-cycle loop
  #define DELAYMICROS_TEN
#elif F_CPU >= 36000000L
  // 12 MHz math, 12-cycle loop
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 32000000L
  // 16 MHz math, 8-cycle loop
  #define DELAYMICROS_EIGHT
#elif F_CPU >= 30000000L
  // 12 MHz math, 10-cycle loop
  #define DELAYMICROS_TEN
#elif F_CPU >= 28000000L
  // 16 MHz math, 7-cycle loop
  #define DELAYMICROS_SEVEN
#elif F_CPU >= 24000000L
  // 12 MHz math, 8-cycle loop
  #define DELAYMICROS_EIGHT
#endif


__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us) {

#if F_CPU >= 48000000L

  // the loop takes 1/4 of a microsecond (12 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2; // x4 us, = 4 cycles

  // account for the time taken in the preceding commands.
  // we only burned ~14 cycles above, subtraction takes another 2 - so we've lost half a us,
  // and only need to drop 2 rounds through the loop!
  us -= 1; // = 2 cycles,

#elif F_CPU >= 40000000L

  // the loop takes 1/4 of a microsecond (4 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2; // x4 us, = 4 cycles

  // account for the time taken in the preceding commands.
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 2; //

#elif F_CPU >= 36000000L
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 10 cycles in 4 words

  // the loop takes 1/3 of a microsecond (12 cycles)
  // per iteration, so execute it three times for each microsecond of
  // delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 23 (25) cycles above, remove 2
  us -= 2; //2 cycles

#elif F_CPU >= 32000000L
  // here, we only take half a us at the start!
  __asm__ __volatile__ ("rjmp .+0"); //just waiting 2 cycles - so we're half a us
  //in by the end of this section.
  // the loop takes 1/4 of a microsecond (8 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2; // x4 us, = 4 cycles

  // account for the time taken in the preceding commands.
  // we only burned ~14 cycles above, subtraction takes another 2 - so we've lost half a us,
  // and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles

#elif F_CPU >= 30000000L
  // for the 30 MHz clock

  // for a one-microsecond delay, burn 14 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
    "ret" "\n\t"          // 4 cycles - rjmped over initially...
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
    "rcall .-6");         // 2+4 cycles - ... and here again!
                          // Waiting 14 cycles - in only 4 words!
  if (us <= 1) return; // = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (10 cycles)
  // per iteration, so execute it three times for each microsecond of
  // delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 28 (30) cycles above, remove 3
  us -= 3; //2 cycles



#elif F_CPU >= 28000000L

  // for a one-microsecond delay, burn 12 cycles and return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 12 cycles in 5 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/4 of a microsecond (7 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2; // x4 us, = 4 cycles

  // account for the time taken in the preceding commands.
  // we just burned 27 (29) cycles above, remove 4, (7*4=28)
  // us is at least 8 so we can subtract 5
  us -= 4; // = 2 cycles,


#elif F_CPU >= 24000000L
  // for the 24 MHz clock

  // for a one-microsecond delay, burn 8 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles)
  // per iteration, so execute it three times for each microsecond of
  // delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 24 (22) cycles above, remove 3
  us -= 3; //2 cycles


#elif F_CPU >= 20000000L
  // for the 20 MHz clock

  // for a one-microsecond delay, burn 4 clocks and then return (see 16MHz for logic)
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"
    "rjmp .+0"); //just waiting 4 cycles
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes a 1/5 of a microsecond (4 cycles)
  // per iteration, so execute it five times for each microsecond of
  // delay requested.
  us = (us << 2) + us; // x5 us, = 7 cycles

  // account for the time taken in the preceding commands.
  // we just burned 26 (28) cycles above, remove 7, (7*4=28)
  // us is at least 10 so we can subtract 7
  us -= 7; // 2 cycles

#elif F_CPU >= 16000000L
  // for the 16 MHz clock on most Arduino boards

  // for a one-microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1us
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/4 of a microsecond (4 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2; // x4 us, = 4 cycles

  // account for the time taken in the preceding commands.
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 5; // = 2 cycles,

#elif F_CPU >= 12000000L
  // for the 12 MHz clock...

  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (4 cycles)
  // per iteration, so execute it three times for each microsecond of
  // delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
  us -= 5; //2 cycles

#elif F_CPU >= 10000000L
  // for 10MHz (20MHz/2)

  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 2/5 of a microsecond (4 cycles)
  // per iteration, so execute it 2.5 times for each microsecond of
  // delay requested.
  us = (us << 1) + (us >> 1); // x2.5 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
  us -= 5; //2 cycles

#elif F_CPU >= 8000000L
  // for the 8 MHz internal clock

  // for a 1 and 2 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 2us
  if (us <= 2) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/2 of a microsecond (4 cycles)
  // per iteration, so execute it twice for each microsecond of
  // delay requested.
  us <<= 1; //x2 us, = 2 cycles

  // account for the time taken in the preceding commands.
  // we just burned 17 (19) cycles above, remove 4, (4*4=16)
  // us is at least 6 so we can subtract 4
  us -= 4; // = 2 cycles
#elif F_CPU >= 5000000L
  // for 5MHz (20MHz / 4)

  // for a 1 ~ 3 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 3us
  if (us <= 3) return; //  = 3 cycles, (4 when true)

  // the loop takes 4/5th microsecond (4 cycles)
  // per iteration, so we want to add it to 1/4th of itself
  us +=us >> 2;
  us -= 2; // = 2 cycles

#elif F_CPU >= 4000000L
  // for that unusual 4mhz clock...

  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 4us
  if (us <= 4) return; //  = 3 cycles, (4 when true)

  // the loop takes 1 microsecond (4 cycles)
  // per iteration, so nothing to do here! \o/

  us -= 2; // = 2 cycles


#else
  // for the 1 MHz internal clock (default settings for common AVR microcontrollers)
  // the overhead of the function calls is 14 (16) cycles
  if (us <= 16) return; //= 3 cycles, (4 when true)
  if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to subtract 22)

  // compensate for the time taken by the preceding and next commands (about 22 cycles)
  us -= 22; // = 2 cycles
  // the loop takes 4 microseconds (4 cycles)
  // per iteration, so execute it us/4 times
  // us is at least 4, divided by 4 gives us 1 (no zero delay bug)
  us >>= 2; // us div 4, = 4 cycles


#endif


#if defined(DELAYMICROS_TWELVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t" // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
#elif defined(DELAYMICROS_TEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t" // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles each;
    "rjmp .+0" "\n\t"     // 2 cycles each;
    "rjmp .+0" "\n\t"     // 2 cycles each;
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
#elif defined(DELAYMICROS_EIGHT)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t" // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles each;
    "rjmp .+0" "\n\t"     // 2 cycles each;
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
#elif defined(DELAYMICROS_SEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t" // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop"      "\n\t"     // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
#else
  // the classic 4 cycle delay loop...
  // busy wait
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t" // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
#endif
  // return = 4 cycles
}


void init()
{
  // this is called by main() before setup() - configures the on-chip peripherals.


  #if ((defined(DB_28_PINS) || defined(DB_32_pins)) && !defined(NO_PIN_PD0_BUG))
    // PD0 does not exist on these parts - VDDIO2 took it's (physical) spot.
    // but due to a silicon bug, the input buffer is on, but it's input is floating. Per errata, we are supposed to turn it off.
    PORTD.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  #endif


  init_clock();

  init_timers();
  #if defined(ADC0)
    init_ADC0();
  #endif

  #ifndef MILLIS_USE_TIMERNONE
    init_millis();
  #endif
  // Finally, we enable interrupts; immediately following this, setup will be ve called.
  sei();
}



void stop_millis()
{ // Disable the interrupt:
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("stop_millis() called, but millis is disabled from tools menu!");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.INTCTRL&=0xFE;
    #elif defined(MILLIS_USE_TIMERRTC)
      RTC.INTCTRL&=0xFE;
      RTC.CTRLA&=0xFE;
    #else
      _timer->INTCTRL &= ~TCB_CAPT_bm;
    #endif
  #endif
}

void restart_millis()
{
  // Call this to restart millis after it has been stopped and/or millis timer has been molested by other routines.
  // This resets key registers to their expected states.
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("restart_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.CTRLA = 0x00;
      TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
      TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.CTRLA = 0x00;
      TCA1.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
      TCA1.SPLIT.HPER    = PWM_TIMER_PERIOD;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLA = 0x00;
      while(TCD0.STATUS & 0x01);
    #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1)|| defined(MILLIS_USE_TIMERB2)|| defined(MILLIS_USE_TIMERB3)|| defined(MILLIS_USE_TIMERB4)) //It's a type b timer
      _timer->CTRLB = 0;
    #endif
    init_millis();
  #endif
}

void init_millis()
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CMPBCLR = TIME_TRACKING_TIMER_PERIOD; //essentially, this is TOP
      TCD0.INTCTRL = 0x01;//enable interrupt
      TCD0.CTRLB = 0x00; //oneramp mode
      TCD0.CTRLC = 0x80;
      TCD0.CTRLA = TIMERD0_PRESCALER|0x01; //set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
      while(RTC.STATUS); //if RTC is currently busy, spin until it's not.
      // to do: add support for RTC timer initialization
      RTC.PER = 0xFFFF;
    #ifdef MILLIS_USE_TIMERRTC_XTAL
      _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x03);
      RTC.CLKSEL = 2; //external crystal
    #else
      _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, 0x02);
      //RTC.CLKSEL = 0; this is the power on value
    #endif
      RTC.INTCTRL = 0x01; //enable overflow interrupt
      RTC.CTRLA = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|RTC_PRESCALER_DIV32_gc);//fire it up, prescale by 32.
    #else //It's a type b timer
      _timer->CCMP = TIME_TRACKING_TIMER_PERIOD;
      // Enable timer interrupt, but clear the rest of register
      _timer->INTCTRL = TCB_CAPT_bm;
      // Clear timer mode (since it will have been set as PWM by init())
      _timer->CTRLB = 0;
      // CLK_PER/1 is 0b00, . CLK_PER/2 is 0b01, so bitwise OR of valid divider with enable works
      _timer->CTRLA = TIME_TRACKING_TIMER_DIVIDER|TCB_ENABLE_bm;  // Keep this last before enabling interrupts to ensure tracking as accurate as possible
    #endif
  #endif
}


void set_millis(uint32_t newmillis)
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
    GPR.GPR0 = newmillis; // keeps the compiler from warning about unused parameter, it's a compile error if this is reachable anyway.
  #else
    #if defined(MILLIS_USE_TIMERRTC)
      //timer_overflow_count = newmillis >> 16;
      // millis = 61/64(timer_overflow_count << 16 + RTC.CNT)
      uint16_t temp = (newmillis%61) << 6;
      newmillis = (newmillis/61) << 6;
      temp = temp/61;
      newmillis += temp;
      timer_overflow_count = newmillis >> 16;
      while(RTC.STATUS&RTC_CNTBUSY_bm); //wait if RTC busy
      RTC.CNT = newmillis&0xFFFF;
    #else
      timer_millis = newmillis;
    #endif
  #endif
}




/********************************* ADC ****************************************/
#if defined(ADC0)
  void __attribute__((weak)) init_ADC0() {
    #if F_CPU >= 48000000
      ADC0.CTRLC = ADC_PRESC_DIV48_gc; //1 @ 48 MHz
    #elif F_CPU >  40000000
      ADC0.CTRLC = ADC_PRESC_DIV32_gc; //1.25 @ 40 to 1.5 @ 48
    #elif F_CPU >= 36000000
      ADC0.CTRLC = ADC_PRESC_DIV28_gc; //1.286 @ 36, 1.429 @ 40 MHz
    #elif F_CPU >  28000000
      ADC0.CTRLC = ADC_PRESC_DIV24_gc; //1.33 @ 32 MHz, 1.
    #elif F_CPU >= 24000000
      ADC0.CTRLC = ADC_PRESC_DIV20_gc; //1.2 @ 24, 1.25 @ 25, 1.4 @ 28  MHz
    #elif F_CPU >= 20000000
      ADC0.CTRLC = ADC_PRESC_DIV16_gc; //1.25 @ 20 MHz
    #elif F_CPU >  12000000
      ADC0.CTRLC = ADC_PRESC_DIV12_gc; //1.333 @ 16 MHz
    #elif F_CPU >= 8000000
      ADC0.CTRLC = ADC_PRESC_DIV8_gc;  //1-1.5 MHz
    #elif F_CPU >= 4000000
      ADC0.CTRLC = ADC_PRESC_DIV4_gc;  //1 MHz
    #else  // 1 MHz / 2 = 500 kHz - the lowest setting
      ADC0.CTRLC = ADC_PRESC_DIV2_gc;
    #endif
    ADC0.SAMPCTRL = 14; //16 ADC clock sampling time - should be about the same amount of *time* as originally?
    // This is WAY conservative! We could drop it down...
    ADC0.CTRLD = ADC_INITDLY_DLY64_gc; //VREF can take 50uS to become ready, and we're running the ADC clock
    // at around 1 MHz, so we want 64 ADC clocks when we start up a new reference so we don't get bad readings at first
    /* Enable ADC */
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
    //start at 10 bit for compatibuility with existing code.

    #if (defined(__AVR_DA__) && (!defined(NO_ADC_WORKAROUND)))
      // That may become defined when DA-series silicon is available with the fix
      ADC0.MUXPOS = 0x40;
      ADC0.COMMAND = 0x01;
      ADC0.COMMAND = 0x02;
    #endif
    analogReference(VDD);
    DACReference(VDD);
  }
#endif

/******************************** CLOCK ***************************************/
// This section includes accessory functions for the system clock configuration
// currently, this includes only on ones used to generate a "blink code" to
// assist in debugging issues related to an external clock source.


void  __attribute__((weak)) init_clock() {

  #if CLOCK_SOURCE == 0
    /*
     * internal can be cranked up to 32 Mhz by just extending the prior pattern from 24 to 28 and 32.
     *  F_CPU    CLKCTRL_FREQSEL
     *  1 MHz    0x0
     *  2 MHz    0x1
     *  3 MHz    0x2
     *  4 MHz    0x3  - default at power on reset
     * Reserved  0x4
     *  8 MHz    0x5
     * 12 MHz    0x6
     * 16 MHz    0x7
     * 20 MHz    0x8
     * 24 MHz    0x9
     * 28 MHz    0xA  - undocumented
     * 32 MHz    0xB  - undocumented
     * 20 MHz    0xC  - repeat
     * 24 MHz    0xD  - repeat
     * 28 MHz    0xE  - repeat
     * 32 MHz    0xF  - repeat
     */
    /* Some speeds not otherwise possible can be generated with the internal oscillator by prescaling
     * This is done for 5 and 10 because those were common speeds to run 0 and 1-series parts at.
     * It was not done for 14, 7, 6, nor any of the other stupid frequencies that can be generated this way.
     */
    /* Notice: Give us a FREQing break! In ATpack version 1.9.103, they renamed another field...
     * FREQSEL was changed to FRQSEL. It is unclear to me why it was necessary to make a breaking change
     * impacting nearly all existing code that used the internal oscillator on these parts but it must
     * have been very important to eliminate that E.
     * We here use the old spelling, and breathe a sigh of relief that we didn't use the group code names.
     * Our supplied core_devices.h will work around this: if the the old spelling of the group masks and
     * is not defined, we define it to the new spelling (and if neither is defined, then we are attempting
     * to compile for something that isn't a supported part so failure is expected). core_devices.h doesn't
     * patch up the group codes, and those should not be used until a toolchain package with the new names
     * is ready for DxCore. This will happen only after ship dates are available for DD-series parts with
     * less than 64k of flash major changes/fixes are put in that impact available parts.
     */
     /* 10/13: made this more efficient. - we were doing  a read mofiofy erite on protected rteg for no reason!
      * This code runs right aftter reset, and we normally assume that registers are in there reset state.
      * Also code defensively around renaming of bitfield in some ATPack versions.
      */
    #if (F_CPU == 32000000)
      /* Overclocked - generally reliable at room temperature*/
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0B << 2));
    #elif (F_CPU == 28000000)
      /* Overclocked - generally quite reliable at room temperature */
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2));
    #elif (F_CPU == 24000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x09 << 2));
    #elif (F_CPU == 20000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));
    #elif (F_CPU == 16000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x07 << 2));
    #elif (F_CPU == 12000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x06 << 2)); /* should it be 24MHz prescaled by 2? */
    #elif (F_CPU == 8000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x05 << 2)); /* Should it be 16MHz prescaled by 2? */
    #elif (F_CPU == 4000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x03 << 2)); /* Should it be 16MHz prescaled by 4? */
    #elif (F_CPU == 3000000)
      #warning "3 MHz, currently selected for F_CPU, is not supported by this core and has not been tested. Expect timekeeping problems."
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x02 << 2)); /* There's like, no support for this anywhere in the core!  */
    #elif (F_CPU == 2000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x01 << 2));
    #elif (F_CPU == 1000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x00 << 2));
      /* Prescaled clock options */
    #elif (F_CPU == 10000000) /* 10 MHz = 20 MHz prescaled by 2 */
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));
    #elif (F_CPU == 5000000)  /* 5 MHz = 20 MHz prescaled by 4 */
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));
    #else
      #error "F_CPU defined as an unsupported value"
    #endif
  #elif (CLOCK_SOURCE == 1 || CLOCK_SOURCE == 2)
    /* For this, we don't really care what speed it is at - we will run at crystal frequency, and **the user must pick the right one**
     * It is impractical to determine what we're running at at runtime, and the user should really knoe the basic parameters of the
     * crystal - it's usually printed on the damned thing. We don't prescale from crystals, and won't unless someone gives a damned
     * convincing reason why that feature is important.
     * Crystals in the relevant frequency range are readily available.
     * So are oscillators, though for reasons I don't understand, cost an arm and a leg. The DA-series doesn't support crystals*/
    #if !defined(CLKCTRL_XOSCHFCTRLA)
      // it's an AVR DA-series
      #if (CLOCK_SOURCE == 1)
        #error "AVR DA-series selected, but crystal as clock source specified. DA-series parts only support internal oscillator or external clock."
      #else
        //external clock
        uint8_t i = 255;
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc); //
        while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
          i--;
          if(i == 0) blinkCode(3);
          // in my tests, it only took a couple of passes through this loop to pick up the external clock, so at this point we can be pretty certain that it's not coming....
        }
      #endif
    #else
      // it's a DB; likely the DD will be the same as well
      // turn on clock failure detection - it'll just go to the blink code error, but the alternative would be hanging with no indication of why!
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLC, CLKCTRL_CFDSRC_CLKMAIN_gc | CLKCTRL_CFDEN_bm);
      _PROTECTED_WRITE(CLKCTRL_MCLKINTCTRL, CLKCTRL_CFD_bm);
      #if (CLOCK_SOURCE == 2)
        //external clock
        _PROTECTED_WRITE(CLKCTRL_XOSCHFCTRLA, (CLKCTRL_SELHF_EXTCLOCK_gc | CLKCTRL_ENABLE_bm));
      #else
        // external crystal
        #ifndef USE_XTAL_DRIVE
          // In a quick test, with terrible layout (strip-board), I could run a 16 MHz crystal with any of these options!
          // it was an 18 pf crystal with parasitic capacitance of stripboard as loading. User can force it to desired value
          // but nobody is likely to care. Lower speed settings use less power, I *think*.
          #if     (F_CPU > 24000000)
            #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_32M_gc
          #elif   (F_CPU > 16000000)
            #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_24M_gc
          #elif   (F_CPU >  8000000)
            #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_16M_gc
          #else
            #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_8M_gc
          #endif
        #endif
        #ifndef USE_CSUTHF
          #define USE_CSUTHF CLKCTRL_CSUTHF_256_gc
        #endif
      _PROTECTED_WRITE(CLKCTRL_XOSCHFCTRLA, (USE_CSUTHF | USE_XTAL_DRIVE | CLKCTRL_SELHF_XTAL_gc | CLKCTRL_ENABLE_bm));
      /*Formerly CLKCTRL_SELHF_CRYSTAL_gc, but they changed it 6 months after they started shipping DB's*/
      #endif
    #endif
    uint16_t i = 4096;
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
      i--;
      if(i == 0) onClockTimeout();
      // crystals can take a lot longer to reach stability.
    }
  #else
    #error "CLOCK_SOURCE was not 0 (internal), 1 (crystal) or 2 (ext. clock); you must specify a valid clock source with F_CPU and CLOCK_SOURCE."
  #endif
}


/********************************* CLOCK FAILURE HANDLING **************************************/
/*                                                                                             *
 * These are used for blink codes which indicate issues that would prevent meaningful startup  *
 * While we could leave it running at 4 MHz and run the sketch, I think at that point it is    *
 * more useful to abort startup than to leave the user wondering why the sketch is running     *
 * otherwise correctly at an unexpected speed. "Why is my sketch running really slow?" on      *
 * the classic AVRs is a very common complaint due to people forgetting to turn off the        *
 * CLKDIV8 there. Instead, if the code doesn't work at all with some alternative clock         *
 * uploading other code is not impeded (the chip is not softbricked) and the user will         *
 * hopefully be able to deduce that there is a problem with the clock, even without an LED on  *
 * PA7 - however, if an LED is connected, a blink code will be shown. The most distinctive     *
 * aspect of them is that, having an odd number of "change" operations, the pattern will be    *
 * shown, and then the inverted pattern will be shown, This is unlikely to be similar to       *
 * anything the user might upload.                                                             */


#if (CLOCK_SOURCE == 1 || CLOCK_SOURCE == 2)
  void _asmDelay(uint16_t us) {
    __asm__ __volatile__ (
      "1: sbiw %0, 1" "\n\t" // 2 cycles
      "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
    );
  }
  // These two functions need only exist if not using internal clock source.
  void _blinkCode(uint8_t blinkcount) {
    openDrainFast(LED_BUILTIN, LOW);
    while(1) {
      for (byte i = 3; i != 0; i--){
        digitalWriteFast(LED_BUILTIN, CHANGE);
        for (byte j = 20; j != 0; j--) {
          _asmDelay(50000);
      }
      for (byte i = blinkcount; i != 0; i--){
        digitalWriteFast(LED_BUILTIN, CHANGE);
        _asmDelay(50000);
        digitalWriteFast(LED_BUILTIN, CHANGE);
        for (byte j = 9; j != 0; j--) {
          _asmDelay(50000);
        }
      }
    }
  }
}

void __attribute__((weak)) onClockTimeout() {
  _blinkCode(3);
}
  // If this gets called, then we tried - and failed - to configure an external clock source!
  // running at 4 MHz due to missing clock, so 1 pass through loop = 1 us



  #ifdef CLKCTRL_CFD_vect
    ISR(CLKCTRL_CFD_vect){
      onClockFailure();
    }
    void __attribute__((weak)) onClockFailure() {
       _blinkCode(4);
    }
  #endif
#endif




/******************************** PWM TIMERS ***************************************/

uint8_t PeripheralControl = 0xFF;

void init_timers() {


  init_TCA0();
  #if (defined(TCA1))
    PORTMUX.TCAROUTEA = TCA0_PINS | TCA1_PINS;
    init_TCA1();
  #else
    PORTMUX.TCAROUTEA = TCA0_PINS;
  #endif

  init_TCBs();
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM) && !defined(MILLIS_USE_TIMERD0))
    init_TCD0();
  #endif
}


 /* TIMER INITIALIZATION CODE START
  *
  * Basic PWM Timer initialization
  * Type A timer for millis and/or PWM Type B timer for PWM (not millis)
  *
  * 8-bit PWM with PER/TOP = 0xFD / 254 for a 255 clock period (not 256)
  * Therre are two important reasons that we want a 255 clock period not 256 as
  * one might expect. If TCA is used for millis, we find that 255 is a magical
  * number that makes a bunch of math work out exactly with integers.
  * This prevents the rounding errors from manifesting in millis() return values.
  *
  * For PWM, it is also critical. There are 256 possible inputs to analogWrite()
  * which are valid. But if we count to 255, then the pin would have 257 states
  * There is always a compare value that can be chosen that results in no PWM
  * (which end depends on the timer and implementation details), giving constant
  * HIGH or LOW. However, the opposite extreme corresponds to 1 cycle away from
  * constant output.
  * You cannot smoothly map 256 inputs to 256 outputs. Worse still, most schemes
  * that do not account for this have the glitch at the bottom, meaning that
  * analogWrite(pin, 0) sets the pin low, while analogWrite(pin, 0) gives not
  * 1/256th duty cycle but 2/256ths, and any scheme will put it at one of the ends
  * unless it explicitly accounts for this. And the ends are of course the worst place
  * for such artifacts. Few notice or care whether an LED has a duty cycle of 128/256ths
  * or 127/256ths. But they absolutely know the difference between 1/256th and 2/256ths.
  * Since things may be active LOW or active HIGH, the same issue applies to
  * the top of the range.
  * Thus, by counting to 254, we have 255 valid compare values and 256 valid
  * analogWrite() values without any discontinuities, and the millis math works
  * out better.
  *
  * PWM frequency vs clock frequency
  * for TCA and TCB timers as configured by the core.
  * The "target" frequency is 1kHz. This is the same as the standard boards
  * and is a sensible default: It's fast enough that most things don't have
  * noticeable flicker/etc, but you can still PWM the gate of a MOSFET at
  * that speed and get away with it for small MOSFETs with low gate charge.
  * As the frequency gets much faster, you enter the teritory where the pin
  * can't drive the gate hard enough for the fet to spend most of it's time
  * either ON or OFF, switching losses increase, the MOSFET gets hot, and
  * in some cases, failure of the FET can occur at a much lower load than
  * would be expected.
  *
  * The canonical value is 980 Hz on 16 MHz Uno: 16,000,000/ (255 * 64)
  * or 1000/1020; counting to 255 gives 256 clocks and a slightly slower
  * 976 Hz (1000/1024). Generally, it is F_CPU/(cycle_length * prescale).
  * equivalent to F_CPU/((TOP + 1) * prescale).
  *
  *  F_CPU    Prescale   F_PWM
  * 48 MHz      256     735 Hz
  * 44 MHz      256     674 Hz
  * 40 MHz      256     613 Hz
  * 36 MHz      256     551 Hz
  * 32 MHz      256     490 Hz
  * 28 MHz      256     429 Hz  // Better this than 1716 Hz
  * 25 MHz       64    1532 Hz  // Ewwww! It's this or 375 though!
  * 24 MHz       64    1471 Hz  // Higher than I'd like...
  * 20 MHz       64    1225 Hz
  * 16 MHz       64     980 Hz
  * 12 MHz       64     735 Hz
  * 10 MHz       64     613 Hz
  *  8 MHz       64     490 Hz
  *  5 MHz       16    1225 Hz
  *  4 MHz       16     980 Hz
  *  1 MHz        8     490 Hz
  *
  * It sure makes you wish there were a 1/128 prescale option doesn't it?
  */


void __attribute__((weak)) init_TCA0() {
  /* TCA0_PINS from pins_arduino.h */
  /* Enable Split Mode */
  TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

  //Only 1 WGM so no need to specifically set up.

  /* Period setting, 8-bit register in SPLIT mode */
  TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
  TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;

  /* No need to set the compare values. PWM is not enabled on startup and
   * analogWrite() sets this. This saves 12 instruction words because each
   * assignment becomes a 2-word STS instruction. A smarter compiler that
   * was better able to detect when STD would safe flash/time could do these
   * more efficiently. But they don't need to be done so it's academic.
   */
  /*
  TCA0.SPLIT.LCMP0 = 0;
  TCA0.SPLIT.HCMP0 = 0;
  TCA0.SPLIT.LCMP1 = 0;
  TCA0.SPLIT.HCMP1 = 0;
  TCA0.SPLIT.LCMP2 = 0;
  TCA0.SPLIT.HCMP2 = 0;
  */

  /* Use prescale appropriate for system clock speed
   * Detect conflict between wiring.c and timers.h if we spot them, as that indicates
   * a defect in the core and would result in extremely bad behavior*/

  #if (F_CPU > 30000000) //use 256 divider when clocked over 25 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) //use 64 divider unless it's 5 MHz or under
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 64)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 1000000) // anything above 1 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 16)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
  #else
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 8)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =   (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
  #endif
}

#if defined(TCA1)
void __attribute__((weak)) init_TCA1() {

  /* Enable Split Mode */
  TCA1.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

  /* Period setting, 8-bit register in SPLIT mode */
  TCA1.SPLIT.LPER    = PWM_TIMER_PERIOD;
  TCA1.SPLIT.HPER    = PWM_TIMER_PERIOD;


  #if (F_CPU > 30000000) //use 256 divider when clocked over 25 MHz
    #if defined(MILLIS_USE_TIMERA1) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA1 to different divider"
    #endif
    TCA1.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) //use 64 divider unless it's 5 MHz or under
    #if defined(MILLIS_USE_TIMERA1) && (TIME_TRACKING_TIMER_DIVIDER != 64)
      #error "wiring.c and timers.h want to set millis timer TCA1 to different divider"
    #endif
    TCA1.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 1000000) // anything above 1 MHz
    #if defined(MILLIS_USE_TIMERA1) && (TIME_TRACKING_TIMER_DIVIDER != 16)
      #error "wiring.c and timers.h want to set millis timer TCA1 to different divider"
    #endif
    TCA1.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
  #else
    #if defined(MILLIS_USE_TIMERA1) && (TIME_TRACKING_TIMER_DIVIDER != 8)
      #error "wiring.c and timers.h want to set millis timer TCA1 to different divider"
    #endif
    TCA1.SPLIT.CTRLA   =   (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
  #endif
}
#endif

void __attribute__((weak)) init_TCBs() {

  /*    TYPE B TIMERS  */
  /* Set up routing (defined in pins_arduino.h)
   * This is a heinously ugly use of the preprocessor, but all alternatives
   * are even uglier and messier. Notice how there's no semicolon until after
   * the preprocessor conditionals.
   */

  PORTMUX.TCBROUTEA = 0
  #if defined(TCB0)
                        | TCB0_PINS
  #endif
  #if defined(TCB1)
                        | TCB1_PINS
  #endif
  #if defined(TCB2)
                        | TCB2_PINS
  #endif
  #if defined(TCB3)
                        | TCB3_PINS
  #endif
  #if defined(TCB4)
                        | TCB4_PINS
  #endif
        ;

    // Start with TCB0
  TCB_t *timer_B = (TCB_t *)&TCB0;

  // Find end timer
  #if defined(TCB4)
    TCB_t *timer_B_end = (TCB_t *) &TCB4;
  #elif defined(TCB3)
    TCB_t *timer_B_end = (TCB_t *) &TCB3;
  #elif defined(TCB2)
    TCB_t *timer_B_end = (TCB_t *) &TCB2;
  #elif defined(TCB1)
    TCB_t *timer_B_end = (TCB_t *) &TCB1;
  #else
    TCB_t *timer_B_end = (TCB_t *) &TCB0;
  #endif

  // Timer B Setup loop for TCB[0:end]
  do
  {
    // 8 bit PWM mode, but do not enable output yet, will do in analogWrite()
    timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

    // Assign 8-bit period
    timer_B->CCMPL = PWM_TIMER_PERIOD;

    // default duty 50%, set when output enabled
    // without this, some things that recklessly use TCBs without carefully
    // ensuring correct CCMP register settings at initialization can break.
    timer_B->CCMPH = PWM_TIMER_COMPARE;

    // Use TCA clock (250kHz, +/- 50%) and enable
    // (sync update commented out, might try to synchronize later

    timer_B->CTRLA = (TCB_CLKSEL_TCA0_gc)
                     //|(TCB_SYNCUPD_bm)
                     | (TCB_ENABLE_bm);

    // Increment pointer to next TCB instance
    timer_B++;

    // Stop when pointing to TCB3
  } while (timer_B <= timer_B_end);
}

#if (defined(TCD0) && defined(USE_TIMERD0_PWM) && !defined(MILLIS_USE_TIMERD0))
/* Configure TCD0 for PWM in the non-millis case; currently we do not support
 * TCD0 driven millis on the Dx-series, but we may have to for the DD-series
 * because, particularly in low pincounts, they are highly timer-constrained.
 * With just 1 TCA, 2 TCBs and TCD0 we find ourselves in the same boat as the
 * tinyAVRs. With the same timer layout as the "golden" 1-series, we shouldn't
 * be more constrained on this much fancier part. There, we *default* TCD0
 * because it's the least "useful" to take over.
 * TCD0 is a strange timer - most registers can only be changed with it disabled
 * and many require protected writes (ie, it is meant for critical operations
 * where miswrites have catastrophic consequences). Wierder still, because it
 * is async (and may be clocked faster than the CPU) the current count isn't
 * available directly. You have to do a software capture, wait for a few clocks
 * for sync (I think 2-3 synchronizer clocks there, 2-3 system clocks back).
 * and then you can read that value. See the millis implementations
 */
void __attribute__((weak)) init_TCD0() {
  TCD0.CMPBCLR  = TIMERD0_TOP_SETTING;    // From timers.h - is equal to (255 * 2^n)-1 where n is an integer value. This lets us lower
                                          // PWM frequency, or hold it constant and lower the sync prescale, reducing sync delays.
  TCD0.CMPACLR  = 0x0FFF;                 // Match with CMPBCLR clears all outputs. This just needs to be higher than it will count to.
  TCD0.CTRLC    = 0x80;                   // WOD outputs PWM B, WOC outputs PWM A
  TCD0.CTRLB    = TCD_WGMODE_ONERAMP_gc;  // One ramp or dual slope are the only options that are viable to reproduce classic behavior.
                                          // and the latter is incompatible with using it as millis timer, and we want to share as much
                                          // code as we can between the TCD0 and non-TCD0 millis.
  TCD0.CTRLA    = TIMERD0_CLOCK_SETTING | TCD_ENABLE_bm;  // See timers.h for determination of clock settings.
}
#endif
