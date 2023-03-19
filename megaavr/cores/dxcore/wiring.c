/*
  wiring.c - Partial implementation of the Wiring API
  Originally part of Arduino - http://www.arduino.cc/
  Copyright (c) 2005-2006 David A. Mellis


  Copyright (c) 2018-2021 Spence Konde
  This has been ported to modern AVRs (Arduino team did that)
  Almost every part of it has since been rewritten for
  megaTinyCore and DxCore. This is the DxCore version, and is
  part of DxCore.

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

void init_timers();

#ifndef F_CPU
  #error "F_CPU not defined. F_CPU must always be defined as the clock frequency in Hz"
#endif
#ifndef CLOCK_SOURCE
  #error "CLOCK_SOURCE not defined. Must be 0 for internal, 1 for crystal, or 2 for external clock"
#endif

#if (defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA1))
  #define MILLIS_USE_TCA
#endif

#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
  #define MILLIS_USE_TCB
#endif

#if (defined(MILLIS_USE_TIMERD0))
  #define MILLIS_USE_TCD
#endif




#ifndef MILLIS_USE_TIMERNONE /* If millis is enabled, we need to provide it millis() and micros() */

  // volatile uint16_t microseconds_per_timer_overflow;
  // volatile uint16_t microseconds_per_timer_tick;
  // overflow count is tracked for all timer options, even the RTC
  struct sTimer {
    uint8_t            intClear;
    volatile uint8_t  *intStatusReg;
  };

  #if defined(MILLIS_USE_TIMERRTC)
    // this is done for us in timers.h
    // #define MILLIS_VECTOR (RTC_CNT_vect)
    const struct sTimer _timerS = {RTC_OVF_bm, &RTC.INTCTRL};
  #else
    #if defined (MILLIS_USE_TIMERA0)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCA0_HUNF_vect)
      const struct sTimer _timerS = {TCA_SPLIT_HUNF_bm,  &TCA0.SPLIT.INTFLAGS};

    #elif defined (MILLIS_USE_TIMERA1)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCA1_HUNF_vect)
      const struct sTimer _timerS = {TCA_SPLIT_HUNF_bm,  &TCA1.SPLIT.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERB0)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCB0_INT_vect)
      static volatile TCB_t *_timer = &TCB0;
      const struct sTimer _timerS = {TCB_CAPT_bm, &TCB0.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERB1)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCB1_INT_vect)
      static volatile TCB_t *_timer = &TCB1;
      const struct sTimer _timerS = {TCB_CAPT_bm, &TCB1.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERB2)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCB2_INT_vect)
      static volatile TCB_t *_timer = &TCB2;
      const struct sTimer _timerS = {TCB_CAPT_bm, &TCB2.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERB3)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCB3_INT_vect)
      static volatile TCB_t *_timer = &TCB3;
      const struct sTimer _timerS = {TCB_CAPT_bm, &TCB3.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERB4)
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCB4_INT_vect)
      static volatile TCB_t *_timer = &TCB4;
      const struct sTimer _timerS = {TCB_CAPT_bm, &TCB4.INTFLAGS};

    #elif defined(MILLIS_USE_TIMERD0)
      /*
      // this is done for us in timers.h
      // #define MILLIS_VECTOR (TCD0_OVF_vect)
      const struct sTimer _timerS = {TCD_OVF_bm, &TCD0.INTFLAGS};
      */
      #error "TCD0 is not a supported timing source on the Dx-series; the greater number of possible configurations of the clock make it even more unwieldy. "
    #else
      #error "No millis timer selected, but not disabled - can't happen!".
    #endif  /* defined(MILLIS_USE_TIMER__) */
  #endif  /* defined(MILLIS_USE_TIMERRTC) */

  #define FRACT_MAX (1000)
  #define FRACT_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)%1000)
  #define MILLIS_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/1000)

  struct sTimeMillis {
    #if defined(MILLIS_USE_TCB)     // Now TCB as millis source does not need fraction
      volatile uint32_t timer_millis;   // That's all we need to track here

    #elif defined(MILLIS_USE_TIMERRTC)  // RTC
      volatile uint16_t timer_overflow_count;
    #else                               // TCAx or TCD0
      volatile uint16_t timer_fract;
      volatile uint32_t timer_millis;
      volatile uint32_t timer_overflow_count;

    #endif
  } timingStruct;

  // Now for the ISRs. This gets a little bit more interesting now...
  #if defined (MILLIS_USE_TIMERRTC)
    ISR(MILLIS_VECTOR) {
      // if RTC is used as timer, we only increment the overflow count
      // Overflow count isn't used for TCB's
      if (RTC.INTFLAGS & RTC_OVF_bm) {
        timingStruct.timer_overflow_count++;
      }
      RTC.INTFLAGS = RTC_OVF_bm | RTC_CMP_bm; // clear flag
    }
  #else
    ISR(MILLIS_VECTOR, ISR_NAKED) {
      __asm__ __volatile__(
      "push       r30"          "\n\t" // First we make room for the pointer to timingStruct by pushing the Z registers
      "push       r31"          "\n\t" //
      ::);
    #if defined(MILLIS_USE_TCB)
      __asm__ __volatile__(
      "push       r24"            "\n\t" // we only use two register other than the pointer
      "in         r24,     0x3F"  "\n\t" // Need to save SREG too
      "push       r24"            "\n\t" // and push the SREG value  - 7 clocks here + the 5-6 to enter the ISR depending on flash and sketch size, 12-13 total
      "ld         r24,        Z"  "\n\t" // Z points to LSB of timer_millis, load the LSB
    #if (F_CPU > 2000000)                // if it's 1 or 2 MHz, millis timer overflows every 2ms, intentionally sacrificing resolution for reduced time spent in ISR
      "subi       r24,     0xFF"  "\n\t" // sub immediate 0xFF is the same as to add 1. (There is no add immediate instruction, except add immediate to word)
    #else
      "subi       r24,     0xFE"  "\n\t" // sub immediate 0xFE is the same as to add 2
    #endif
      "st           Z,      r24"  "\n\t" // Store incremented value back to Z
      "ldd        r24,      Z+1"  "\n\t" // now load the next higher byte
      "sbci       r24,     0xFF"  "\n\t" // because this is sbci, it treats carry bit like subtraction, and unless we did just roll over with the last byte,
      "std        Z+1,      r24"  "\n\t" // carry will be cleared. Thus, sbci 0xFF after a subi pressed into service to add, is the same as adc r1 after an add
      "ldd        r24,      Z+2"  "\n\t" // which is what we want.
      "sbci       r24,     0xFF"  "\n\t" // This gets repeated...
      "std        Z+2,      r24"  "\n\t" //
      "ldd        r24,      Z+3"  "\n\t" //
      "sbci       r24,     0xFF"  "\n\t" //
      "std        Z+3,      r24"  "\n\t" // ... until all 4 bytes were handled, at 4 clocks and 3 words per byte -> 16 clocks
      "ldi        r24, %[CLRFL]"  "\n\t" // This is the TCB interrupt clear bitmap
      "sts   %[PTCLR],      r24"  "\n\t" // write to Timer interrupt status register to clear flag. 2 clocks for sts
      "pop        r24"            "\n\t" // pop r24 to get the old SREG value - 2 clock
      "out       0x3F,      r24"  "\n\t" // restore SREG - 1 clock
      "pop        r24"            "\n\t"
      "pop        r31"            "\n\t"
      "pop        r30"            "\n\t" // 6 more clocks popping registers in reverse order.
      "reti"                      "\n\t" // and 4 clocks for reti - total 12/13 + 16 + 2 + 1 + 6 + 4 = 41/42 clocks total, and 7+16+3+6 = 32 words, vs 60-61 clocks and 40 words
      :: "z" (&timingStruct.timer_millis),   // we are changing the value of "Z", so to be strictly correct, this must be declared input output - though in this case it doesn't matter
         [CLRFL] "M" (_timerS.intClear),
         [PTCLR] "m" (*_timerS.intStatusReg)
      ); // grrr, sublime highlights this as invalid syntax because it gets confused by the ifdef's and odd syntax on inline asm
      /* ISR in C:
        ISR (TCBx_INT_vect) {       // x depends on user configuration
          #if (F_CPU > 2000000)
            timer_millis += 1;
          #else
            timer_millis += 2;
          #endif
          _timer->INTFLAGS = TCB_CAPT_bm;   // reset Interrupt flag of TCBx
        }
      */
    #else // TCA0 or TCD0, also naked
      __asm__ __volatile__(
      // ISR prologue (overall 9 words / 9 clocks):
      "push       r24"            "\n\t" // we use three more registers other than the pointer
      "in         r24,     0x3F"  "\n\t" // Need to save SREG too
      "push       r24"            "\n\t" // and push the SREG value
      "push       r25"            "\n\t" // second byte
      "push       r23"            "\n\t" // third byte
      // timer_fract handling (13 words / 15/16 clocks):
      "ldi        r23, %[MIINC]"  "\n\t" // load MILLIS_INC. (part of timer_millis handling)
      "ld         r24,        Z"  "\n\t" // lo8(timingStruct.timer_fract).
      "ldd        r25,      Z+1"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRINC]"  "\n\t" // use (0xFFFF - FRACT_INC) and use the lower and higher byte to add by subtraction
      "sbci       r25,%[HFRINC]"  "\n\t" // can't use adiw since FRACT_INC might be >63
      "st         Z,        r24"  "\n\t" // lo8(timingStruct.timer_fract)
      "std        Z+1,      r25"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRMAX]"  "\n\t" // subtract FRACT_MAX and see if it is lower
      "sbci       r25,%[HFRMAX]"  "\n\t" //
      "brlo               lower"  "\n\t" // skip next three instructions if it was lower
      "st         Z,        r24"  "\n\t" // Overwrite the just stored value with the decremented value
      "std        Z+1,      r25"  "\n\t" // seems counter-intuitive, but it requires less registers
      "subi       r23,     0xFF"  "\n\t" // increment the MILLIS_INC by one, if FRACT_MAX was reached
      "lower:"                    "\n\t" // land here if fract was lower then FRACT_MAX
      // timer_millis handling (13 words / 17 clocks):
      "ldd        r25,      Z+2"  "\n\t" // lo16.lo8(timingStruct.timer_millis)
      "add        r25,      r23"  "\n\t" // add r23 to r25. r23 depends on MILLIS_INC and if FRACT_MAX was reached
      "std        Z+2,      r25"  "\n\t" //
      "ldi        r24,     0x00"  "\n\t" // get a 0x00 to adc with. Problem: can't subi 0x00 without losing the carry
      "ldd        r25,      Z+3"  "\n\t" // lo16.hi8(timingStruct.timer_millis)
      "adc        r25,      r24"  "\n\t" //
      "std        Z+3,      r25"  "\n\t" //
      "ldd        r25,      Z+4"  "\n\t" // hi16.lo8(timingStruct.timer_millis)
      "adc        r25,      r24"  "\n\t" //
      "std        Z+4,      r25"  "\n\t" //
      "ldd        r25,      Z+5"  "\n\t" // hi16.hi8(timingStruct.timer_millis)
      "adc        r25,      r24"  "\n\t" //
      "std        Z+5,      r25"  "\n\t" //
      // timer_overflow_count handling (12 words / 16 clocks):
      "ldd        r25,      Z+6"  "\n\t" // lo16.lo8(timingStruct.timer_overflow_count)
      "subi       r25,     0xFF"  "\n\t" //
      "std        Z+6,      r25"  "\n\t" //
      "ldd        r25,      Z+7"  "\n\t" // lo16.hi8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+7,      r25"  "\n\t" //
      "ldd        r25,      Z+8"  "\n\t" // hi16.lo8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+8,      r25"  "\n\t" //
      "ldd        r25,      Z+9"  "\n\t" // hi16.hi8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+9,      r25"  "\n\t" //
      // timer interrupt flag reset handling (3 words / 3 clocks):
      "ldi        r24, %[CLRFL]"  "\n\t" // This is the TCx interrupt clear bitmap
      "sts   %[PTCLR],      r24"  "\n\t" // write to Timer interrupt status register to clear flag. 2 clocks for sts
      // ISR epilogue (8 words / 17/18 clocks):
      "pop        r23"            "\n\t"
      "pop        r25"            "\n\t"
      "pop        r24"            "\n\t" // pop r24 to get the old SREG value - 2 clock
      "out       0x3F,      r24"  "\n\t" // restore SREG - 1 clock
      "pop        r24"            "\n\t"
      "pop        r31"            "\n\t"
      "pop        r30"            "\n\t"
      "reti"                      "\n\t" // total 77 - 79 clocks total, and 58 words, vs 104-112 clocks and 84 words
      :: "z" (&timingStruct),            // we are changing the value of "Z", so to be strictly correct, this must be declared input output - though in this case it doesn't matter
        [LFRINC] "M" (((0x0000 - FRACT_INC)    & 0xFF)),
        [HFRINC] "M" (((0x0000 - FRACT_INC)>>8 & 0xFF)),
        [LFRMAX] "M" ((FRACT_MAX    & 0xFF)),
        [HFRMAX] "M" ((FRACT_MAX>>8 & 0xFF)),
        [MIINC]  "M" (MILLIS_INC),
        [CLRFL]  "M" (_timerS.intClear),
        [PTCLR]  "m" (*_timerS.intStatusReg)
      );

      /* ISR ASM logic written out in C:
        // timer_fract handling:
        uint8_t temp = MILLIS_INC;
        uint16_t f = timingStruct.timer_fract;
        f -= 0xFFFF;
        timingStruct.timer_fract = f;
        f -= FRACT_MAX;
        if (f > FRACT_MAX) {
          timingStruct.timer_fract = f;
          temp++;
        }
        // timer_millis handling:
        timingStruct.timer_millis += temp;
        // timer_overflow_count handling:
        timingStruct.timer_overflow_count -= 0xFFFFFFFF;
        // timer interrupt flag reset handling:
        *_timerS.intStatusReg = _timerS.intClear;
      */
      /* old ISR C code:
        uint32_t m = timingStruct.timer_millis;
        uint16_t f = timingStruct.timer_fract;
        m += MILLIS_INC;
        f += FRACT_INC;
        if (f >= FRACT_MAX) {

          f -= FRACT_MAX;
          m += 1;
        }
        timingStruct.timer_fract = f;
        timingStruct.timer_millis = m;
        timingStruct.timer_overflow_count++;
        *_timerS.intStatusReg = _timerS.intClear;
      */
    #endif /* defined(MILLIS_USE_TCB) */
    }
  #endif /* defined (MILLIS_USE_TIMERRTC)*/


  /*  Both millis and micros must take great care to prevent any kind of backward time travel.
   *
   * These values are unsigned, and should not decrease, except when they overflow. Hence when
   * we compare a value with what we recorded previously and find the new value to be lower, it
   * looks the same as it would 2^32 (4.2 billion) intervals in the future. Timeouts end prematurely
   * and similar undesired behaviors occur.
   *
   * There are three hazardous things we read here:
   * timer_millis, timer_overflow_count, and the timer count itself (TCxn.CNT).
   * The normal variables need only be read with interrupts disabled, in case of an
   * interrupt writing to it while we were reading it. AVRs are little-endian, so this would result
   * in the low byte being read before the overflow and the high byte after, and hence a value
   * higher than it should be for that call. Subsequent calls would return the right value.
   *
   * In the case of the timer value, it is more complicated.
   * Here, the hardware at first glance seems to protect us (see "reading 16-bit registers" in the
   * datasheet). But the register gets read in the interrupt, so we still need those disabled.
   * There is an additional risk though that we get a value from after the timer has overflowed
   * and since we disabled interrupts, the interrupt hasn't updated the overflow. We check the
   * interrupt flag, and if it's set, we check whether the timer value we read was near overflow
   * (the specifics vary by the timer - they're very different timers). If it isn't close to overflow
   * but the flag is set, we must have read it after the overflow, so we compensate for the missed
   * interrupt. If interrupts are disabled for long enough, this heuristic will be wrong, but in
   * that case it is the user's fault, as this limitation is widely known and documentedm, as well
   * as unavoidable. Failure to compensate looks like the inverse of the above case.
   *
   * (note that only micros reads the timer, and hence, only micros can experience backwards time
   * travel due to interrupts being left disabled for too long, millis will just stop increasing.
   *
   * Both of these cause severe breakage everywhere. The first type is simple to avoid, but if
   * missed can be more subtle, since it makes a big difference only if the byte where the read
   * was interrupted rolled over. The second type is more obvious, potentially happening on every timer
   * overflow, instead of just every 256th timer overflow, and when it does happen, anything waiting
   * for a specific number of microseconds to pass that gets that value will do so.
   * Though (see delay below) each incidence only short-circuits one ms of delay(), not the whole
   * thing.
   *
   * All time time travel except for glitchs from disabling millis for too long should no longer
   * be possible. If they are, that is a critical bug.
   */


  unsigned long millis() {
    // return timer_overflow_count; // for debugging timekeeping issues where these variables are out of scope from the sketch
    unsigned long m;
    // disable interrupts while we read timer_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer_millis)
    uint8_t oldSREG = SREG;
    cli();
    #if defined(MILLIS_USE_TIMERRTC)
      uint16_t rtccount = RTC.CNT;
      m = timingStruct.timer_overflow_count;
      if (RTC.INTFLAGS & RTC_OVF_bm) {
        /* There has just been an overflow that hasn't been accounted for by the interrupt. Check if the high bit of counter is set.
         * We just basically need to make sure that it didn't JUST roll over at the last couple of clocks. But this merthod is
         * implemented very efficiently (just an sbrs) so it is more efficient than other approaches. If user code is leaving
         * interrupts off nearly 30 seconds, they shouldn't be surprised. */
        if (!(rtccount & 0x8000)) m++;
      }
      SREG = oldSREG;
      m = (m << 16);
      m += rtccount;
      m = m - (m >> 5) + (m >> 7);
      /* the compiler is incorrigible - it cannot be convinced not to copy m twice, shifting one 7 times and the other 5 times
       * and wasting 35 clock cycles and several precious instruction words.
       * What you want is for it to make one copy of m, shift it right 5 places, subtract, then rightshift it 2 more.
       */
      /* Would this work?
      uint32_t temp;
      uint8_t cnt;
      __asm__ __volatile__ (
        "movw %A1, %A0     \n\t"
        "movw %C1, %C0     \n\t" //make our copy
        "ldi %2, 5"       "\n\t"
        "lsr %D1"         "\n\t"
        "ror %C1"         "\n\t"
        "ror %B1"         "\n\t"
        "ror %A1"         "\n\t"
        "dec %2"          "\n\t"
        "brne .-12"       "\n\t"
        "sub %A0, %A1"    "\n\t"
        "subc %B0, %B1"   "\n\t"
        "subc %C0, %C1"   "\n\t"
        "subc %D0, %D1"   "\n\t"
        "ldi %2, 2"       "\n\t"
        "lsr %D1"         "\n\t"
        "ror %C1"         "\n\t"
        "ror %B1"         "\n\t"
        "ror %A1"         "\n\t"
        "dec %2"          "\n\t"
        "brne .-12"       "\n\t"
        "add %A0, %A1"    "\n\t"
        "adc %B0, %B1"    "\n\t"
        "adc %C0, %C1"    "\n\t"
        "adc %D0, %D1"    "\n\t"
        : "+r" (m), "+r" (temp), "+d" (cnt)
        );
      */
    #else
      m = timingStruct.timer_millis;
      SREG = oldSREG;
    #endif
    return m;
  }


  #if !defined(MILLIS_USE_TIMERRTC)
    unsigned long micros() {
      uint32_t overflows, microseconds;
      #if (defined(MILLIS_USE_TCD) || defined(MILLIS_USE_TCB))
        uint16_t ticks;
      #else /* TCA */
        uint8_t ticks;
      #endif
      uint8_t flags;
      /* Save current state and disable interrupts */
      uint8_t oldSREG = SREG;
      cli(); /* INTERRUPTS OFF */
      #if defined(MILLIS_USE_TIMERA0)
        ticks = TCA0.SPLIT.HCNT;
        flags = TCA0.SPLIT.INTFLAGS;

      #elif defined(MILLIS_USE_TIMERA1)
        ticks = TCA1.SPLIT.HCNT;
        flags = TCA1.SPLIT.INTFLAGS;
        /*
      #elif defined(MILLIS_USE_TCD)
        TCD0.CTRLE = TCD_SCAPTUREA_bm;
        while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for sync - should be only one iteration of this loop
        flags = TCD0.INTFLAGS;
        ticks = TCD0.CAPTUREA;
        */
      #else /* = defined(MILLIS_USE_TCB) */
        ticks = _timer->CNT;
        flags = _timer->INTFLAGS;
      #endif // end getting ticks
      /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
       * ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
       * so we shouldn't increment overflows, or interrupts are disabled and micros isn't expected to work so it
       * doesn't matter.
       * Get current number of overflows and timer count */
      #if defined(MILLIS_USE_TCB)
        overflows = timingStruct.timer_millis;
      #else
        overflows = timingStruct.timer_overflow_count;
      #endif
      /* Turn interrupts back on, assuming they were on when micros was called. */
      SREG = oldSREG; /* INTERRUPTS ON */
      /*
      #if defined(MILLIS_USE_TIMERD0)
        if ((flags & TCD_OVF_bm) && (ticks < 0x07)) {
      #elif defined(MILLIS_USE_TIMERA0)
      */
      #if defined(MILLIS_USE_TCA)
        ticks = (TIME_TRACKING_TIMER_PERIOD) - ticks;
        if ((flags & TCA_SPLIT_HUNF_bm) && (ticks < 0x03)) {
      #else /* = defined(MILLIS_USE_TCB) */
        if ((flags & TCB_CAPT_bm) && !(ticks & 0xFF00)) {
      #endif
      #if (defined(MILLIS_USE_TCB) && (F_CPU <= 2000000UL))
        overflows += 2;
      #else
        overflows++;
      #endif
        } // end getting ticks

      /*#if defined(MILLIS_USE_TIMERD0)

        #if (F_CPU == 20000000UL || F_CPU == 10000000UL || F_CPU == 5000000UL)
          uint8_t ticks_l = ticks >> 1;
          ticks = ticks + ticks_l + ((ticks_l >> 2) - (ticks_l >> 4) + (ticks_l >> 7));
          // + ticks +(ticks>>1)+(ticks>>3)-(ticks>>5)+(ticks>>8))
          // speed optimization via doing math with smaller datatypes, since we know high byte is 1 or 0.
          microseconds =   overflows * (TIME_TRACKING_CYCLES_PER_OVF / 20) + ticks; // ticks value corrected above.
        #else
          microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF / 16))
                            + (ticks * (TIME_TRACKING_CYCLES_PER_OVF / 16 / TIME_TRACKING_TIMER_PERIOD)));
        #endif
        #if defined(CLOCK_TUNE_INTERNAL) && !(F_CPU == 16000000UL || F_CPU ==  20000000UL || F_CPU ==  8000000UL || F_CPU ==  10000000UL || F_CPU ==  4000000UL || F_CPU ==  5000000UL)
          #warning "TCD is not supported as a millis timing source when the oscillator is tuned to a frequency other than 16 or 20 MHz. Timing results will be wrong - use TCA0 or a TCB."
        #endif
      */
      #if defined(MILLIS_USE_TCB)
        /* Ersatz Division for TCBs - now with inline assembly!
         *
         * It's well known that division is an operator you want to avoid like the plague on AVR.
         * Not only is it slow, the execution time isn't even easy to analyze - it depends on the
         * two opperands, particularly the divisor... so you can't just look at the generated
         * assembly and count clock cycles, you've got to either time it expoerimentally with
         * a representative set of sample data, or know how many times it will pass through the
         * loops and then count clock cycles. If the operands aren't constant (if they are, you
         * can probably manage to get it optimized away at compile time) your best hope is likely
         * simulation, assuming you know enough about the values it will end up having to divide.
         *
         * Anyway. You don't want to be doing division. But that's what you need in order to
         * appropriately scale the tick count from the prescaler-deprived TCBs. Since many users
         * reconfigure the TCA for advanced PWM, using the TCA-prescaled clock was a non-starter
         * particularly since many parts have only one TCA. But division can be approximated
         * very closely if the divisor is constant using a series of bitshifts and addition/subtraction.
         *
         * The series of shifts was determined numerically with a spreadsheet that calculated the results for
         * each value that could come from the initial round of rightshifts for any combination of
         * bitshifts and provided a number of statistics to select based on (what?). Backwards time travel must
         * never happen, or if it does, it must be a shorter backward step than micros runtime - 1 us
         * otherwise delay() will break and timeouts can instantly expire when it is hit. Similarly,
         * one wants to avoid large jumps forward, and cases where more consecutive "actual" times
         * than absolutely necessary return the same value (time should flow at a constant rate).
         * Finally, the artifacts of the calculation that are unavoidable should be distributed uniformly.
         * Undershooting or overshooting 999 endpoint at the counter's maximum value is the usual
         * source of large jumps (at the overflow point) in either direction. Terms should, as much as
         * possible alternate between positive and negative to minimize artifacts.
         *
         * The most popular/important speeds are hand-implemented in assembly because the compiler
         * was doing a miserable job of it - wasting 20-30% of the execution time and it's one of the few
         * Arduino API functions that users will be surprised and dismayed to find running slowiy.
         * Not only does it run faster on "normal" boards (the 16 MHz clock eliminates the need to divide
         * DxCore offers many speeds where the math doesn't all optimize away to nothing like it does at
         * 1/2/4/8/16/32.
         *
         * Do notice that we are replacing a smaller number of terms, and it's still much faster
         * The 10's went from 5 term ersatz-division to 6, while 12's went from 5 terms to 9, yet still
         * got a lot faster. The terrible twelves are the frequency most difficult to do this with.
         * Ironically, one of the the two that are is easiest is 36, which is good enough with 3 and
         * effectively exact (That "middle 12" is closer than the other 12's get with 9!)
         * 25 also matches it. Maybe it's something 25 and 36 being perfect squares?
         *
         * The three problems were that:
         * 1. Compiler generated code stubbornly insisted doing repeated shift operation in a loop
         * with 3 cycle per iteration (the shift itself took only 2)
         * 2. Compiler could not be convinced to do things that we know will always be < 255 as
         * bytes. Sure, it wouldn't know that - it's not legal for it to do that on it's own.
         * But even when I cast everything to uint8_t, it would shift a 16-bit value around
         * unnecessarily.
         * 3. It would distribute the ticks >> 4. That is, it wouldn't shift the value of
         * ticks in place, even though it wasn't referenced after this because I was assigning
         * the result to ticks, and hence the old value was "dead"
         * Instead, it would copy it, shift the copy 3 or 4 places. Then when it needed the
         * ticks >> 2, it would make a copy of the ORIGINAL and shift that 6 places,
         * instead of copying the copy and shifting just 2 places.
         *
         * A general trend seems to be that the compiler is not smart enough to "reuse" an
         * existing value that has already been shifted such that it's closer to the target.
         * at least for multi-byte variables. This is not the worst example of it I've run into
         * but the micros() function is a little bit sensitive to the execution time.
         * Apparently people sometimes want to *do something* in response to the value it
         * returns - and they seem to want to do that in a timely manner, otherwise they'd
         * have not bothered to record a time so accurately...
         *
         * general algorithm in the assembly implementations is:
         * start with ticks in a register pair, copy to r0, r1.
         * rightshift it until we have the 0th term (closest power of 2).
         * copy it to back to original location..
         * continue rightshifting it, adding or subtracting from the original when we reach
         * the appropriate terms.
         * As soon as we've rightshifted the original enough times that we know it's < 256,
         * we switch from lsr r1 ror r0 to just lsr r0. At the next term that we want to add
         * we copy it to r1. Subsequent subtractions or additions are single-byte until we've got the last term.
         * this time, we add r1 to r0 instead of the other way around.
         * we will need to clear r1 anyway, but we do it now, since we need a known 0 to do the carry.
         * we addthat to the ticks intermediate value to get the final ticks value, and drop back into C
         * where we calculate overflows * 1000, the (now 0-999) ticks to it, and return it.
         *
         */
          // Oddball clock speeds
        #if   (F_CPU == 44000000UL) // Extreme overclocking
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks - /* (ticks >> 1)  + */ (ticks >> 2) - (ticks >> 5) + /* (ticks >> 6) - */ (ticks >> 7)); // + (ticks >> 10)
        #elif (F_CPU == 36000000UL) // 50% overclock!
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks - (ticks >> 3) + (ticks >> 6)); // - (ticks >> 9) + (ticks >> 10) // with 5 terms it is DEAD ON
        #elif (F_CPU == 28000000UL) // Not supported by DxCore - nobody wants it.
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 3) + (ticks >> 5) - (ticks >> 6)); // + (ticks >> 8) - (ticks >> 9)
        #elif (F_CPU == 14000000UL) // Not supported by DxCore - nobody wants it.
          ticks = ticks >> 3;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 3) + (ticks >> 5) - (ticks >> 6)); // + (ticks >> 8) - (ticks >> 9)
        #elif (F_CPU == 30000000UL) // Easy overclock
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks + (ticks >> 3) - (ticks >> 4) + (ticks >> 7) - (ticks >> 8)); // 5 terms is the optimal. Good but not as close as we get for most.
        #elif (F_CPU == 27000000UL) // You'd think this one would be a flaming bitch right?
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 4) - (ticks >> 9)); // +0.1 average error with only 4 terms, minimal scatter... that's just not supposed to happen!
        #elif (F_CPU == 25000000UL) // Barely overclocked.
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks + /* (ticks >> 1) -*/ (ticks >> 2) + /* (ticks >> 4) -*/ (ticks >> 5)); // DEAD ON with 5 terms

        /* The Terrible Twelves (or threes) - Twelve may be a great number in a lot of ways... but here, it's actually 3 in disguise.
         * NINE TERMS in the damned bitshift division expansion. And the result isn't even amazing. - it's worse than what can be done
         * with just 5 terms for dividing by 36 or 25, or a mere 3 terms with 27... where you're dividing by 9, 12.5, and 13.5 respectively,
         * or after the initial shifts, by 0.78125, 1.25 or 1.18, and comparable to the best series for division by 1.375 (44 MHz) or 0.9375 (30 MHz) which each have 7 terms,
         * though it's better than the best possible for the division by 0.875 associated with 28 MHs clocks which is also a 7 term one.
         * This is division by 0.75, which sounds like it should be the easiest out of the lot.
         *
         * This does the following:
         * ticks = ticks >> (1, 2, 3, 4, or 6 for 3 MHz, 6 MHz, 12 MHz, 24 MHz, or 48 MHz)
         * ticks = ticks + (ticks >> 1) - (ticks >> 2) + (ticks >> 3) - (ticks >> 4) + (ticks >> 5) - (ticks >> 6) + (ticks >> 7) - (ticks >> 9)
         *
         * Equivalent to :
         * ticks = ticks / (1.5, 3, 6, 12, or 24)
         *
         * Division is way too slow, but we need to convert current timer ticks, which
         * are are 0-2999, 0-5999, 0-11999, or 0-23999 into the 3 least significant digits
         * of the number of microseconds so that it can be added to overflows * 1000.
         *
         * Runtime of the assembly is 28, 30, 32, or 34 clocks
         * 3 and 6 MHz not a supported speed.
         * 57 replaced with 30 save 27 clocks @ 12 = 2 us saved
         * 67 replaced with 32 save 35 clocks @ 24 = 1.5us saved
         * 77 replaced with 34 save 43 clocks @ 48 = 1 us saved
         */
        #elif (F_CPU == 48000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || F_CPU == 6000000UL || F_CPU == 3000000UL)
          __asm__ __volatile__(
            "movw r0,%A0"   "\n\t" // we copy ticks to r0 (temp_reg) and r1 (zero_reg) so we don't need to allocate more registers.
            "lsr r1"        "\n\t" // notice how at first, each shift takes insns. Compiler wants to use an upper register, ldi number of shifts
            "ror r0"        "\n\t" // into it, then lsr, ror, dec, breq (4 insn + 5 clocks per shift, and including the ldi, it's 5 insns + 5*shiftcount clocks)
            #if (F_CPU != 3000000UL)
              "lsr r1"        "\n\t"
              "ror r0"        "\n\t"
            #endif
            #if (F_CPU == 12000000UL || F_CPU == 24000000UL || F_CPU == 48000000UL)
              "lsr r1"      "\n\t"  // sacrifice 1 word for 9 clocks on the 12 MHz configuration
              "ror r0"      "\n\t"
            #endif
            #if (F_CPU == 24000000UL || F_CPU == 48000000UL)
              "lsr r1"      "\n\t"  // sacrifice 3 words for 12 clocks on the 24 MHz configuration
              "ror r0"      "\n\t"
            #endif
            #if (F_CPU == 48000000UL)
              "lsr r1"      "\n\t"  // sacrifice 5 words for 15 clocks on the 48 MHz configuration.
              "ror r0"      "\n\t"
            #endif
            "movw %A0,r0"   "\n\t"  // This is the value we call ticks, because that's what it was in old code.
            "lsr r1"        "\n\t"  // we just copied the now shifted value back to original location.
            "ror r0"        "\n\t"  // 2 words per shift still
            "add %A0, r0"   "\n\t"  // we now have ticks >> 1, add it to original.
            "adc %B0, r1"   "\n\t"  //
            "lsr r1"        "\n\t"  //
            "ror r0"        "\n\t"  // we now have ticks >> 2. Now it's under 250, and r1 is 0
            "mov r1,r0"     "\n\t"  // so we copy the remaining value into r1.
            "lsr r1 "       "\n\t"  // now it's only 1 insn/shift!
            "sub r0,r1"     "\n\t"  // - ticks >> 3
            "lsr r1"        "\n\t"
            "add r0,r1"     "\n\t"  // + ticks >> 4
            "lsr r1"        "\n\t"
            "sub r0,r1"     "\n\t"  // - ticks >> 5
            "lsr r1"        "\n\t"
            "add r0,r1"     "\n\t"  // + ticks >> 6
            "lsr r1"        "\n\t"
            "sub r0,r1"     "\n\t"  // - ticks >> 7
            "lsr r1"        "\n\t"
            "lsr r1"        "\n\t"
            "add r0,r1"     "\n\t"  // + ticks >> 9
            "eor r1,r1"     "\n\t"  // clear out r1
            "sub %A0,r0"    "\n\t"  // Add the sum of terms that fit in a byte to what was ticks in old code.
            "sbc %B0,r1"    "\n"    // carry - see,this is why AVR needs a known zero.
            : "+r" (ticks));        // Do the rest in C. ticks is a read/write operand.
          microseconds = overflows * 1000 + ticks; // nice and clean.

        /* The Troublesome Tens - I initially fumbled this after the **now** r1 is 0 line
         * I did several dumb things - at first I thought it was my pointless moving and
         * adding. But the real problem was that on that line, I'd just deleted the
         * now unnecessary lsr r1, leaving the next as ror instead of lsr. So instead of pushing
         * that bit into the void, it came back as the new high bit, causing the device to travel
         * back in time. Unfortunately, a few hundred milliseconds isn't far back enough to
         * snag a winning ticket for todays lotto, but more than than the execution time
         * of micros is far enough back to thoroughly break delay() Even if I could just go back
         * just far enough to tell myself where the bug was, I'd take it...
         *
         * This does the following:
         * ticks = ticks >> (1, 2, 3, or 4 for 5 MHz, 10 MHz, 20 MHz, or 40 MHz)
         * ticks = ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6) + (ticks >> 8)
         *
         * Equivalent to:
         * ticks = tick / (2.5, 5, 10, or 20)
         * Division is way too slow, but we need to convert current timer ticks, which
         * are 0-2499, 0-4999, 0-9999, or 0-19999, into the 3 least significant digits
         * of the number of microseconds so that it can be added to overflows * 1000.
         *
         * Runtime is 23,25,27, or 29 clocks, savings vs the best I could do in C
         *
         * 33 replaced with 23 save 10 clocks @ 5  = 2 us saved
         * 46 replaced with 25 save 21 clocks @ 10 = 2.5 us saved
         * 56 replaced with 27 save 29 clocks @ 20 = 1.5 us saved
         * 66 replaced with 29 save 37 clocks @ 40 = 1 us saved
         */
        #elif (F_CPU == 40000000UL || F_CPU == 20000000UL || F_CPU == 10000000UL || F_CPU == 5000000UL)
          __asm__ __volatile__(
            "movw r0,%A0"   "\n\t"  // no savings until after the initial rightshifts at 5 MHz
            "lsr r1"        "\n\t"
            "ror r0"        "\n\t"
            #if (F_CPU == 10000000UL || F_CPU == 20000000UL || F_CPU == 40000000UL)
              "lsr r1"      "\n\t"  // sacrifice 1 word for 9 clocks at 10 MHz
              "ror r0"      "\n\t"
            #endif
            #if (F_CPU == 20000000UL || F_CPU == 40000000UL)
              "lsr r1"      "\n\t"  // sacrifice 3 words for 12 clocks at 20 MHz
              "ror r0"      "\n\t"
            #endif
            #if (F_CPU == 40000000UL)
              "lsr r1"      "\n\t"  // sacrifice 5 words for 15 clocks at 40 MHz
              "ror r0"      "\n\t"
            #endif
            "movw %A0,r0"   "\n\t"  // ticks
            "lsr r1"        "\n\t"
            "ror r0"        "\n\t"
            "lsr r1"        "\n\t"
            "ror r0"        "\n\t"  //   ticks >> 2.
            "sub %A0, r0"   "\n\t"  // - ticks >> 2
            "sbc %B0, r1"   "\n\t"  // It could be 312 so we can't do what we did for the 12's
            "lsr r1"        "\n\t"
            "ror r0"        "\n\t"  // **now** r1 is 0.
            "lsr r0"        "\n\t"
            "mov r1,r0"     "\n\t"  // + ticks >> 4
            "lsr r1"        "\n\t"
            "lsr r1"        "\n\t"
            "sub r0,r1"     "\n\t"  // - ticks >> 6
            "lsr r1"        "\n\t"
            "lsr r1"        "\n\t"
            "add r0,r1"     "\n\t"  // + ticks >> 8
            "eor r1,r1"     "\n\t"  // restore zero_reg
            "add %A0,r0"    "\n\t"  // add to the shifted ticks
            "adc %B0,r1"    "\n"    // carry
            : "+r" (ticks));        // Do the rest in C. ticks is a read/write operand.
          microseconds = overflows * 1000 + ticks;
        /* replaces:
        #elif (F_CPU == 48000000UL) // Extreme overclocking
          ticks = ticks >> 5;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
        #elif (F_CPU == 24000000UL) // max rated speed
          ticks = ticks >> 4;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
        #elif (F_CPU == 12000000UL)
          ticks = ticks >> 3;
          microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
        // Never was an implementation for 3 or 6, but it's obvious what the old style implementation would be,
        #elif (F_CPU == 40000000UL) // overclocked aggressively
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
        */

        // powers of 2  - and a catchall for parts without dedicated implementations. It gives wrong results, but
        // it also doesn't take forever like doing division would.
        #elif (F_CPU  == 32000000UL || F_CPU > 24000000UL)
          microseconds = overflows * 1000 + (ticks >> 4);
        #elif (F_CPU  == 16000000UL || F_CPU > 12000000UL)
          microseconds = overflows * 1000 + (ticks >> 3);
        #elif (F_CPU  ==  8000000UL || F_CPU >  6000000UL)
          microseconds = overflows * 1000 + (ticks >> 2);
        #elif (F_CPU  ==  4000000UL || F_CPU >= 3000000UL)
          microseconds = overflows * 1000 + (ticks >> 1);
        #else //(F_CPU == 1000000UL || F_CPU == 2000000UL) - here clock is running at system clock instead of half system clock.
              // and hence overflows only once per 2ms. On 2 MHz
              // also works at 2MHz, since we use CLKPER for 1MHz vs CLKPER/2 for all others.
          microseconds   = overflows * 1000 + ticks;
        #endif
        #if !((F_CPU == 48000000UL || F_CPU == 36000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || /* multiples of 12           */ \
               F_CPU == 40000000UL || F_CPU == 30000000UL || F_CPU == 20000000UL || F_CPU == 10000000UL || /* multiples of 10           */ \
               F_CPU == 32000000UL || F_CPU == 16000000UL || F_CPU ==  8000000UL || F_CPU ==  4000000UL || /* powers of 2               */ \
               F_CPU ==  2000000UL || F_CPU ==  1000000UL || F_CPU == 25000000UL || F_CPU ==  5000000UL || /* powers of 2 cont, 25, 5   */ \
               F_CPU == 44000000UL || F_CPU == 28000000UL || F_CPU == 14000000UL || F_CPU ==  3000000UL || /* oddball frequencies       */ \
               F_CPU == 27000000UL)&& /* warn fools who messed with the timers.h file too and expected that the core would sort out how */ \
              ((TIME_TRACKING_TIMER_DIVIDER == 2 && TIME_TRACKING_TICKS_PER_OVF == F_CPU/2000) || /*how to make the timer work correctly*/ \
               (TIME_TRACKING_TIMER_DIVIDER == 1 && (TIME_TRACKING_TICKS_PER_OVF == F_CPU/500 && F_CPU == 1000000) || (TIME_TRACKING_TICKS_PER_OVF == F_CPU/1000 && F_CPU == 2000000))))
                                                   /*  without them implementing it. No such luck  */
          #warning "Millis timer (TCBn) at this frequency and/or configuration unsupported, micros() will return totally bogus values."
        #endif
      #else /* = defined(MILLIS_USE_TCA) */
        __attribute__((unused)) uint8_t ticks_temp;  // As this is the case for TCA, ticks is always 8 bit
        __attribute__((unused)) int16_t ticks_acc;   // As the compiler does not "buffer" previous bit-shifts to the same variable, we do it ourselves
        #if (F_CPU == 30000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          ticks_acc  = ticks * 2;
          ticks_acc += ticks >> 3;
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 28000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          ticks_acc  = ticks * 2;
          ticks_acc += (ticks_temp = (ticks >> 2));
          ticks_acc += (ticks_temp = (ticks_temp >> 3)); //ticks_temp = ticks >> 5
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 25000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          ticks_acc  = ticks * 2;
          ticks_acc += ticks >> 1;
          ticks_acc += ticks >> 4; //ticks_temp = ticks >> 4 (compiler uses SWAP + ANDI 0x0F, faster then shifting three times)
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 24000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)  // 1 timer clock equals 2.667 us
          ticks_acc  = ticks * 3;                         // 1 tick = 3 us
          ticks_acc -= (ticks_temp = (ticks >> 1));       // (1/2) 1 tick = 2.5 us   =>   2.5us - 2.667us =  0.167us
          ticks_acc += (ticks_temp = (ticks_temp >> 2));  // (1/8) 1 tick = 2.675 us => 2.675us - 2.667us = -0.007us
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 20000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF));
          ticks_acc  = ticks * 3;
          ticks_acc += (ticks_temp = (ticks >> 2));
          ticks_acc -= (ticks_temp = (ticks_temp >> 2));   //ticks_temp = ticks >> 4
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 16000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)  // 1 timer clock equals 3.922 us
          ticks_acc  = ticks * 4;                         // 1 tick = 4 us, 4us-3.922us = 0.078
          ticks_acc -= (ticks_temp = (ticks >> 4));       // (1/16) 1 tick = 3.9375us => 3.9375us-3.922us =  0.0155
          ticks_acc -= (ticks_temp = (ticks_temp >> 2));  // (1/64) 1 tick = 3.9219us => 3.9219us-3.922us = -0.0003
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 12000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)  // 1 timer clock equals 5.49 us
          ticks_acc  = ticks * 5;                         // 1 tick = 5 us, 5us-5.49us = -0.49us
          ticks_acc += ticks >> 1;                        // (1/2) 1 tick = 5.5us => 5.5us-5.49us =  0.010us    // next would be >>7 - not really worth it
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #elif (F_CPU == 10000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)  // 1 timer clock equals 6.27 us
          ticks_acc  = ticks * 6;                         // 1 tick = 6 us, 6us-6.27us = -0.27us
          ticks_acc += (ticks_temp = (ticks >> 2));       // (1/4)  1 tick = 6.25us  => 6.25us-6.27s =  -0.020us
          ticks_acc += (ticks_temp = (ticks_temp >> 4));  // (1/64) 1 tick = 6.266us => 6.266us-6.27s = -0.004us
          microseconds  = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)) + ticks_acc;
        #else
          #if (TIME_TRACKING_TIMER_DIVIDER%(F_CPU/1000000))
            #warning "Millis timer (TCA) at this frequency unsupported, micros() will return bogus values."
          #endif
          microseconds = ((overflows * millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
                        + (ticks * (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) / TIME_TRACKING_TIMER_PERIOD)));
        #endif
      #endif // end of timer-specific part of micros calculations
      return microseconds;
    }
  #else // MILLIS_USE_TIMERRTC is defined, so we don't have micros
    /* We do not have a timebase sufficiently accurate to give microsecond timing. In fact, we barely have millisecond timing available
     * The microsecond delay counts clock cycles, and so it does still work. It is planned that a future library will switch the millis
     * pause millis before sleeping and turn on the RTC, tracking the passage of time to a much coarser resolution with that, and turn
     * it back on when waking from sleep, so people can keep time while sleeping without sacrificing micros().
     * In any event, as of 2.4.3 we now provide the stub below, which we hope is more useful than being told that micros() isn't defined.
     */
      unsigned long micros() {
        badCall("microsecond timekeeping is not supported when the RTC is used as the sole timekeeping timer (though delayMicroseconds() is)");
        return -1;
      }
  #endif /* !defined(MILLIS_USE_TIMERRTC) */
#else // MILLIS_USE_TIMERNONE defined - we have neither of these functions.
    /* Uses should not call millis() or micros() if the core timekeeping has been disabled. Usually, encountering this error either means
     * that they disabled millis earlier for some other sketch, and the preferences were saved with that - or that they are using a library
     * with a dependence on the timekeeping facilities. Sometimes these are meaningful, other times it is only for a feature that isn't
     * being used, or to catch a particular corner case (see: tinyNeoPixel, very end of show() for an example).
     * As of 2.4.3 we provide the stubs below, which we hope is more useful than being told that millis or micros isn't defined.
     */
    unsigned long micros() {
      badCall("micros() is not available because it has been disabled through the tools -> millis()/micros() menu");
      return -1;
    }
    unsigned long millis() {
      badCall("millis() is not available because it has been disabled through the tools -> millis()/micros() menu");
      return -1;
    }
#endif // MILLIS_USE_TIMERNONE code


/* delay()
 * So what do you WANT in a good delay function?
 * First, obviously you want it to delay things. You do not want it to block interrupts (then a long one would throw off
 * timekeeping, miss inputs, and so on). And you want the compiled size to not be prohibitive for the part.
 * The reason it's so important wrt. interrupts is that in Arduino standard delay(), if an interrupt fires in the middle,
 * will still end at the same time - it is "interrupt insensitive". Whenever a delay is using the builtin _delay_ms()
 * if that is interrupted it has no way of knowing time has passed. Now hopefully you're not spending so much time in
 * an ISR that this is significant, but it is still undesirable.
 *
 * For the unfortunate souls using small-flash parts, the flash usage becomes a major problem - why is it such a space-hog?
 * Because it has to pull in micros(), which is bulky even with the division turned into bitshifts... RTC has same problem
 * with millis(), the conversion of 1024ths of a second to 1000ths is a killer, even with the bitshift tricks,
 * and the compiler seems really stupid about how it handles it; I can't keep it from making an extra copy of the 32-bit
 * value, which ALSO requires 4 more push and pop operations to get registers it can use.
 *
 * Now we will use one of three delay() implementations:
 * If you have 16k+ your delay is the standard one, it pulls in micros(), yes, but you may well already have grabbed
 *  that for your sketch already, and the delay is more accurate and fully interrupt insensitive, and you can afford
 *  the memory. For RTC users they will get the analogous implementation that is based on millis.
 * Users with millis disabled, or with less than 16k flash and using RTC will get the implementation based on _delay_ms().
 * Everyone else (flash under 16k but millis enabled via non-RTC timer) will get the light version which calls _delay_ms()
 *  if the delay is under 16 ms to get less flash usage, and calculates the delay using **millis** not micros otherwise,
 *  saving over 100b of flash. The reason for the split is that the limited granularity of millis introduces an error in
 *  the delay duration of up to 1ms. That doesn't matter much when you call delay(1000) on an internal clock that's within
 *  1% on a good day. It matters greatly when you call delay(1);    */


#if (!(defined(MILLIS_USE_TIMERNONE) || defined(MILLIS_USE_TIMERRTC) || (F_CPU == 7000000L || F_CPU == 14000000)))
  // delay implementation when we do have micros() - we know it won't work at 7 or 14, and those can be generated
  // from internal, and switch logic is in even though micros isn't.
  void delay(unsigned long ms)
  {
    uint16_t start = (uint16_t)micros();
    while (true) {
      if (ms == 0) break;
      yield();
      uint16_t us_passed = (uint16_t)micros() - start;
      if (us_passed >= 1000) {
        ms--;
        start += 1000;
      }
    }
  }
#else // delay implementation when we do not
  void _delay_nonconst(unsigned long ms) {
    while (ms > 0) {
      _delay_ms(1);
      ms--;
    }
  }
  inline __attribute__((always_inline)) void delay(unsigned long ms) {
    if(__builtin_constant_p(ms)) {
      _delay_ms(ms);
    } else {
      _delay_nonconst(ms);
    }
  }
#endif

inline __attribute__((always_inline)) void delayMicroseconds(unsigned int us) {
  // This function gets optimized away, but to what depends on whether us is constant.
  if (__builtin_constant_p(us)) {
    _delay_us(us); // Constant microseconds use the avr-libc _delay_us() which is highly accurate for all values and efficient!
  } else { // If it is not, we have to use the Arduino style implementation.
    _delayMicroseconds(us);
  }
}

/* delayMicroseconds() when delay is not a compile-time known constant.
 * Delay for the given number of microseconds. This is UGLY AS SIN and explicitly depends on function call
 * overhead for very short delays.
 * High clock speeds shouldn't return immediately for a 1us delay - we can instead only drop a fraction of a us
 * 48, 44, 40, and 32 drop 1/2 us, and 36 drops 2/3rds.
 * Note that us ceases to be in units of microseconds as soon as the function is entered; it gets turned into the loop counter.
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
 * rcall instructions can get 6 clocks in a single word, though we only get to take advantage of that once for
 * the 30 MHz case and any longer delays do better with a loop.
 */
#if   F_CPU >= 48000000L
  // 16 MHz math, 12-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 44000000L
  // 16 MHz math, 11-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_ELEVEN
#elif F_CPU >= 40000000L
  // 20 MHz math, 10-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_TEN
#elif F_CPU >= 36000000L
  // 12 MHz math, 12-cycle loop, 1us burns and passes through loop once.
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 32000000L
  // 16 MHz math, 8-cycle loop, 1us passes through loop twice.
  #define DELAYMICROS_EIGHT
#elif F_CPU >= 30000000L
  // 12 MHz math, 10-cycle loop, 1us burns and returns.
  #define DELAYMICROS_TEN
#elif F_CPU >= 28000000L
  // 16 MHz math, 7-cycle loop, 1us burns and returns.
  #define DELAYMICROS_SEVEN
#elif F_CPU >= 27000000L
  // 12 MHz math, 9 cycle loop, 1us burns and returns
  #define DELAYMICROS_NINE
#elif F_CPU >= 24000000L
  // 12 MHz math, 8-cycle loop, 1us burns and returns.
  #define DELAYMICROS_EIGHT
#elif F_CPU >= 20000000L
  // 20 MHz math, 10-cycle loop, 1us burns and returns.
  #define DELAYMICROS_TEN
#elif F_CPU >= 16000000L
  // 16 MHz math, 4-cycle loop, 1us returns immediately.
#elif F_CPU >= 12000000L
  // 16 MHz math, 4-cycle loop, 1us returns immediately.
#elif F_CPU >= 10000000L || (F_CPU >= 5000000L && F_CPU < 8000000L)
  // 10 MHz: 5-cycle loop, 1us returns immediately
  // 5 MHz: 5-cycle loop, 1-3 us returns immediately.
  #define DELAYMICROS_FIVE
#else
  // 8 MHz: 16 MHz math, 4-cycle loop, 1-2 us returns immediately.
  // 4 MHz: 16 MHz math, 4-cycle loop, 1-4 us returns immediately.
  // 2 MHz: 16 MHz math, 4-cycle loop, 1-8 us returns immediately.
  // 1 MHz: 16 MHz math, 4-cycle loop, < 16 us returns immediately, < 25 burns and returns.
  // Anything not listed uses the fastest one that is and which is slower than F_CPU
#endif

__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us) {
 /* Must be noinline because we rely on function-call overhead */
#if F_CPU == 48000000L
  // make the initial delay 24 cycles
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  // the loop takes 1/4 of a microsecond (12 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~22 cycles above, subtraction takes another 2 - so we've lost
  // half a us and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles,

#elif F_CPU >= 44000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 5 cycles in 3 words.
  // the loop takes 1/4 of a microsecond (11 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 2
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 40000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 3 cycles in 2 words.
  // the loop takes 1/4 of a microsecond (10 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 17 (19) cycles above, remove 2.
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 36000000L
  // Here we get the initial delay is about 24 cycles, so we pass through
  // the loop once for 1us delay.
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 10 cycles in 4 words
  // the loop takes 1/3 of a microsecond (12 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 23 (25) cycles above, remove 2
  us -= 2; // 2 cycles

#elif F_CPU >= 32000000L
  // here, we only take half a us at the start
  __asm__ __volatile__ ("rjmp .+0");
                          // wait 2 cycles
  // in by the end of this section.
  // the loop takes 1/4 of a microsecond (8 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~14 cycles above, subtraction takes another 2 - so we've lost half a us,
  // and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles

#elif F_CPU >= 30000000L
  // for a one-microsecond delay, burn 14 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
    "ret" "\n\t"          // 4 cycles - rjmped over initially...
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
    "rcall .-6");         // 2+4 cycles - ... and here again!
                          // Waiting 14 cycles in only 4 words
  if (us <= 1) return; // = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (10 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 28 (30) cycles above, remove 3
  us -= 3; // 2 cycles

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

  // the loop takes 1/4 of a microsecond (7 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles=
  // we just burned 27 (29) cycles above, remove 4, (7*4=28)
  // us is at least 8 so we can subtract 5
  us -= 4; // = 2 cycles,

#elif F_CPU >= 27000000L
  // for a one-microsecond delay, burn 11 cycles and return
  __asm__ __volatile__ (  // wait 8 cycles with 3 words
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here);
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 more  == 11 total
  if (us <= 1) return;    //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 27 (24) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 25000000L
  // for a one-microsecond delay, burn 8 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/5 of a microsecond (5 cycles) per iteration
  // so execute it 5x  for each microsecond of delay requested.
  us = (us << 2) + us; // x5 us, = 7 cycles
  // we just burned 24 (22) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 24000000L
  // for a one-microsecond delay, burn 8 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 24 (22) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 20000000L
  // for a one-microsecond delay, burn 4 clocks and then return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop" );              // 1 cycle
                          // wait 3 cycles with 2 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes a 1/2 of a microsecond (10 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 21 (23) cycles above, remove 2
  // us is at least 4 so we can subtract 2.
  us -= 2; // 2 cycles

#elif F_CPU >= 16000000L
  // for a one-microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/4 of a microsecond (4 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 5; // = 2 cycles

#elif F_CPU >= 12000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (4 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
  us -= 5; // 2 cycles

#elif F_CPU >= 10000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (5 cycles) per iteration
  // so execute it 2 times for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 20 (22) cycles above, remove 4, (5*4=20)
  // us is at least 6 so we can subtract 4
  us -= 4; // 2 cycles

#elif F_CPU >= 8000000L
  // for a 1 and 2 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 2us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (4 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us <<= 1; // x2 us, = 2 cycles
  // we just burned 17 (19) cycles above, remove 5, (4*5=20)
  // us is at least 6 so we can subtract 4
  us -= 5; // = 2 cycles

#elif F_CPU >= 5000000L
  // for a 1 ~ 3 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 3us
  if (us <= 3) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (5 cycles) per iteration
  // so just remove 3 loops for overhead
  us -= 3; // = 2 cycles

#elif F_CPU >= 4000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 4us
  if (us <= 4) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#elif F_CPU >= 2000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 8us
  if (us <= 8) return; // 3 cycles, (4 when true)
  // the loop takes 2 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us >>= 1; // divide by 2.
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#else // F_CPU >= 1000000
  // for the 1 MHz internal clock (default settings for common AVR microcontrollers)
  // the overhead of the function calls is 14 (16) cycles
  if (us <= 16) return; // 3 cycles, (4 when true)
  if (us <= 25) return; // 3 cycles, (4 when true), (must be at least 26 if we want to subtract 22 and rightshift twice.)
  // compensate for the time taken by the preceding and following commands (about 22 cycles)
  us -= 22; // = 2 cycles
  // the loop takes 4 microseconds (4 cycles)
  // per iteration, so execute it us/4 times
  // us is at least 4, divided by 4 gives us 1 (no zero delay bug)
  us >>= 2; // us div 4, = 4 cycles
#endif
/* Implementation of the delay loop of 4, 5, 7, 8, 10, 11, or 12 clocks. */
#if defined(DELAYMICROS_TWELVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+2"      "\n\t"            // 2 cycles - jump over next instruction.
    "ret"           "\n\t"            // 4 cycles - rjmped over initially....
    "rcall .-4"     "\n\t"            // 2 cycles - ... but then called here
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_ELEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_TEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_NINE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_EIGHT)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_SEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_FIVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#else // the classic 4 cycle delay loop...
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#endif
  // return = 4 cycles
}

void init() {
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
  // enabling interrupts is done in main.cpp after a final empty function (that the user can override) is called.
}

void stop_millis() { // Disable the interrupt:
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("stop_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.INTCTRL &= 0xFE;
    #elif defined(MILLIS_USE_TIMERRTC)
      RTC.INTCTRL &= 0xFE;
      RTC.CTRLA &= 0xFE;
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
    /*
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLA = 0x00;
      while(TCD0.STATUS & 0x01);
    */
    #elif defined(MILLIS_USE_TCB) // It's a type b timer
      _timer->CTRLB = 0;
    #endif
    init_millis();
  #endif
}

void __attribute__((weak)) init_millis()
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    /*
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CMPBCLR        = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
      TCD0.CTRLB          = 0x00; // oneramp mode
      TCD0.CTRLC          = 0x80;
      TCD0.INTCTRL        = 0x01; // enable interrupt
      TCD0.CTRLA          = TIMERD0_PRESCALER | 0x01; // set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
      while(RTC.STATUS); // if RTC is currently busy, spin until it's not.
      // to do: add support for RTC timer initialization
      RTC.PER             = 0xFFFF;
      #ifdef MILLIS_USE_TIMERRTC_XTAL
        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x03);
        RTC.CLKSEL        = 2; // external crystal
      #else
        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA,0x02);
        // RTC.CLKSEL=0; this is the power on value
      #endif
      RTC.INTCTRL         = 0x01; // enable overflow interrupt
      RTC.CTRLA           = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|RTC_PRESCALER_DIV32_gc);//fire it up, prescale by 32.
    */
    #else // It's a type b timer - we have already errored out if that wasn't defined
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

void set_millis(__attribute__((unused))uint32_t newmillis)
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
  #else
    /*
    #if defined(MILLIS_USE_TIMERRTC)
      // timer_overflow_count = newmillis >> 16;
      // millis = 61/64(timer_overflow_count << 16 + RTC.CNT)
      uint8_t oldSREG = SREG; // save SREG
      cli();                // interrupts off
      uint16_t temp = (newmillis % 61) << 6;
      newmillis = (newmillis / 61) << 6;
      temp = temp / 61;
      newmillis += temp;
      timer_overflow_count = newmillis >> 16;
      while(RTC.STATUS&RTC_CNTBUSY_bm); // wait if RTC busy
      RTC.CNT = newmillis & 0xFFFF;
      SREG = oldSREG; // reemable oimterripts if we killed them,
    #else
     */
      /* farting around with micros via overflow count was ugly and buggy.
       * may implement again, better, in the future - but millis and micros
       * will get out of sync when you use set_millis
       * I think the way to do it is to make this implementation (but not big one)
       * inline, so if newmillis is constant, we can calculate the (compile-time known)
       * number of overflows using all the floating point math we want, and otherwise,
       * document that it will zero out micros. (I believe MX682X wrote this?)*/

      /* I see no problem with millis and micros getting out of sync. This function is
       * called set_millis, not setMicros(). Why should it change micros? Remember, this function is planned for applications like:
       * Chucking millis forward when you wake up before hitting snooze again.
       * uhhh. Honestly I think that's it.
       * And in that circumstance, if you have something being timed with micros, either you should be using millis, or you went to sleep when
       * you shouldn't have, and you have long since missed your appointment. If you're timing something with micros while doing something on the timescales this is meant for
       * you're doin' it wrong. I'm not convinced that it matters that micros and millis get out of sync.
       *
       * Also, at least on DxCore, 99% of millis timers are going to be TCB. But this limitation only effects TCD0 (unsupported) and TCAn. It might become more
       * relevant when the EB-series comes along with those two new timers. On the other hand, it may turn out that there is some way to make one or either of them into
       * good millis timers (I would say I doubt that for TCE - it's too valuable as a PWM timer - but with even a 2:1 prescaler option, you could slow down a TCF so you got
       * ONE MILLION microseconds per overflow - though extracting millis from micros would then be hard. )
       * Using TCA for millis is what you do when you're desperate, not what you set out planning to do. timer_millis is all the state we store for TCB.
       * so this won't cause any desynchronization of timing for the vast majority of users.
       * -SK 2/4/23
       */
      timingStruct.timer_millis = newmillis;
    //#endif
  #endif
}

void nudge_millis(__attribute__((unused)) uint16_t nudgesize) {
  #if (MILLIS_TIMER & 0x78) /* 0x40 matches TCDs, 0x20 matches TCBs, 0x10 matches TCA0 0x08 matches TCA1, so OR them together and AND it with the timer to make sure it's not RTC, disabled, etc.  */
    uint8_t oldSREG=SREG;
    cli();
    timingStruct.timer_millis += nudgesize;
    SREG=oldSREG;
  #else
    #if defined(MILLIS_USE_TIMERNONE)
      badCall("nudge_millis() is not available with millis disabled. What are you hoping for it to do?");
    #else
      badCall("The selected timer does not support nudging millis at this point in time. Only TCA, TCB and TCD timers support this currently.");
    #endif
  #endif
}

/********************************* ADC ****************************************/
#if defined(ADC0)
  void __attribute__((weak)) init_ADC0() {
    #if F_CPU >= 48000000
      ADC0.CTRLC = ADC_PRESC_DIV48_gc; // 1 @ 48 MHz
    #elif F_CPU >  40000000
      ADC0.CTRLC = ADC_PRESC_DIV32_gc; // 1.25 @ 40 MHz
    #elif F_CPU >= 36000000
      ADC0.CTRLC = ADC_PRESC_DIV28_gc; // 1.286 @ 36 MHz
    #elif F_CPU >  28000000
      ADC0.CTRLC = ADC_PRESC_DIV24_gc; // 1.33 @ 32 MHz, 1.
    #elif F_CPU >= 24000000
      ADC0.CTRLC = ADC_PRESC_DIV20_gc; // 1.2 @ 24, 1.25 @ 25, 1.4 @ 28  MHz
    #elif F_CPU >= 20000000
      ADC0.CTRLC = ADC_PRESC_DIV16_gc; // 1.25 @ 20 MHz
    #elif F_CPU >  12000000
      ADC0.CTRLC = ADC_PRESC_DIV12_gc; // 1 @ 12, 1.333 @ 16 MHz
    #elif F_CPU >= 8000000
      ADC0.CTRLC = ADC_PRESC_DIV8_gc;  // 1-1.499 between 8 and 11.99 MHz
    #elif F_CPU >= 4000000
      ADC0.CTRLC = ADC_PRESC_DIV4_gc;  // 1 MHz
    #else  // 1 MHz / 2 = 500 kHz - the lowest setting
      ADC0.CTRLC = ADC_PRESC_DIV2_gc;
    #endif
    ADC0.SAMPCTRL = 14; // 16 ADC clock sampling time - should be about the same amount of *time* as originally?
    // This is WAY conservative! We could drop it down...
    ADC0.CTRLD = ADC_INITDLY_DLY64_gc; // VREF can take 50uS to become ready, and we're running the ADC clock
    // at around 1 MHz, so we want 64 ADC clocks when we start up a new reference so we don't get bad readings at first
    /* Enable ADC */
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
    // start at 10 bit for compatibuility with existing code.

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

/**************************************** CLOCK ************************************************/
/* This ugly function configures the system clock speed to match what the user requested       *
 * See also the Clock Failure Detection section immediately below for the helper functions to  *
 * attempt to handle problems with external clock sources. It doesn't work very well though.   *
 * the system is more liekly to think the clock is working, switch to it, and then find that   *
 * actually it's not working and end up bootlooping.                                           */

// These are defaults that could be overridden by variant or arguments passed to compiler
// They are only relevant for the case of using a crystal.
#if CLOCK_SOURCE == 1
  #ifndef USE_XTAL_DRIVE
    // In a quick test, with terrible layout (strip-board), I could run a 16 MHz crystal with any of these options!
    // it was an 18 pf crystal with parasitic capacitance of stripboard as loading. User can force it to desired value
    // but nobody is likely to care. Lower speed settings use less power, I *think* - but the datasheet has nothing
    // to say about it.
    // It has been found experimentally that often 24 MHz crystals with poor layout or poorly chosen loading capacitors
    // would work if the core was told they were 25 MHz; the tests below were changed from > to >= to put frequencies at
    // the top of one of the ranges into the next highest bucket.
    #if     (F_CPU >= 24000000)
      #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_32M_gc
    #elif   (F_CPU >= 16000000)
      #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_24M_gc
    #elif   (F_CPU >=  8000000)
      #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_16M_gc
    #else
      #define USE_XTAL_DRIVE CLKCTRL_FRQRANGE_8M_gc
    #endif
  #endif
  #ifndef USE_CSUTHF
    #define USE_CSUTHF CLKCTRL_CSUTHF_4K_gc
  #endif
#endif


void  __attribute__((weak)) init_clock() {
  #if CLOCK_SOURCE == 0
    /* internal can be cranked up to 32 Mhz by just extending the prior pattern from 24 to 28 and 32.
     *  F_CPU    CLKCTRL_FREQSEL or FRQSEL depending on ATpack version
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
     * 28 MHz    0xA  - undocumented, and makes the math harder
     * 32 MHz    0xB  - undocumented
     * 0xC-F repeat 0x8-0xB
     */
    /* Some speeds not otherwise possible can be generated with the internal oscillator by prescaling
     * This is done for 5 and 10 because those were common speeds to run 0 and 1-series parts at.
     * The logic for switching clock to the other stupid frequencies that can be generated this way
     * is implemented, but millis, micros, and delayMicroseconds may not be. See the clock reference.
     * There is just no demand for operation at these strange frequencies.
     */
    /* Give us a FREQing break, Microchip! In ATpack version 1.9.103, they renamed another field...
     * FREQSEL was changed to FRQSEL. It is unclear to me why it was necessary to make a breaking change
     * impacting nearly all existing code that used the internal oscillator on these parts but it must
     * have been very important to eliminate that E.
     * We here use the old spelling, and breathe a sigh of relief that we didn't use the group code names.
     * Our supplied core_devices.h will work around this: if the the old spelling of the group masks and
     * is not defined, we define it to the new spelling (and if neither is defined, then we are attempting
     * to compile for something that isn't a supported part so failure is expected). core_devices.h now does
     * patch up the group codes
     */
    #if (F_CPU == 32000000)
      /* Overclocked - generally reliable at room temperature, and a very convenient frequency. */
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0B << 2));
    #elif (F_CPU == 28000000)
      /* Overclocked - generally quite reliable at room temperature, but a dumb frequency (see PWM section) */
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
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x01 << 2)); /* Not exposed by the tools submenus but will probably work */
      #warning "2 MHz, currently selected for F_CPU, is not supported by this core and has not been tested. Expect timekeeping problems."
    #elif (F_CPU == 1000000)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x00 << 2));
      /* Prescaled clock options */
    #elif (F_CPU == 14000000)
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2));
      #warning "14 MHz, currently selected for F_CPU, is not supported by this core and has not been tested. Expect timekeeping problems."
    #elif (F_CPU == 10000000) /* 10 MHz = 20 MHz prescaled by 2 */
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));
    #elif (F_CPU == 7000000)
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2));
      #warning "7 MHz, currently selected for F_CPU, is not supported by this core and has not been tested. Expect timekeeping problems."
    #elif (F_CPU == 6000000)
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2));
      #warning "6 MHz, currently selected for F_CPU, is not supported by this core and has not been tested. Expect timekeeping problems."
    #elif (F_CPU == 5000000)  /* 5 MHz = 20 MHz prescaled by 4 */
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm));
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));
    #else
      #error "F_CPU defined as an unsupported value for the internal oscillator."
    #endif
  #elif (CLOCK_SOURCE == 1 || CLOCK_SOURCE == 2)
  /* For this, we don't really care what speed it is at - we will run at crystal frequency, and THE USER MUST TELL US WHAT THAT IS.
   * It is foolish to determine what we're running at at runtime, as the user should really knoe the basic parameters of the
   * crystal - it's usually printed on the damned thing. We don't prescale from crystals, eveh though the hardware is perfectly
   * capable of it. If someone wants to make a convincing argument for adding it, please do so in the issues.
   * Crystals in the relevant frequency range are readily available; it's not like you there's a reasonable frequency to run at for
   * which you can't obtain a crystal, but you can get one for twice that speed.
   * In fact, there is an almost infinite variety of crystals for *unreasonable* frequencies within the range of clock speeds that
   * these parts run at. Unlike on classic AVRs, there is no need for wacky UART crystals.
   */
    #if !defined(CLKCTRL_XOSCHFCTRLA)
      // it's an AVR DA-series or something with that version of CLKCTRL.
      #if (CLOCK_SOURCE == 1)
        #error "AVR DA-series selected, but crystal as clock source specified. DA-series parts only support internal oscillator or external clock."
      #else
        // external clock
        uint8_t i = 255;
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc); //
        while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
          i--;
          if(i == 0) onClockTimeout();
          // in my tests, it only took a couple of passes through this loop to pick up the external clock, so at this point we can be pretty certain that it's not coming....
        }
      #endif
    #else
      // it's a DB/DD with the crystal supporting version of CLKCTRL.
      // turn on clock failure detection - it'll just go to the blink code error, but the alternative would be hanging with no indication of why!
      // Unfortunately, this is not reliable when a crystal is used, only for external clock. It appears that crystal problems often result in
      // a clock sufficiently broken that it resets instead.
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLC, CLKCTRL_CFDSRC_CLKMAIN_gc | CLKCTRL_CFDEN_bm);
      _PROTECTED_WRITE(CLKCTRL_MCLKINTCTRL, CLKCTRL_CFD_bm);
      #if (CLOCK_SOURCE == 2)
        // external clock
        // CLKCTRL_SELHF_EXTCLOCK_gc or CLKCTRL_SELHF_EXTCLK_gc? Microchip can't seem to decide, and we can't test for it with the preprocessor because it's a bloody enumerated type.
        // 0x02 is the numeric value of that constant. I can't even provide compatibility defines, because I don't have any way to tell which one it is for a given version of the headers.
        _PROTECTED_WRITE(CLKCTRL_XOSCHFCTRLA, (0x02 | CLKCTRL_ENABLE_bm));
        uint8_t i = 255;
      #else
        // external crystal
        _PROTECTED_WRITE(CLKCTRL_XOSCHFCTRLA, (USE_CSUTHF | USE_XTAL_DRIVE | CLKCTRL_SELHF_XTAL_gc | CLKCTRL_ENABLE_bm));
        /*Formerly CLKCTRL_SELHF_CRYSTAL_gc, but they changed it 6 months after they started shipping DB's*/
        uint16_t i = 8192; // crystals can take a lot longer to reach stability.
      #endif
    #endif
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
      i--;
      if(i == 0) onClockTimeout();
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
 * anything the user might upload.                                                             *
 * The weakly defined functions can be overridden if the blink code should be disabled or if   *
 * the same frequency can be achieved with internal oscilaltor and that is desired as a        *
 * fallback instead of halting startup.                                                         *
 ***********************************************************************************************/


#if (CLOCK_SOURCE == 1 || CLOCK_SOURCE == 2)
  inline void _asmDelay(uint16_t us);
  inline void _asmDelay(uint16_t us) {
    __asm__ __volatile__ (
      "1: sbiw %0, 1" "\n\t" // 2 cycles
      "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
    );
  }
  // These two functions need only exist if not using internal clock source.
  void _blinkCode(uint8_t blinkcount);
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

  #ifdef CLKCTRL_CFD_vect
    ISR(CLKCTRL_CFD_vect){
      onClockFailure();
    }
    void __attribute__((weak)) onClockFailure() {
       _blinkCode(4);
    }
  #endif
#endif

/******************************** TIMER INITIALIZATION  *****************************************/
/*                                                                                              *
 * Basic PWM Timer initialization                                                               *
 * Type A timer for millis and/or PWM Type B timer for PWM (not millis)                         *
 *                                                                                              *
 * 8-bit PWM with PER/TOP = 0xFD / 254 for a 255 clock period (not 256). There are two          *
 * important reasons that we want a 255 clock period not 256 as one might expect. If TCA is     *
 * used for millis, we find that 255 is a magical number that makes a bunch of math work out    *
 * exactly with integers. This prevents the rounding errors from manifesting in millis() return *
 * values, which could potentially lead to time not moving solely forward.                      *
 *                                                                                              *
 * For PWM, it is also critical. There are 256 possible inputs to analogWrite() which are       *
 * valid. But if we count to 255, then the pin would have 257 states; HIGH, LOW, and 255 other  *
 * values (there is always one compare value which produces no PWM output, for values of the    *
 * compare match from -0 through TOP). Which end depends on the timer and implementation        *
 * details), giving constant output, while the opposite extreme corresponds to 1 cycle away     *
 * from constant output. Constant output would thus be a 257th option. We would have no way to  *
 * specicfy that with a single byte value passed to analogWrite. The result being that - usually *
 * at one end or the other, the first step was twice as large. The ends, of course are the      *
 * worst place for that. Most implementations that have the full 256 values results in          *
 * analogWrite(pin, 0) setting the pin LOW, while analogWrite(pin, 1) gives not 1/256th duty    *
 * cycle but 2/256ths, or they'll handle that end correctly but analogWrite(pin,255) will set   *
 * the pin HIGH and analogWrite(pin, 254) will give 254/256ths duty cycle. That meas that the   *
 * point where each step already matters the most will start with one double-step, This is      *
 * readily visible if dimming a LED - precisely at the time that each step makes the largest    *
 * apparent difference in brightness.                                                           *
 * Thus, by counting to 254, we have 255 valid compare values, one of which is "Off" and the    *
 * 256 valid analogWrite() values map perfectly to them without any discontinuities.            *
 *                                                                                              *
 * PWM frequency vs clock frequency                                                             *
 * for TCA and TCB timers as configured by the core - the "target" frequency is 1kHz. This is   *
 * the same as the standard boards and is a sensible default: It's fast enough that most things *
 * don't have noticeable flicker/etc, but you can still PWM the gate of a MOSFET gate at that   *
 * speed and get away with it for small MOSFETs with low gate charge. As the frequency gets     *
 * higher, you enter the territory where the pin can't drive the gate hard enough for the FET to *
 * spend most of it's time either ON or OFF, switching losses increase, the MOSFET gets hotter  *
 * In some cases, this may lead to failure of the FET at a far lighter load than it would if    *
 * there was no PWM involved. Other timer, it will merely degrade performance and efficiency.   *
 * Specialized "gate driver" ICs must be userd for high frequency PWM of a MOSFET gate. The gate*
 * charge is largest for higher current FETs. We want users to be abke to connect a FET and not *
 * have problems at the default dfrequency. 1kHz happens to be a pretty safe value.             *
 * Note that there is surprisingly little correlation between physical size of the FET and the  *
 * gate charge - the more important factor is the Rds(on); that combined with the physical size *
 * (hence area available for heat transfer) determines maximum current. Many good fets in SOT23 *
 * packages have Rds(on) as low as much larger ones - and similar gate charge to go with it.    *
 *                                                                                              *
 * The oddball frequencies of 28 MHz and 30 MHz are the most problematic. If the timer is used  *
 * as millis, using /256 prescale results in micros resolution as low as 8 us, which raises     *
 * concerns about timekeeping stability  but with /64, the pwm frequency is uncomfortably high  *
 *                                                                                              *
 * F_PWM = F_CPU/(cycle_length * prescale) = F_CPU/((TOP + 1) * prescale)                       *
 *                                                                                              *
 * For TCAs and TCBs these are:                                                                 *
 *                                                                                              *
 *  F_CPU    Prescale   F_PWM                                                                   *
 * 48 MHz      256     735 Hz                                                                   *
 * 44 MHz      256     674 Hz  // Not exposed, but internally supported by core for some reason *
 * 40 MHz      256     613 Hz                                                                   *
 * 36 MHz      256     551 Hz                                                                   *
 * 32 MHz      256     490 Hz                                                                   *
 * 30 MHz       64    1838 Hz  // Eewww! TOO HIGH! Neither choice here is good!                 *
 * 28 MHz       64    1716 Hz  // Eewww! TOO HIGH! Neither choice here is good!                 *
 * 25 MHz       64    1532 Hz  // Ewwww! It's this or 375 though!                               *
 * 24 MHz       64    1471 Hz  // Higher than I'd like...                                       *
 * 20 MHz       64    1225 Hz                                                                   *
 * 16 MHz       64     980 Hz                                                                   *
 * 12 MHz       64     735 Hz                                                                   *
 * 10 MHz       64     613 Hz                                                                   *
 *  8 MHz       64     490 Hz                                                                   *
 *  5 MHz       16    1225 Hz                                                                   *
 *  4 MHz       16     980 Hz                                                                   *
 *  3 MHz       16     735 Hz  // Unsupported by the core                                       *
 *  2 MHz       16     490 Hz  // Not exposed, but internally supported by core for some reason *
 *  1 MHz        8     490 Hz                                                                   *
 *                                                                                              *
 * Unfortunately there's no 1/128 prescaler option.                                             *
 *                                                                                              *
 * The type D timer has more flexible options for PWM frequency partly because there we can      *
 * increase TOP and analogWrite can leftshift it's argument accordingly.                        *
 * Changing at runtime is supported for TCD and analogWrite()                                   *
 ************************************************************************************************/

uint8_t __PeripheralControl = 0xFF;

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
    #if defined(ERRATA_TCD_PORTMUX) && ERRATA_TCD_PORTMUX == 0 && defined(TCD0_PINS)
      PORTMUX.TCDROUTEA = TCD0_PINS;
    #else
      /* Do nothing - portmux is busted */
      #if (defined(TCD0_PINS) && (TCD0_PINS != 0))
        #error "You are using a variant that specifies alternate portmux value for TCD0. However, the selected part doesn't have available silicon that has that functionality due to outstanding errata."
      #endif
    #endif
  #endif
}

void __attribute__((weak)) init_TCA0() {
  /* TCA0_PINS from pins_arduino.h */
  /* Enable Split Mode to get more PWM pins, since analogWrite() only provides 8-bit PWM anyway*/
  TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

  // Only 1 WGM so no need to specifically set up.

  /* Period setting, 8-bit register in SPLIT mode */
  TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
  TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;

  /* Default duty 0%, will re-assign in analogWrite() */
  // 2/1/2021: Why the heck are we bothering to set these AT ALL?! The duty cycles for *non-active* type A timer channels? Which are already initialized to 0 automatically?
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
   * a defect in the core and would result in extremely bad behavior
   */

  #if (F_CPU > 30000000) // use 256 divider when clocked over 30 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) // use 64 divider unless it's 5 MHz or under
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 64)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 1000000) // anything above 1 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 16)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
  #else /* for 1 MHz and lower */
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

  #if (F_CPU > 30000000) // use 256 divider when clocked over 30 MHz
    #if defined(MILLIS_USE_TIMERA1) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA1 to different divider"
    #endif
    TCA1.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) // use 64 divider unless it's 5 MHz or under
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
/*    TYPE B TIMERS  *
 * Set up routing (defined in pins_arduino.h)
 * Notice how there's no semicolon until after the preprocessor conditionals.
 * In other words, that's all one line.
 */

  PORTMUX.TCBROUTEA = 0
  #if defined(TCB0_PINS)
                        | TCB0_PINS
  #endif
  #if defined(TCB1_PINS)
                        | TCB1_PINS
  #endif
  #if defined(TCB2_PINS)
                        | TCB2_PINS
  #endif
  #if defined(TCB3_PINS)
                        | TCB3_PINS
  #endif
  #if defined(TCB4_PINS)
                        | TCB4_PINS
  #endif
        ;
  // Start with TCB0 - we take advantage of the fact that we can get a pointer
  // to TCB0's struct, and increment it to go through all of them.
  TCB_t *timer_B = (TCB_t *)&TCB0;

  // Find end timer - the highest numbered TCB that is not used for millis.
  // though if we do set up the millis timer because it's in the middle, that's
  // fine and there's no need to skip it.
  #if   defined(TCB4)
    TCB_t *timer_B_end = (TCB_t *) &TCB4;
  #elif defined(TCB3)
    TCB_t *timer_B_end = (TCB_t *) &TCB3;
  #elif defined(TCB2)
    TCB_t *timer_B_end = (TCB_t *) &TCB2;
  #elif defined(TCB1)
    TCB_t *timer_B_end = (TCB_t *) &TCB1;
  #else // Only TCB0 - possible only for DD-series with 14 or 20 pins using TCB1 for millis
    TCB_t *timer_B_end = (TCB_t *) &TCB0;
  #endif

  // Timer B Setup loop for TCB[0:end]
  do {
    #if defined(MILLIS_USE_TIMERB0)
      if(timer_B != (TCB_t *)&TCB0)
    #elif defined(MILLIS_USE_TIMERB1)
      if(timer_B != (TCB_t *)&TCB1)
    #elif defined(MILLIS_USE_TIMERB2)
      if(timer_B != (TCB_t *)&TCB2)
    #elif defined(MILLIS_USE_TIMERB3)
      if(timer_B != (TCB_t *)&TCB3)
    #elif defined(MILLIS_USE_TIMERB4)
      if(timer_B != (TCB_t *)&TCB4)
    #endif
    {
      // 8 bit PWM mode, but do not enable output yet, will do in analogWrite()
      timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

      // Assign 8-bit period
      timer_B->CCMPL = PWM_TIMER_PERIOD; // TOP = 254 see section at start
      // default duty 50% - we have to set something here because of the
      // errata, otherwise CCMP will not get the CCMPL either.
      timer_B->CCMPH = PWM_TIMER_COMPARE;

      // Use TCA clock (250kHz, +/- 50%) and enable
      timer_B->CTRLA = (TCB_CLKSEL_TCA0_gc) | (TCB_ENABLE_bm);

    }
    // Increment pointer to next TCB instance
    timer_B++;
    // Stop when pointing to the last timer.
  } while (timer_B <= timer_B_end);
}

#if (defined(TCD0) && defined(USE_TIMERD0_PWM) && !defined(MILLIS_USE_TIMERD0))
/* TCD0 gets a totally different initialization routine.                                        *
 * Configure TCD0 for PWM in the non-millis case; currently we do not support TCD0 driven millis*
 * on the Dx-series, but we may have to for the DD-series because, particularly in low pincounts*
 * they are highly timer-constrained. With just 1 TCA, 2 TCBs and TCD0 we find ourselves in the *
 * same boat as the tinyAVRs. With the same timer layout as the "golden" 1-series, we shouldn't *
 * be more constrained with what timers can be used. There, we *default* TCD0 because it's the  *
 * least "useful" timer since it is so hard to use and isn't particularly amazing. However on   *
 * Dx-series parts, the type D timer is much more interesting - we have more clock sources to   *
 * choose from, plus the PLL, and at particularly high frequencies, we can afford to burn a few *
 * clocks in analogWrite() to leftshift the compare value if we have set the period to a longer *
 * than default one. This helps us to compensate for the prescaler which is optimized for the   *
 * case of high output frequencies - this is adjustable at runtime and effectively allows us to *
 * get an extra /2, /4, /8 or /16 prescaler (the last two are only supported at high system     *
 * clock speeds, as they're not necessary at lower ones and why slow down analogWrite any more  *
 * than we have to?                                                                             *
 *                                                                                              *
 * TCD0 is a strange timer - most registers can only be changed with it disabled and some even  *
 * require protected writes. It is clearly meant for cases where misconfiguration could have    *
 * catastrophic consequences (BLDC motors destroying themselves, or H-bridges suffering shoot-  *
 * through (where the supply rails are shorted through the two FETs which shouldh't be on at the*
 * same time ever, or power conversion applications where it could output a voltage that would  *
 * destroy everything downstream of it. That's why the advanced event driven functionality is   *
 * there, and why they call it fault detection.                                                 *
 * Wierder still, because it is async (and may be clocked faster than the CPU) the current count*
 * isn't available directly. You have to do a software capture, wait for a few clocks for sync  *
 * I think this delay is 2-3 synchronizer clocks there and 2-3 system clocks back, but the      *
 * timing is never actually described in the datasheet, surprisingly. It is fast enough that a  *
 * polling loop on the status register only runs once or clocked from system clock and sync     *
 * prescaler is 1. Similarly, it cannot be enabled right after disabling it - the ENRDY bit must*
 * be set in the status register. We skip checking this here because, as we do many places the  *
 * initiialization functions assume that the chip starts from a reset condition.                */

void __attribute__((weak)) init_TCD0() {
  #if defined(USE_TIMERD0_PWM) || defined (MILLIS_USE_TIMERD0)
    TCD0.CMPACLR  = 0x0FFF;
    // Match with CMPBCLR clears all outputs. For normal PWM this just needs to be higher than
    // it will count to - so set to maximum (other settings used for non-overlapping PWM and
    // other advanced functionality)
    TCD0.CTRLC    = 0x80;                   // WOD outputs PWM B, WOC outputs PWM A
    // That doesn't need to be that way, but I declare that that's all we support through analogWrite()
    TCD0.CTRLB    = TIMERD0_WGMODE_SETTING;
    // One ramp or dual slope are the only options that are viable to reproduce classic behavior without
    // considerable calculation overhead. We use one-ramp mode
    TCD0.CMPBCLR  = TIMERD0_TOP_SETTING;
    // From timers.h - is equal to (255 * 2^n)-1 where n is an integer value. This lets us lower
    // PWM frequency, or hold it constant and lower the sync prescale, reducing sync delays.

    #if defined(TIMERD0_SET_CLOCK)
      _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (TIMERD0_SET_CLOCK));
    #endif


    TCD0.CTRLA    = TIMERD0_CLOCK_SETTING | TCD_ENABLE_bm;
    // See timers.h for determination
  #endif
}

#endif
