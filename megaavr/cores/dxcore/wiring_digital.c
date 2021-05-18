/*
  wiring_digital.c - digital input and output functions
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

  Modified 28 September 2010 by Mark Sproul
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"



inline __attribute__((always_inline)) void check_valid_digital_pin(uint8_t pin)
{
  if(__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN) {
      // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
      // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
      // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
      // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      if (pin & 0x80) {
        badArg("Digital pin is constant, but not a valid pin - it is > 0x80 and not NOT_A_PIN. At present, this function only accepts digital pin numbers, NOT analog channel numbers.");
      } else {
        badArg("Digital pin is constant, but not a valid pin");
      }
    }
  }
}

inline __attribute__((always_inline)) void check_valid_pin_mode(uint8_t mode) {
  if(__builtin_constant_p(mode)) {
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinMode must be INPUT, OUTPUT, or INPUT_PULLUP (these have numeric values of 0, 1, or 2); it was given a constant that was not one of these.");
    }
  }
}



void pinConfigure(uint8_t pin, uint16_t pinconfig) {
  check_valid_digital_pin(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return;                             /* ignore invalid pins passed at runtime */
  }
  volatile uint8_t *portbase = (volatile uint8_t*) digitalPinToPortStruct(pin);
  uint8_t bit_pos = digitalPinToBitPosition(pin);
  uint8_t setting = pinconfig & 0x03; //grab direction bits
  if (setting) {
    *(portbase + setting) = bit_mask;
  }
  pinconfig >>= 2;
  setting = pinconfig & 0x03; // as above, only for output
  if (setting) {
    *(portbase + 4 + setting) = bit_mask;
  }
  if (!(pinconfig & 0x03FFC)) return;
  pinconfig >>= 2;
  uint8_t oldSREG = SREG;
  cli();
  uint8_t pinncfg = *(portbase + 0x10 + bit_pos);
  if (pinconfig & 0x08 ) {
    pinncfg = (pinncfg & 0xF8 ) | (pinconfig & 0x07);
  }
  uint8_t temp = pinconfig & 0x30;
  if (temp) {
    if (temp == 0x30) {
      pinncfg ^= 0x08;    // toggle pullup - of dubious utility
    } else if (temp == 0x20) {
      pinncfg &= ~(0x08); // clear
    } else {
      pinncfg |= 0x08;    // set
    }
  }
  pinconfig >>= 8; // now it's just the last 4 bits.
  #ifdef MVIO // only MVIO parts have the TTL levels.
  // Their utility in a mixed voltage system is obvious: when you run out
  // of MV pins, you can still use lower voltage lines as inputs, or drive them open drain
  // but not if you can't read them reliably. The schmitt trigger is only guaranteed to read
  // high above 0.8*Vcc, so even 3.3v logic (which usually *does* work) isn't in spec!
  temp = pinconfig & 0x03;
  if (temp) {
    if (temp == 0x01) {
      pinncfg |= 0x40; // set
    } else {
      pinncfg &= ~(0x40);   // clear - toggle not supported. I question the usefulness of the other PINnCTRL toggles,
      // but I am more confident here than in those cases, that if you want a toggle INLVL option, you're doing something
      // sufficiently wrong that it is more helpful to not give you that tool than help you go further into the weeds.
    }
  }
  #endif
  temp = pinconfig & 0x0C;
  if (temp) {
    if (temp == 0x0C) {
      pinncfg ^= 0x80;    // toggle invert - of dubious utility
    } else if (temp == 0x08) {
      pinncfg &= ~(0x80); // clear
    } else {
      pinncfg |= 0x80;    // set
    }
  }
  *(portbase + 0x10 + bit_pos)=pinncfg;
  SREG=oldSREG; //re-enable interrupts
}

void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);         /* generate compile error if a constant that is not a valid pin is used as the pin */
  check_valid_pin_mode(mode);           /* generate compile error if a constant that is not a valid pin mode is used as the mode */
  uint8_t bit_mask = digitalPinToBitMask(pin);

  if ((bit_mask == NOT_A_PIN) || (mode > INPUT_PULLUP)) {
    return;                             /* ignore invalid pins passed at runtime */
  }

  PORT_t *port = digitalPinToPortStruct(pin);
  //if (port == NULL) return;           /* skip this test; if bit_mask isn't NOT_A_PIN, port won't be null.

  if (mode == OUTPUT) {
    port->DIRSET = bit_mask;            /* Configure direction as output and done*/
  } else {                              /* mode == INPUT or INPUT_PULLUP - more complicated */
                                        /* Calculate where pin control register is */
    uint8_t bit_pos = digitalPinToBitPosition(pin);
    volatile uint8_t *pin_ctrl = getPINnCTRLregister(port, bit_pos);
    uint8_t status = SREG;              /* Save state */
    cli();                              /* Interrupts off for PINnCTRL stuff */
    port->DIRCLR = bit_mask;            /* Configure direction as input */
    if (mode == INPUT_PULLUP) {         /* Configure pull-up resistor */
      *pin_ctrl |= PORT_PULLUPEN_bm;    /* Enable pull-up */
      port->OUTSET = bit_mask;          /* emulate setting of the port output register on classic AVR */
    } else {                            /* mode == INPUT (no pullup) */
      *pin_ctrl &= ~(PORT_PULLUPEN_bm); /* Disable pull-up */
      port->OUTCLR = bit_mask;          /* emulate clearing of the port output register on classic AVR */
    }
    SREG = status;                      /* Restore state */
  }
}

/* This turns off PWM, if enabled. It is called automatically on every digitalWrite();
 * Note that it only operates on the PWM source with priority - TCA > TCD > TCB/DAC
 * the order of the cases here doesn't matter - which one has priority is determined in
 * digitalPinToTimerNow() in wiring_analog.c. That's why it's recommended to make sure
 * that no pin you're about to move the PWM output of a TCA onto is currently outputting
 * PWM. It can also be used from user code (unlike on the stock core). */
void turnOffPWM(uint8_t pin)
{
  /* Actually turn off compare channel, not the timer */

  /* Get pin's timer */
  uint8_t timer = digitalPinToTimerNow(pin);
  if(timer == NOT_ON_TIMER) return;

  //uint8_t bit_pos = digitalPinToBitPosition(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  // we know is valid because we were told it was a timer above.
  TCB_t *timerB;

  switch (timer) {

  /* TCA0 */
  case TIMERA0:
    /* Bit position will give output channel */
    if (bit_mask > 0x04)  bit_mask <<= 1; //there's a blank bit in the middle
    /* Disable corresponding channel */
    TCA0.SPLIT.CTRLB &= ~bit_mask;
    break;
  #ifdef TCA1
  case TIMERA1:
    /* Bit position will give output channel */
    if (bit_mask > 0x04)  bit_mask <<= 1; //there's a blank bit in the middle
    /* Disable corresponding channel */
    TCA1.SPLIT.CTRLB &= ~bit_mask;
    break;
  #endif
  //TCB - only one output
  case TIMERB0:
  case TIMERB1:
  case TIMERB2:
  case TIMERB3:
  case TIMERB4:

    timerB = (TCB_t *) &TCB0 + (timer - TIMERB0);

     //Disable TCB compare channel
    timerB->CTRLB &= ~(TCB_CCMPEN_bm);

    break;
  #if defined(DAC0)
  case DACOUT:
    DAC0.CTRLA = 0x00;
    break;
  #endif
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
    case TIMERD0:
    {
      // rigmarole that produces a glitch in the PWM
      #ifdef MEGATINYCORE
        // it's either bit 6 or 7 - it's the CMPC and CMPD channels we use; A and B are on pins that we can already cover with TCA0.
        uint8_t fcset = TCD0.FAULTCTRL & (bit_mask == 0x02 ? 0x80 : 0x40 );
      #else
        // on the DA series, it could be any of them
        #ifndef ERRATA_TCD_PORTMUX
          uint8_t fcset = TCD0.FAULTCTRL & (bit_mask > 0x0F ? bit_mask : bit_mask << 4 ); //hopefully that gets rendereed as swap, not 4 leftshifts
        #else
          uint8_t fcset = TCD0.FAULTCTRL & bit_mask;
        #endif
      #endif
      if (fcset) {
        // don't do any of this unless the pin is currently set to output PWM - spamming digital I/O on a pin that could output PWM shouldn't
        // cause TCD0 to lose a couple of clocks of timing each time.
        #if defined (NO_GLITCH_TIMERD0)
          // Arrgh, almost didn't need bit position!
          volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(digitalPinToPortStruct(pin), digitalPinToBitMask(pin));
          // at least get it before disablling interrupts.
        #endif
        uint8_t oldSREG = SREG;
        cli();
        TCD0.CTRLA &= ~TCD_ENABLE_bm; // stop the timer
        // This is slightly dangerous - if the timer isn't running, it will start the timer. But it will only do any of that if
        // it was currently set to output PWM, so it's very hard to imagine triggering it with just innocent calls to digitalWrite
        // in a constructor - we do not promise core functions will behave if users are reconfiguring peripherals in arbitrary ways.
        _PROTECTED_WRITE(TCD0.FAULTCTRL,TCD0.FAULTCTRL & ~fcset);
        while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until we can restart it
        TCD0.CTRLA |= TCD_ENABLE_bm;  // re-enable it
        #if defined(NO_GLITCH_TIMERD0)
          *pin_ctrl_reg &= ~(PORT_INVEN_bm);
        #endif
        SREG = oldSREG;
      }
      break;
    }
  #endif
  default:
    break;
  }
}

void digitalWrite(uint8_t pin, uint8_t val) {
  check_valid_digital_pin(pin);
  /* Get bit mask for pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return;
  }

  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);


  /*
  Set output to value
  This now runs even if port set INPUT in order to emulate
  the behavior of digitalWrite() on classic AVR devices, where
  you could digitalWrite() a pin while it's an input, to ensure
  that the value of the port was set correctly when it was
  changed to an output. Code in the wild relies on this behavior.
  */

  if (val == LOW) { /* If LOW */
    port->OUTCLR = bit_mask;
  } else if (val == CHANGE) { /* If TOGGLE */
    port->OUTTGL = bit_mask;
    // Now, for the pullup setting part below
    // we need to know if it's been set high or low
    // otherwise the pullup state could get out of
    // sync with the output bit. Annoying!
    val = port->OUT & bit_mask;
    // val will now be 0 (LOW) if the toggling made it LOW
    // or bit_mask if not. And further down, we only need to
    // know if it's
  /* If HIGH OR  > TOGGLE  */
  } else {
    port->OUTSET = bit_mask;
  }

  /* Input direction */
  if (!(port->DIR & bit_mask)) {
    /* Old implementation has side effect when pin set as input -
      pull up is enabled if this function is called.
      Should we purposely implement this side effect?
    */

    /* Get bit position for getting pin ctrl reg */
    uint8_t bit_pos = digitalPinToBitPosition(pin);

    /* Calculate where pin control register is */
    volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Save system status and disable interrupts */
    uint8_t status = SREG;
    cli();

    if (val == LOW) {
      /* Disable pullup */
      *pin_ctrl_reg &= ~PORT_PULLUPEN_bm;
    } else {
      /* Enable pull-up */
      *pin_ctrl_reg |= PORT_PULLUPEN_bm;
    }

    /* Restore system status */
    SREG = status;
  }
  /* Turn off PWM if applicable */
  // If the pin supports PWM output, we need to turn it off
  // Better to do so AFTER we have set PORTx.OUT to what we want it to be when we're done
  // The glitch would be super short, of course, but why make a glitch we don't have to?
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void digitalWriteFast(uint8_t pin, uint8_t val)
{
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  if (pin == NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Write pin value from VPORTx.OUT register
  vport = (VPORT_t *)(port * 4);
/*
  if (val == HIGH)
    vport->OUT |= mask;
  else if (val == LOW)
    vport->OUT &= ~mask;
  else // CHANGE
    vport->IN = mask;
*/
  if (val == LOW)
    vport->OUT &= ~mask;
  else if (val == CHANGE)
    vport->IN |= mask;
  else // HIGH
    vport->OUT |= mask;

}

int8_t digitalRead(uint8_t pin) {
  check_valid_digital_pin(pin);
  /* Get bit mask and check valid pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return -1;
  }
  // Origionbally the Arduino core this was derived from turned off PWM on the pin
  // I cannot fathom why, insofar as the Arduino team sees Arduino as an educational
  // tool, and I can't think of a better way to learn about PWM...
  //
  // More importantkly, digitialRead() already *crawls*. Hence there is a particularly
  // high bar for stuff that would make it even slower than it already is.
  //
  // turnOffPWM(pin);

  /* Get port and check valid port */
  PORT_t *port = digitalPinToPortStruct(pin);

  /* Read pin value from PORTx.IN register */
  if (port->IN & bit_mask) {
    return HIGH;
  } else {
    return LOW;
  }
}


inline __attribute__((always_inline)) int8_t digitalReadFast(uint8_t pin)
{
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Old style port logic is a small integer 0 for PORTA, 1 for PORTB etc.
  vport = (VPORT_t *)(port * 4);

  // Read pin value from VPORTx.IN register
  return !!(vport->IN & mask);
}

void openDrain(uint8_t pin, uint8_t state){
  check_valid_digital_pin(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN)  return;
  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);
  port->OUTCLR=bit_mask;
  if (state == LOW)
    port->DIRSET=bit_mask;
  else if (state == CHANGE)
    port->DIRTGL=bit_mask;
  else // assume FLOATING
    port->DIRCLR=bit_mask;
  turnOffPWM(pin);
  }

inline __attribute__((always_inline)) void openDrainFast(uint8_t pin, uint8_t val)
{
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  if (pin==NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  PORT_t *portstr;
  portstr=(PORT_t *)(0x400+(20*port));

  if (val == LOW) {
    vport->OUT &= ~mask;
    vport->DIR |= mask;
  } else if (val==CHANGE) {
    vport->OUT &= ~mask;
    portstr->DIRTGL = mask;
  } else// FLOAT
    vport->DIR &= ~mask;
}
