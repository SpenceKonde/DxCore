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


// Array for storing ISR function pointers
#if defined(ZCD2_ZCD_vect)
static volatile voidFuncPtr intFuncAC[3];
#elif defined(ZCD1_ZCD_vect)
static volatile voidFuncPtr intFuncAC[2];
#elif defined(ZCD0_ZCD_vect)
static volatile voidFuncPtr intFuncAC[1];
#else
#error target does not have zero-cross detection hardware!
#endif


ZeroCross::ZeroCross(const uint8_t zero_cross_number, ZCD_t& zcd, register8_t& input_pin)
  : zcd_number(zero_cross_number), ZCD(zcd), INPUT_PIN(input_pin) { }

void ZeroCross::init()
{
  // Set input
  INPUT_PIN = PORT_ISC_INPUT_DISABLE_gc;

  // Prepare for output pin swap
  PORT_t& output_port = PORTA;
  uint8_t pin_number = PIN7_bm;
  if(output_swap == out::pin_swap || output_swap == out::swap_all)
  {
    output_port = PORTC;
    pin_number = PIN7_bm;
    PORTMUX.ZCDROUTEA = ~(1 << zcd_number) | output_swap;
  }

  // Set output
  if(output == out::enable)
  {
    ZCD.CTRLA = (ZCD.CTRLA & ~out::invert) | out::enable;
    output_port.DIRSET = pin_number;
  }
  else if(output == out::invert)
  {
    ZCD.CTRLA |= out::enable | out::invert;
    output_port.DIRSET = pin_number;
  }
  else if(output == out::disable)
  {
    ZCD.CTRLA &= ~out::enable & ~out::invert;
    output_port.DIRCLR = pin_number;
  }
}

bool ZeroCross::have_separate_mux() {
  #if (defined(__AVR_DB__) && PROGMEM_SIZE==0x20000)
    // Fixed in Silicon Rev. A5 of AVR128DB only
    return (SYSCFG.REVID >= 0x14);
  #else
    // other parts are still waiting...
    return false;
  #endif
}

void ZeroCross::start(bool state)
{
  if(state)
    ZCD.CTRLA |= ZCD_ENABLE_bm;
  else
    ZCD.CTRLA &= ~ZCD_ENABLE_bm;
}

void ZeroCross::stop()
{
  start(false);
}

bool ZeroCross::read()
{
  return !!(ZCD.STATUS & ZCD_STATE_bm);
}

void ZeroCross::attachInterrupt(void (*userFunc)(void), uint8_t mode)
{
  ZCD_INTMODE_t intmode;
  switch (mode)
  {
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
  intFuncAC[zcd_number] = userFunc;

  // Set interrupt trigger and enable interrupt
  ZCD.INTCTRL = intmode;
}

void ZeroCross::detachInterrupt()
{
  // Disable interrupt
  ZCD.INTCTRL = ZCD_INTMODE_NONE_gc;
}

#ifdef ZCD0_ZCD_vect
ISR(ZCD0_ZCD_vect)
{
  // Run user function
  intFuncAC[0]();

  // Clear flag
  ZCD0.STATUS = ZCD_CROSSIF_bm;
}
#endif

#ifdef ZCD1_ZCD_vect
ISR(ZCD1_ZCD_vect)
{
  // Run user function
  intFuncAC[1]();

  // Clear flag
  ZCD1.STATUS = ZCD_CROSSIF_bm;
}
#endif

#ifdef ZCD2_ZCD_vect
ISR(ZCD2_ZCD_vect)
{
  // Run user function
  intFuncAC[2]();

  // Clear flag
  ZCD2.STATUS = ZCD_CROSSIF_bm;
}
#endif
