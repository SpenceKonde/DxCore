#include "ZCD.h"
#include "Arduino.h"

#if defined(ZCD0_ZCD_vect)
  ZeroCross zcd0(0, ZCD0, PORTD.PIN1CTRL);
#endif
#if defined(ZCD1_ZCD_vect)
  ZeroCross zcd1(1, ZCD1, PORTE.PIN3CTRL);
#endif
#if defined(ZCD2_ZCD_vect)
  ZeroCross zcd2(2, ZCD2, PORTE.PIN7CTRL);
#endif
#if defined(__AVR_DD__)
  ZeroCross zcd3();
#endif


// Array for storing ISR function pointers
#if defined(ZCD2_ZCD_vect)
  static volatile voidFuncPtr intFuncZCD[3];
#elif defined(ZCD1_ZCD_vect)
  static volatile voidFuncPtr intFuncZCD[2];
#elif defined(ZCD0_ZCD_vect) || defined(ZCD3_ZCD_vect)
  static volatile voidFuncPtr intFuncZCD[1];
#else
  #error "No ZCD hardware detected!"
#endif

#if !defined(__AVR_DD__)
ZeroCross::ZeroCross(const uint8_t zero_cross_number, ZCD_t &zcd, register8_t &input_pin)
  : zcd_number(zero_cross_number), ZCD(zcd), INPUT_PIN(input_pin) { }
#else
ZeroCross::ZeroCross() { }
#endif

void ZeroCross::init() {
  // Set input
  #if defined(__AVR_DD__)
  PORTC.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORT_t &output_port = PORTA;
  uint8_t pin_number = PIN7_bm;
  // Set output
  if (output == out::enable) {
    ZCD3.CTRLA = (ZCD.CTRLA & ~out::invert) | out::enable;
    PORTA.DIRSET = pin_number;
  } else if (output == out::invert) {
    ZCD3.CTRLA |= out::enable | out::invert;
    PORTA.DIRSET = pin_number;
  } else if (output == out::disable) {
    ZCD3.CTRLA &= ~out::enable & ~out::invert;
    // output_port.DIRCLR = pin_number;
  }
  #else
  // Prepare for output pin swap
  INPUT_PIN = PORT_ISC_INPUT_DISABLE_gc;
  PORT_t &output_port = PORTA;
  uint8_t pin_number = PIN7_bm;
  if (output_swap == out::pin_swap || output_swap == out::swap_all) {
    output_port = PORTC;
    pin_number = PIN7_bm;
    PORTMUX.ZCDROUTEA = ~(1 << zcd_number) | output_swap;
  }
  // Set output
  if (output == out::enable) {
    ZCD.CTRLA = (ZCD.CTRLA & ~out::invert) | out::enable;
    output_port.DIRSET = pin_number;
  } else if (output == out::invert) {
    ZCD.CTRLA |= out::enable | out::invert;
    output_port.DIRSET = pin_number;
  } else if (output == out::disable) {
    ZCD.CTRLA &= ~out::enable & ~out::invert;
    // output_port.DIRCLR = pin_number;
  }
  #endif
}

bool ZeroCross::have_separate_mux() {
  #if defined(__AVR_DB__) && PROGMEM_SIZE == 0x20000
  // Fixed in Silicon Rev. A5 of AVR128DB only
  return (SYSCFG.REVID >= 0x14);
  #else
  // other parts are still waiting...
  // if they have support for remapping ZCD pins at all (the DD does not, and the EA doesn't even HAVE a ZCD)
  return false;
  #endif
}

void ZeroCross::start(bool state) {
  if (state) {
    ZCD.CTRLA |= ZCD_ENABLE_bm;
  } else {
    ZCD.CTRLA &= ~ZCD_ENABLE_bm;
  }
}

void ZeroCross::stop() {
  start(false);
}

bool ZeroCross::read() {
  return !!(ZCD.STATUS & ZCD_STATE_bm);
}

void ZeroCross::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  ZCD_INTMODE_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = ZCD_INTMODE_RISING_gc;
      break;
    case FALLING:
      intmode = ZCD_INTMODE_FALLING_gc;
      break;
    case CHANGE:
      intmode = ZCD_INTMODE_BOTH_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }

  // Store function pointer
  #ifdef __AVR_DD__
  intFuncZCD[0] = userFunc;
  #else
  intFuncZCD[zcd_number] = userFunc;
  #endif
  // Set interrupt trigger and enable interrupt
  ZCD.INTCTRL = intmode;
}

void ZeroCross::detachInterrupt() {
  // Disable interrupt
  ZCD.INTCTRL = ZCD_INTMODE_NONE_gc;
}

#ifdef ZCD0_ZCD_vect
ISR(ZCD0_ZCD_vect) {
  // Run user function
  intFuncZCD[0]();

  // Clear flag
  ZCD0.STATUS = ZCD_CROSSIF_bm;
}
#endif

#ifdef ZCD1_ZCD_vect
ISR(ZCD1_ZCD_vect) {
  // Run user function
  intFuncZCD[1]();

  // Clear flag
  ZCD1.STATUS = ZCD_CROSSIF_bm;
}
#endif

#ifdef ZCD2_ZCD_vect
ISR(ZCD2_ZCD_vect) {
  // Run user function
  intFuncZCD[2]();

  // Clear flag
  ZCD2.STATUS = ZCD_CROSSIF_bm;
}
#endif

#ifdef ZCD3_ZCD_vect
ISR(ZCD3_ZCD_vect) {
  // Run user function
  intFuncZCD[0]();

  // Clear flag
  ZCD3.STATUS = ZCD_CROSSIF_bm;
}
#endif
