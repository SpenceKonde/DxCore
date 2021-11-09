//*INDENT-OFF* formatting checker doesn't like all the indentation...
#include "Comparator.h"
#include "Arduino.h"

#if defined(AC0_AC_vect)
  #if defined(PORTE)
    AnalogComparator Comparator0(0, AC0, PORTD.PIN2CTRL, PORTE.PIN0CTRL, PORTE.PIN2CTRL, PORTD.PIN6CTRL, PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
  #else
    AnalogComparator Comparator0(0, AC0, PORTD.PIN2CTRL, PORTD.PIN0CTRL, PORTD.PIN0CTRL, PORTD.PIN6CTRL, PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
  #endif
#endif
#if defined(AC1_AC_vect)
  AnalogComparator Comparator1(1, AC1, PORTD.PIN2CTRL, PORTD.PIN3CTRL, PORTD.PIN4CTRL, PORTD.PIN6CTRL, PORTD.PIN5CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
#endif
#if defined(AC2_AC_vect)
  #if defined(PORTE)
    AnalogComparator Comparator2(2, AC2, PORTD.PIN2CTRL, PORTD.PIN4CTRL, PORTE.PIN1CTRL, PORTD.PIN6CTRL, PORTD.PIN7CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
  #else
    AnalogComparator Comparator2(2, AC2, PORTD.PIN2CTRL, PORTD.PIN4CTRL, PORTD.PIN0CTRL, PORTD.PIN6CTRL, PORTD.PIN7CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
  #endif
#endif

// Array for storing ISR function pointers
#if defined(AC2_AC_vect)
  static volatile voidFuncPtr intFuncAC[3];
#elif defined(AC1_AC_vect)
  static volatile voidFuncPtr intFuncAC[2];
#elif defined(AC0_AC_vect)
  static volatile voidFuncPtr intFuncAC[1];
#else
  #error "Unsupported part? This device does not have an analog comparator!"
#endif

/* A few notes on adaptations for megaTinyCore/DxCore
  1. There are two definitions for the comparator objects, depending on whether
  PORTE is defined. If it's not, we still need to feed the constructor something.
  2. The Dx-series only get one shared voltage reference!
  3. Inversion may be desired even when just using a as an event source, with no
  pin output -> disable_invert
  4. Added read() method to get current state.
  5. Added optional (defaults to false) argument to stop(), if true, it re-enables
  the digital input buffers on the pins that were used.
  6. 11/21: cleaned up a few places where we did R/M/W instead of write.
  7. Windowed mode and POWER are not supported. Windowed mode is not worth it
  POWER might be?
  8. Needs a way to disable defining the ISRs.
*/

AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n) { }
//*INDENT-ON*

void AnalogComparator::init() {
  // Set voltage reference
  if (reference != ref::disable) {
    VREF.ACREF = VREF_ALWAYSON_bm | reference;
  } else {
    VREF.ACREF &= ~VREF_ALWAYSON_bm;
  }
  // Spence 1/21: We have 3 ACs sharing one voltage reference.
  // I adapted it to reach out and set the other objects reference property
  // to match when you call init()... not sure if this is ideal,
  // but it seems weird to have to specify it every time you turn one on otherwise you lose the others.
  if (comparator_number != 0) {
    Comparator0.reference = reference;
  }
  if (comparator_number != 1) {
    Comparator1.reference = reference;
  }
  if (comparator_number != 2) {
    Comparator2.reference = reference;
  }
  // Set DACREF
  AC.DACREF = dacref;

  // Set hysteresis and output.
  // Power setting not supported at this time.
  AC.CTRLA = (AC.CTRLA & ~(AC_HYSMODE_gm | AC_OUTEN_bm)) | hysteresis | (output & 0x40);


  // Set inputs
  if (input_p == in_p::in0) {
    IN0_P = PORT_ISC_INPUT_DISABLE_gc;
  } else if (input_p == in_p::in1) {
    IN1_P = PORT_ISC_INPUT_DISABLE_gc;
  } else if (input_p == in_p::in2) {
    IN2_P = PORT_ISC_INPUT_DISABLE_gc;
  } else if (input_p == in_p::in3) {
    IN3_P = PORT_ISC_INPUT_DISABLE_gc;
  }
  if (input_n == in_n::in0) {
    IN0_N = PORT_ISC_INPUT_DISABLE_gc;
  } else if (input_n == in_n::in1) {
    IN1_N = PORT_ISC_INPUT_DISABLE_gc;
  } else if (input_n == in_n::in2) {
    IN2_N = PORT_ISC_INPUT_DISABLE_gc;
  }

  AC.MUXCTRL = output_initval | (AC.MUXCTRL & ~0x3f) | (input_p << 3) | input_n | (output & 0x80);

  // Prepare for output pin swap
  if (output_swap == out::pin_swap) {
    PORTMUX.ACROUTEA = ~(1 << comparator_number) | output_swap;
  }
}

void AnalogComparator::start(bool state) {
  if (state) {
    AC.CTRLA |= AC_ENABLE_bm;
  } else {
    AC.CTRLA &= ~AC_ENABLE_bm;
  }
}

void AnalogComparator::stop(bool restorepins) {
  start(false);
  if (restorepins) {
    if (input_p == in_p::in0) {
      IN0_P = 0;
    } else if (input_p == in_p::in1) {
      IN1_P = 0;
    } else if (input_p == in_p::in2) {
      IN2_P = 0;
    } else if (input_p == in_p::in3) {
      IN3_P = 0;
    }
    if (input_n == in_n::in0) {
      IN0_N = 0;
    } else if (input_n == in_n::in1) {
      IN1_N = 0;
    } else if (input_n == in_n::in2) {
      IN2_N = 0;
    }
  }
}


void AnalogComparator::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  #if !defined(DXCORE)
  AC_INTMODE_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_t)AC_INTMODE_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_t)AC_INTMODE_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_t)AC_INTMODE_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #else
  AC_INTMODE_NORMAL_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #endif
  // Store function pointer
  intFuncAC[comparator_number] = userFunc;


  // Set interrupt trigger and enable interrupt
  #ifdef !defined(DXCORE)
  AC.CTRLA = (AC.CTRLA & ~AC_INTMODE_gm) | intmode ;
  AC.INTCTRL = AC_CMP_bm
  #else
  AC.INTCTRL = intmode | AC_CMP_bm;
  #endif
}

void AnalogComparator::detachInterrupt() {
  // Disable interrupt
  AC.INTCTRL = 0;
}

#ifdef AC0_AC_vect
ISR(AC0_AC_vect) {
  // Run user function
  intFuncAC[0]();

  // Clear flag
  AC0.STATUS = AC_CMPIF_bm;
}
#endif

#ifdef AC1_AC_vect
ISR(AC1_AC_vect) {
  // Run user function
  intFuncAC[1]();

  // Clear flag
  AC1.STATUS = AC_CMPIF_bm;
}
#endif

#ifdef AC2_AC_vect
ISR(AC2_AC_vect) {
  // Run user function
  intFuncAC[2]();

  // Clear flag
  AC2.STATUS = AC_CMPIF_bm;
}
#endif
