#ifndef __TIMERS_H__
#define __TIMERS_H__

/* For megaTinyCore only */
#ifdef MILLIS_USE_TIMERD0_A0
  #ifdef TCD0
    #define MILLIS_USE_TIMERD0
  #else
    #define MILLIS_USE_TIMERA0
  #endif
#endif


#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2)|| defined(MILLIS_USE_TIMERB3)|| defined(MILLIS_USE_TIMERB4))
  #if (F_CPU==1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   1
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/500)-1)
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   2
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/2000)-1)
  #endif
#elif defined(MILLIS_USE_TIMERD0) || (defined(MILLIS_USE_TIMERD0_A0) && defined(TCD0))
/* megaTinyCore only - because of the better HF internal oscillator on DxCore
 * we generally would not use a prescaler... but if we were going to use TCD for millis
 * running at F_CPU, we'd need to lower the prescaler to maintain resolution, which
 * brings us back to the question of whether to use higher clock speed and prescale or
 * not. As the Type D timer is more useful on AVR Dx parts, and Type B timers more
 * abundant such that rarely will users be unwilling to use one for millis, TCD0 is
 * NOT SUPPORTED AS A MILLIS TIMING SOURCE on DxCore
 *
 *  #define TIME_TRACKING_TIMER_PERIOD    0x1FD
 *  #if (F_CPU==1000000UL)
 *   #define TIME_TRACKING_TIMER_DIVIDER    64
 *  #else
 *    #define TIME_TRACKING_TIMER_DIVIDER   32
 *  #endif
 */
  #error "TCD0 is not supported as a millis timing source on DxCore."
#else //Otherwise it must be a TCA
  #define TIME_TRACKING_TIMER_PERIOD    0xFE

  #if     (F_CPU > 25000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   256
  #elif   (F_CPU <= 5000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   16
  #elif   (F_CPU <= 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   8
  #else // Almost always this one:
        // 25000000UL >= F_CPU > 5000000
    #define TIME_TRACKING_TIMER_DIVIDER   64
  #endif
#endif



#if (F_CPU == 1000000 || F_CPU == 20000000)
  #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc|TCD_SYNCPRES_DIV1_gc|TCD_CLKSEL_CLKPER_gc)
#elif (F_CPU == 5000000 || F_CPU == 10000000)
  #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV32_gc|TCD_SYNCPRES_DIV1_gc|TCD_CLKSEL_OSCHF_gc)
#else
  #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV32_gc|TCD_SYNCPRES_DIV1_gc|TCD_CLKSEL_CLKPER_gc)
#endif

#if (F_CPU == 1000000 || F_CPU == 20000000 || F_CPU == 40000000 || F_CPU == 80000000)
  #define TIMERD0_TOP_SETTING   (254)
#elif (F_CPU > 20000000)
  #define TIMERD0_TOP_SETTING   (1019)
#else
  #define TIMERD0_TOP_SETTING   (509)
#endif


//#endif

#define TIME_TRACKING_TICKS_PER_OVF   (TIME_TRACKING_TIMER_PERIOD + 1)
#define TIME_TRACKING_CYCLES_PER_OVF  (TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)

//For a type B timer as millis, these #defines aren't needed!

// However PWM_TIMER_PERIOD _does_ apply to a type B timer as PWM, or a type A timer not doing millis
// in practice, the period for TCA doing millis or just PWM is the same (as is the divistor), it is
// not set in stone, only entered in a header.

#define PWM_TIMER_PERIOD  0xFE  /* For frequency */
#define PWM_TIMER_COMPARE 0x80  /* For duty cycle */

#if defined(MILLIS_USE_TIMERA0)
  #define MILLIS_TIMER TIMERA0
#elif defined(MILLIS_USE_TIMERA1)
  #define MILLIS_TIMER TIMERA1
#elif defined(MILLIS_USE_TIMERB0)
  #define MILLIS_TIMER TIMERB0
#elif defined(MILLIS_USE_TIMERB1)
  #define MILLIS_TIMER TIMERB1
#elif defined(MILLIS_USE_TIMERB2)
  #define MILLIS_TIMER TIMERB2
#elif defined(MILLIS_USE_TIMERB3)
  #define MILLIS_TIMER TIMERB3
#elif defined(MILLIS_USE_TIMERB4)
  #define MILLIS_TIMER TIMERB4
#elif defined(MILLIS_USE_TIMERD0)
  #define MILLIS_TIMER TIMERD0
#elif defined(MILLIS_USE_TIMERRTC)
  #define MILLIS_TIMER TIMERRTC
#else
  #define MILLIS_TIMER NOT_ON_TIMER
#endif

#endif
