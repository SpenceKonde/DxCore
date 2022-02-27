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

/* Even when millis is off, we should still have access to the clock cycle counting maqcros.
 * That's the only way ww can get time estimates there! */
inline uint16_t clockCyclesPerMicrosecond() {
  return ((F_CPU) / 1000000L);
}

inline unsigned long clockCyclesToMicroseconds(unsigned long cycles) {
  return (cycles / clockCyclesPerMicrosecond());
}

inline unsigned long microsecondsToClockCycles(unsigned long microseconds) {
  return (microseconds * clockCyclesPerMicrosecond());
}

#ifndef MILLIS_USE_TIMERNONE /* If millis is enabled, we need to provide it millis() and micros() */

  #if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) // TCB as millis source does not need fraction
    volatile uint32_t timer_millis = 0; // That's all we need to track here
  #elif !defined(MILLIS_USE_TIMERRTC) // non-RTC non-TCB millis stuff - TCA, or potentially TCD in the future.
    // all of this stuff is not used when the RTC is used as the timekeeping timer, but that never happens on DxCore.
    static uint16_t timer_fract = 0;
    uint16_t fract_inc;
    volatile uint32_t timer_millis = 0;
    #define FRACT_MAX  (1000)
    #define FRACT_INC  (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) % 1000);
    #define MILLIS_INC (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) / 1000);
    volatile uint32_t timer_overflow_count = 0;
  #else // RTC millis stuff
    volatile uint16_t timer_overflow_count = 0;
  #endif

  // overflow count is tracked for all timer options, even the RTC, except for TCBs where it is equal to millis.
  #if !defined(MILLIS_USE_TIMERRTC)
    #if defined(MILLIS_USE_TIMERD0)
      #if !defined(TCD0)
        #error "Selected millis timer, TCD0, does not exist on this part"
      #else
        #error "Selected millis timer, TCD0, is not supported on DxCore as a millis timekeeping source"
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
      #else  // it's not TCB0, TCB1, TCD0, TCA0, TCA1, or RTC
        #error "No millis timer selected, but not disabled - can't happen!".
      #endif
    #else
      #if !defined(TCA1) && defined(MILLIS_USE_TIMERA1)
        #error "Selected millis timer, TCA1 does not exist on this part."
      #endif
    #endif
  #endif // end #if !defined(MILLIS_USE_TIMERRTC)

  // Millis ISR:
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
        timer_millis++; // that's all we need to do!
      #else // if it's 1<Hz, we set the millis timer to only overflow every 2 milliseconds, intentionally sacrificing resolution.
        timer_millis += 2;
      #endif
    #else
      #if !defined(MILLIS_USE_TIMERRTC) // TCA0 or TCD0
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
      // if RTC is used as timer, we only increment the overflow count
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
    #else // timerb
      _timer->INTFLAGS = TCB_CAPT_bm;
    #endif
  }

  unsigned long millis()
  {
    unsigned long m;
    // disable interrupts while we read timer_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer_millis)
    uint8_t oldSREG = SREG;
    cli();
    #if defined(MILLIS_USE_TIMERRTC)
      m = timer_overflow_count;
      if (RTC.INTFLAGS & RTC_OVF_bm) { // there has just been an overflow that hasn't been accounted for by the interrupt
        m++;
      }
      SREG = oldSREG;
      m = (m << 16);
      m += RTC.CNT;
      // now correct for there being 1000ms to the second instead of 1024
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
       * Do this ASAP after disabling interrupts     */
      #if defined(MILLIS_USE_TIMERA0)
        ticks = TCA0.SPLIT.HCNT;
        flags = TCA0.SPLIT.INTFLAGS;
      #elif defined(MILLIS_USE_TIMERA1)
        ticks = TCA1.SPLIT.HCNT;
        flags = TCA1.SPLIT.INTFLAGS;
      #elif defined(MILLIS_USE_TIMERD0)
        TCD0.CTRLE = TCD_SCAPTUREA_bm;
        while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for sync - should be only one iteration of this loop
        flags = TCD0.INTFLAGS;
        ticks = TCD0.CAPTUREA;
      #else
        ticks = _timer->CNT;
        flags = _timer->INTFLAGS;
      #endif // end getting ticks
      /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
       * ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
       * so we shouldn't increment overflows, or interrupts are disabled and have been for a while, so
       * micros isn't expected to work so it doesn't matter.
       * Next, get the current number of overflows
       */
      #if defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0)
        overflows = timer_overflow_count;
      #else
        overflows = timer_millis;
      #endif

      /* Turn interrupts back on, assuming they were on when micros was called. */
      SREG = oldSREG;

      // Check if there was just an overflow which didn't happen after we entered the function
      #if defined(MILLIS_USE_TIMERD0)
        if ((flags & TCD_OVF_bm) && (ticks < 0x1E0)) { // ticks is 0-509  and it doesn't take 29 ticks between when we disabled interrupts and when we grab the values.
          overflows++;
        }
      #elif defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA1)
        ticks = (TIME_TRACKING_TIMER_PERIOD) - ticks;
        if ((flags & TCA_SPLIT_HUNF_bm) && (ticks < 0xF0)) { // ticks is 0-254 and it doesn't take 14 ticks between when we disabled interrupts and when we grab the values.
          overflows++;
        }
      #else // timerb
        #if ((F_CPU <= 1000000))
          if ((flags & TCB_CAPT_bm) && !(ticks & 0xFE00)) { // ticks is 1-1000, so if the 512s place isn't 1, it hasn't happened since we entered function.
            overflows += 2;
          }
        #else
          // here, ticks is F_CPU/2000 so it's trickier to pick something to check against.
          // Why not just do a damned normal compare? that bitwise and saved like 2-3 clocks, and vastly decreased our tolerance to disabled interrupts.
          #define OVERFLOW_THRESHOLD ((F_CPU / 2000) - 500)
          if ((flags & TCB_CAPT_bm) && (ticks < OVERFLOW_THRESHOLD)) {
            overflows++;
          }
        #endif
      #endif

      // Perform division without the division operator to
      #if defined(MILLIS_USE_TIMERD0)
        #error "Timer D is not supported as a millis source on the AVR DA or DB series."
      #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
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
         * appropriately scale the ick count from the prescaler-deprived TCBs. Since many users
         * reconfigure the TCA for advanced PWM, using the TCA-prescaled clock was a non-starter
         * particularly since many parts have only one TCA. But division can be approximated
         * very closely if the divisor is constant using a series of bitshifts and addition/subtraction.
         *
         * The series of shifts was determined numerically with a spreadsheet that calculated the results for
         * each value that could come from the initial round of rightshifts for any combination of
         * bitshifts and provided a number of statistics to select based on. Bacwards time travel must
         * never happenb, or if it does, it must be a shorter backward step than micros runtime - 1 us
         * otherwise delay() will break and timeouts can instantly expire when it is hit. Similarly,
         * one wants to avoid large jumps forward, and cases where more consecutive "actual" times
         * than absolutely necessary return the same value (time should flow at a constant rate).
         * Finally, the artifacts of the calculation that are unavoidable should be distributed uniformly.
         * Undershooting or overshooting 999 endpoint at the counter's maximum value is the usual
         * source of large jumps (at the overflow point) in either direction. Terms should, as much as
         * possible alternate between positive and negative to minimize artifacs.
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
            #if (F_CPU == 48000000UL )
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
            "sbc %B0,r1"    "\n"    // carry - that's why you need a known 0 register!
            : "+r" (ticks));        // Do the rest in C
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
            #if (F_CPU == 40000000UL )
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
            : "+r" (ticks));        // Do the rest in C
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
              // also works at 2MHz, since we use CLKPER for 1MHz vs CLKPER/2 for all others.
          microseconds   = overflows * 1000 + ticks;
        #endif
        #if !((F_CPU == 48000000UL || F_CPU == 36000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || /* multiples of 12           */ \
               F_CPU == 40000000UL || F_CPU == 30000000UL || F_CPU == 20000000UL || F_CPU == 10000000UL || /* multiples of 10           */ \
               F_CPU == 32000000UL || F_CPU == 16000000UL || F_CPU ==  8000000UL || F_CPU ==  4000000UL || /* powers of 2               */ \
               F_CPU ==  2000000UL || F_CPU ==  1000000UL || F_CPU == 25000000UL || F_CPU ==  5000000UL || /* powers of 2 cont, 25, 5   */ \
               F_CPU == 44000000UL || F_CPU == 28000000UL || F_CPU == 14000000UL || F_CPU ==  3000000UL || /* oddball frequencies       */ \
               F_CPU == 27000000UL)&& /* warn fools who messed with the timers.h file too and expected that the core would sort out how */ \
               ((TIME_TRACKING_TIMER_DIVIDER == 2 && TIME_TRACKING_TICKS_PER_OVF == F_CPU/2000) ||/*how to make the timer work correctly*/ \
               (TIME_TRACKING_TIMER_DIVIDER == 1 && (TIME_TRACKING_TICKS_PER_OVF == F_CPU/500 && F_CPU == 1000000)) || \
               (TIME_TRACKING_TIMER_DIVIDER == 2 && (TIME_TRACKING_TICKS_PER_OVF == F_CPU/1000 && F_CPU == 2000000))))  /*  without them implementing it. No such luck  */
          #warning "Millis timer (TCBn) at this frequency and/or configuration unsupported, micros() will return totally bogus values."
        #endif
      #else // TCA
        #if   (F_CPU == 48000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
          /* Please compiler, do the math with uint8_t's not ints! */
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 5) + (((uint8_t) ticks >> (uint8_t) 2) + ((uint8_t) ticks >> (uint8_t) 4) + ((uint8_t) ticks >> (uint8_t) 5));
        #elif (F_CPU == 44000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 6) - (((uint8_t) ticks >> (uint8_t) 3) + ((uint8_t) ticks >> (uint8_t) 4));
        #elif (F_CPU == 40000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 6) + (((uint8_t) ticks >> (uint8_t) 1) - ((uint8_t) ticks >> (uint8_t) 3) + ((uint8_t) ticks >> (uint8_t) 5));
        #elif (F_CPU == 36000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 256)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 7) + (((uint8_t) ticks >> (uint8_t) 3) - ((uint8_t) ticks >> (uint8_t) 6));
        #elif (F_CPU == 30000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 8) + (((uint8_t) ticks >> (uint8_t) 1) + ((uint8_t) ticks >> (uint8_t) 5) + ((uint8_t) ticks >> (uint8_t) 7));
        #elif (F_CPU == 28000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 2) + (((uint8_t) ticks >> (uint8_t) 2) + ((uint8_t) ticks >> (uint8_t) 5));
        #elif (F_CPU == 27000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 2) + (((uint8_t) ticks >> (uint8_t) 1) - ((uint8_t) ticks >> (uint8_t) 3));
        #elif (F_CPU == 25000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 2) + (((uint8_t) ticks >> (uint8_t) 1) + ((uint8_t) ticks >> (uint8_t) 4));
        #elif (F_CPU == 24000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 3) - (((uint8_t) ticks >> (uint8_t) 1) - ((uint8_t) ticks >> (uint8_t) 3) - ((uint8_t) ticks >> (uint8_t) 5) - ((uint8_t) ticks >> (uint8_t) 7));
        #elif (F_CPU == 20000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 3) + (((uint8_t) ticks >> (uint8_t) 2) - ((uint8_t) ticks >> (uint8_t) 4) + ((uint8_t) ticks >> (uint8_t) 6));
        #elif (F_CPU == 12000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 5) + (((uint8_t) ticks >> (uint8_t) 2) + ((uint8_t) ticks >> (uint8_t) 4) + ((uint8_t) ticks >> (uint8_t) 5));
        #elif (F_CPU == 10000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 6) + (((uint8_t) ticks >> (uint8_t) 1) - ((uint8_t) ticks >> (uint8_t) 3) + ((uint8_t) ticks >> (uint8_t) 5));
        #elif (F_CPU ==  5000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 16)
          microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
              + (ticks * 3) + (((uint8_t) ticks >> (uint8_t) 2) - ((uint8_t) ticks >> (uint8_t) 4) + ((uint8_t) ticks >> (uint8_t) 6));
        #else
          #if (TIME_TRACKING_TIMER_DIVIDER % (F_CPU / 1000000))
            #warning "Millis timer (TCAn) at this frequency unsupported, micros() will return bogus values."
          #endif
          microseconds = ((overflows *  clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
                            + (ticks * (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) / TIME_TRACKING_TIMER_PERIOD)));
        #endif
      #endif // end of timer-specific part of micros calculations
      return microseconds;
    }
  #else  // end of non-RTC micros code
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
  #endif
#else    // end of non-MILLIS_USE_TIMERNONE code

  /* Uses should not call millis() or micros() if the core timekeeping has been disabled. Usually, encountering this error either means
   * that they disabled millis earlier for some other sketch, and the preferences were saved with that - or that they are using a library
   * with a dependence on the timekeeping facilities. Sometimes these are meaningful, other times it is only for a feature that isn't
   * being used, or to catch a particular corner case (see: tinyNeoPixel, very end of show() for an example).
   * As of 1.4.0 we provide the stubs below, which we hope is more useful than being told that millis or micros isn't defined.
   */
  unsigned long micros() {
    badCall("micros() is not available because it has been disabled through the tools -> millis()/micros() menu");
    return -1;
  }
  unsigned long millis() {
    badCall("millis() is not available because it has been disabled through the tools -> millis()/micros() menu");
    return -1;
  }
#endif

#if (!(defined(MILLIS_USE_TIMERNONE) || defined(MILLIS_USE_TIMERRTC) || (F_CPU == 7000000L || F_CPU == 14000000)))
  // delay implementation when we do have micros() - we know it won't work at 7 or 14, and those can be generated
  // from internal, and switch logic is in even though micros isn't.
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
#else // delay implementation when we do not
  void _delay_nonconst(unsigned long ms) {
    while (ms > 0) {
      _delay_ms(1);
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
  // the loop takes 2/5 of a microsecond (5 cycles) per iteration
  // so execute it 2.5 times for each microsecond of delay requested.
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

void restart_millis() {
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
    #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1)|| defined(MILLIS_USE_TIMERB2)|| defined(MILLIS_USE_TIMERB3)|| defined(MILLIS_USE_TIMERB4)) // It's a type b timer
      _timer->CTRLB = 0;
    #endif
    init_millis();
  #endif
}

void init_millis() {
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CMPBCLR = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
      TCD0.INTCTRL = 0x01;//enable interrupt
      TCD0.CTRLB = 0x00; // oneramp mode
      TCD0.CTRLC = 0x80;
      TCD0.CTRLA = TIMERD0_PRESCALER|0x01; // set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
      while(RTC.STATUS); // if RTC is currently busy, spin until it's not.
      // to do: add support for RTC timer initialization
      RTC.PER = 0xFFFF;
    #ifdef MILLIS_USE_TIMERRTC_XTAL
      _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x03);
      RTC.CLKSEL = 2; // external crystal
    #else
      _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, 0x02);
      // RTC.CLKSEL = 0; this is the power on value
    #endif
      RTC.INTCTRL = 0x01; // enable overflow interrupt
      RTC.CTRLA = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|RTC_PRESCALER_DIV32_gc);//fire it up, prescale by 32.
    #else // It's a type b timer
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


void set_millis(uint32_t newmillis){
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
    GPR.GPR0 = newmillis; // keeps the compiler from warning about unused parameter, it's a compile error if this is reachable anyway.
  #else
    #if defined(MILLIS_USE_TIMERRTC)
      // timer_overflow_count = newmillis >> 16;
      // millis = 61/64(timer_overflow_count << 16 + RTC.CNT)
      uint16_t temp = (newmillis%61) << 6;
      newmillis = (newmillis/61) << 6;
      temp = temp/61;
      newmillis += temp;
      timer_overflow_count = newmillis >> 16;
      while(RTC.STATUS&RTC_CNTBUSY_bm); // wait if RTC busy
      RTC.CNT = newmillis&0xFFFF;
    #else
      timer_millis = newmillis;
    #endif
  #endif
}
/*
void nudge_millis(uint16_t nudgesize) {
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
    GPR.GPR0 = newmillis; // keeps the compiler from warning about unused parameter, it's a compile error if this is reachable anyway.
  #elif !(defined(MILLIS_USE_TIMERA0)||defined(MILLIS_USE_TIMERA1))

    uint8_t oldSREG=SREG;
    cli();
    timer_millis += nudgesize;
    SREG=oldSREG;
  #else
    badCall("Not yet supported for this timer, only TCBn");
    GPR.GPR0=nudgesize;
  #endif
}

*/

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
     * to compile for something that isn't a supported part so failure is expected). core_devices.h doesn't
     * patch up the group codes, and those should not be used until a toolchain package with the new names
     * is ready for DxCore. This will happen only after ship dates are available for DD-series parts with
     * less than 64k of flash or  major changes/fixes are put in that impact available parts.
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
        _PROTECTED_WRITE(CLKCTRL_XOSCHFCTRLA, (CLKCTRL_SELHF_EXTCLOCK_gc | CLKCTRL_ENABLE_bm));
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
 * fallback insead of halting startup.                                                         *
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
 * higher, you enter the teritory where the pin can't drive the gate hard enough for the FET to *
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
 * The type D timer has more flexible optons for PWM frequency partly because there we can      *
 * increase TOP and analogWrite can leftshift it's argument accordingly.                        *
 * Changing at runtime is supported for TCD and analogWrite()                                   *
 ************************************************************************************************/

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

void __attribute__((weak)) init_TCA0() {
  /* TCA0_PINS from pins_arduino.h */
  /* Enable Split Mode to get more PWM pins, since analogWrite() only provides 8-bit PWM anyway*/
  TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

  // Only 1 WGM is available in split mode

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
  // to TCB09's struct, and increment it to go through all of them.
  TCB_t *timer_B = (TCB_t *)&TCB0;

  // Find end timer - the highest numbered TCB that is not used for millis.
  // though if we do set up the millis timer because it's in the middle, that's
  // fine and there's no need to skip it.
  #if   defined(TCB4) && !defined(MILLIS_USE_TIMERB4)
    TCB_t *timer_B_end = (TCB_t *) &TCB4;
  #elif defined(TCB3) && !defined(MILLIS_USE_TIMERB3)
    TCB_t *timer_B_end = (TCB_t *) &TCB3;
  #elif defined(TCB2) && !defined(MILLIS_USE_TIMERB2)
    TCB_t *timer_B_end = (TCB_t *) &TCB2;
  #elif defined(TCB1) && !defined(MILLIS_USE_TIMERB1)
    TCB_t *timer_B_end = (TCB_t *) &TCB1;
  #else // Only TCB0 - possible only for DD-series with 14 or 20 pins using TCB1 for millis
    TCB_t *timer_B_end = (TCB_t *) &TCB0;
  #endif

  // Timer B Setup loop for TCB[0:end]
  do {
    // 8 bit PWM mode, but do not enable output yet, will do in analogWrite()
    timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

    // Assign 8-bit period
    timer_B->CCMPL = PWM_TIMER_PERIOD; // TOP = 254 see section at start
    // default duty 50% - we have to set something here because of the
    // errata, otherwise CCMP will not get the CCMPL either.
    timer_B->CCMPH = PWM_TIMER_COMPARE;

    // Use TCA clock (250kHz, +/- 50%) and enable
    timer_B->CTRLA = (TCB_CLKSEL_TCA0_gc) | (TCB_ENABLE_bm);

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
