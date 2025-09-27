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
  Modified extensively 2018~2021 by Spence Konde for
  megaTinyCore and DxCore.
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"



inline __attribute__((always_inline)) void check_valid_pin_mode(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    // if (mode == OUTPUT_PULLUP)
      // badArg("OUTPUT_PULLUP is not supported through pinMode due to overhead and impracticality vis-a-vis other pinMode features; use pinConfigure() or pinModeFast()");
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinMode must be INPUT, OUTPUT, or INPUT_PULLUP (these have numeric values of 0, 1, or 2); it was given a constant that was not one of these.");
    }
  }
}


void _pinMode(uint8_t pin, uint8_t mode); // Forward Declaration

// Same purpose as the mess before pinConfigure()

inline __attribute__((always_inline)) void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);         /* generate compile error if a constant that is not a valid pin is used as the pin */
  check_valid_pin_mode(mode);           /* generate compile error if a constant that is not a valid pin mode is used as the mode */
  #if defined(HARDWIRE_INPUT_ONLY)      /* Ugly hack to avoid setting certain pin output */
    if (__builtin_constant_p(pin)) {
      if (pin == HARDWIRE_INPUT_ONLY) {
        if (__builtin_constant_p(mode)) {
          if (mode == OUTPUT) {
            /* If HARDWIRE_INPUT_ONLY is defined, that pin is to be an INPUT only */
            badArg("This pin cannot be set as an output due to a known hardware defect in the Rev. A."); // future hardware will correct this defect;
            return;
          }
        } else {
          if (mode == OUTPUT) {
            return;
          }
        }
      }
    } else {
      if (__builtin_constant_p(mode)) {
        if (mode == OUTPUT) {
          if (pin == HARDWIRE_INPUT_ONLY) return;
        }
      } else {
        if (pin == HARDWIRE_INPUT_ONLY && mode == OUTPUT) return;
      }
    }
  #endif
  _pinMode(pin, mode);
}
void _pinMode(uint8_t pin, uint8_t mode) {
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if ((bit_mask == NOT_A_PIN) || (mode > INPUT_PULLUP)) {
    return;                             /* ignore invalid pins passed at runtime */
  }
  PORT_t *port = digitalPinToPortStruct(pin);
  // if (port == NULL) return;           /* skip this test; if bit_mask isn't NOT_A_PIN, port won't be null - if it is, pins_arduino.h contains errors and we can't expect any digital I/O to work correctly.
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
    } else {                            /* mode == INPUT (no pullup) */
      *pin_ctrl &= ~(PORT_PULLUPEN_bm); /* Disable pull-up */
    }
    SREG = status;                      /* Restore state */
  }
}

/* This turns off PWM, if enabled. It is called automatically on every digitalWrite();
 * This function can end up executing a heckovalotta code for one simple
 * Note that it only operates on the PWM source with priority - TCA > TCD > TCB/DAC
 * the order of the cases here doesn't matter - which one has priority is determined in
 * digitalPinToTimerNow() in wiring_analog.c. That's why it's recommended to make sure
 * that no pin you're about to move the PWM output of a TCA onto is currently outputting
 * PWM. It can also be used from user code (unlike on the stock core). */
void turnOffPWM(uint8_t pin) {
  check_valid_digital_pin(pin);   // Compile error if pin is constant and isn't a pin.
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) return; //this catches any run-time determined pin that isn't a pin.
  uint8_t portnum  = digitalPinToPort(pin);
  #if (defined(TCA1)||defined(TCA0))
    uint8_t offset = 0;

    uint8_t portmux_tca = PORTMUX.TCAROUTEA;
    TCA_t* timer_A = NULL;
    #if defined(TCA1)
      uint8_t threepin = 0;
    #endif
    // It could be a TCA0 or TCA1 mux 0 or 3;
    if (bit_mask < 0x40) {
      #if defined(TCA0)
        if (((portmux_tca & PORTMUX_TCA0_gm) == portnum) && ((__PeripheralControl & TIMERA0))) {
          timer_A = &TCA0;
        }
      #endif
      #if defined(TCA1) && defined(PORTB)
        else if (((__PeripheralControl & TIMERA1))) {
          portmux_tca &= PORTMUX_TCA1_gm;
          #if defined(PORTG) && !defined(__AVR_DA__) // 64 pin DB. All DA's are errata'ed
            if ((portmux_tca == 0x00 && portnum == PB) || (portmux_tca == 0x18 && portnum == PG))
          #elif defined(__AVR_DA__) || defined(__AVR_DB__)
            if (portmux_tca == 0x00 && portnum == PB)
          #endif
          {
            timer_A = &TCA1;
          }
        }
      #endif
    }
    #if defined(TCA1)
      if (timer_A == NULL && ((__PeripheralControl & TIMERA1)) && (bit_mask &0x70)) {
        portmux_tca &= PORTMUX_TCA1_gm;
        #if defined(__AVR_DA__) // DA with less than 48 pins has no TCA1
          if ((portmux_tca == 0x08 && portnum == PC))
        #elif defined(__AVR_DB__) // as above, only the other mapping options (potentially) work.
          if ((portmux_tca == 0x08 && portnum == PC) || (portmux_tca == 0x10 && portnum == PE))
        #elif defined(__AVR_EA__)
          if ((portmux_tca == 0x08 && portnum == PC) || (portmux_tca == 0x20 && portnum == PA) || (portmux_tca == 0x28 && portnum == PD))
        #endif
        {
          timer_A = &TCA1;
          threepin = 1;
        }
      }
    #endif
    if (timer_A != NULL) {
      offset = bit_mask;
      #if defined(TCA1)  //
        if (threepin) {  // and it's a 3-pin map then offset = 0b0xxx0000
          _SWAP(offset); // So swapo to get 0b00000xxx
        }
      #endif

      uint8_t ctrlb = offset;
      if (offset > 0x04) { // if 0b00xx x000
        ctrlb <<= 1;       // we leftshift what we're going to write to CTRLB one bit to get it into high nybble
      }

      uint8_t t = timer_A->SPLIT.CTRLB;
      uint8_t oldSREG = SREG;
      cli();
      t &= ~ctrlb;
      timer_A->SPLIT.CTRLB = t;
      SREG = oldSREG;
      /* Okay, this layout tends towards maximum pervosity. You basically have to treat them as entirely separate timers at this point!
       * PORT | DA | DB | DD | EA | portnum
       *    A | XX | XX | XX | 20 | 0; portmux == 0x20 EA only                    portnux >> 2 == 4
       *    B | 00 | 00 | XX | 00 | 1; portmux == 0    DA, DB, EA 48 pin - 6 pins portmux >> 2 == 0
       *    C | 08 | 08 | XX | 08 | 2 == portmux >> 2; DA, DB, EA 48 pin - 3 pins portmux >> 2 == 2
       *    D | XX | XX | XX | 28 | 3; portmux == 0x28 EA only                    portmux >> 2 == 5
       *    E | 10 | 10 | XX | XX | 4 == portmux >> 2  DB, 48 pin (and DA, maybe) - 3 pins
       *    F | XX | XX | XX | XX | -
       *    G | 18 | 18 | XX | XX | 6 == portmux >> 2; DB, 48 pin (and DA, maybe) - 6 pins
       *
       * PORTG and PORTE do not function on currently available DA hardware.
       *
       * PORTC, PORTA, PORTD, and PORTE are pins 4-6 only. No PORTE except on 64-pin parts.
       *
       * No TCA1 on 32-pin or less DB.
       * No PORTA or PORTD except on EA (it was a newly added mux option) there.
       * No TCA1 on DD at all.
       */
      return;
    }

  #elif defined(TCE0)
    if (bit_mask < 0x10 && (__PeripheralControl & TIMERE0)) {
      uint8_t usetce0 = 0;
      uint8_t tcemux = PORTMUX.TCEROUTEA;
      if (tcemux < 7 ) {
        if (portnum== tcemux && bit_mask < 0x10) {
          usetce0 = 1;
        }
      } /* else if (tcemux == 7) {
      } */ else if (tcemux == 8) {
        if (bit_mask < 4 && (portnum== PC || portnum== PA)) {
          usetce0 = 1;
          if (portnum== PC) {
            bit_mask <<= 2;
          }
        }
      } else if (tcemux == 9) {
        if (portnum== PA && ( bit_mask > 2 && bit_mask < 6)) {
          usetce0 = 1;
          bit_mask >>= 2;
        }
      }
      if (usetce0) {
        _SWAP(bit_mask);
        bit_mask = (~bit_mask) & TCE0_CTRLB;
        TCE0_CTRLB = bit_mask;
        return;
      }
    }
  #endif
  /* Now we use the table in variant */
  uint8_t digital_pin_timer = digitalPinToTimer(pin);
  if (digital_pin_timer) {
    #if defined(TCD0)
      // Dx-series
      if (((digital_pin_timer & 0xC0) == 0x40) && (__PeripheralControl & TIMERD0)) { // TCD
        uint8_t tcdmux = digital_pin_timer & 0x07;
        uint8_t usetcd0 = 0;
        uint8_t muxval = ((PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm) >> PORTMUX_TCD0_gp );
        #if !defined(__AVR_DA__)
          if (tcdmux == muxval)
            usetcd0 = 1;
          #if !(defined(__AVR_DA__) || defined(__AVR_DB__))
            else if (muxval == 4 && tcdmux == 0 && !(digital_pin_timer & 0x20)) {
              usetcd0 = 1;
            }
          #endif
          if (usetcd0)
        #else
          if (tcdmux == 0 && muxval == 0)
        #endif
        {
          uint8_t fc_mask;
          if (digital_pin_timer & 0x20) {
            fc_mask = 0x40;
          } else {
            fc_mask = 0x10;
          }
          if (digital_pin_timer & 0x10) {
            fc_mask <<= 1;
          }
          uint8_t oldSREG = SREG;
          /* Check if channel active, if so, have to turn it off */
          if ((TCD0.FAULTCTRL & fc_mask)) {
            // see remarks in analogWrite()
            uint8_t temp2 = TCD0.CTRLA;
            TCD0.CTRLA = temp2 & (~TCD_ENABLE_bm);
            while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
            _PROTECTED_WRITE(TCD0.FAULTCTRL, ((~fc_mask) & TCD0.FAULTCTRL));
            // while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
            TCD0.CTRLA = temp2; // re-enable it if it was enabled
          } else {
            TCD0.CTRLE = TCD_SYNCEOC_bm; // it was already on - just set new value and set sync flag.
          }
          SREG = oldSREG; // Turn interrupts back on, if they were off.
          return;
        }
      }
    #endif
    if ((digital_pin_timer & 0xF0) == 0x10 ) {
      /* its on a TCB */
      TCB_t * timer_B;
      timer_B = &TCB0 + (digital_pin_timer & 0x07);
      if (((timer_B->CTRLB) & TCB_CNTMODE_gm) == TCB_CNTMODE_PWM8_gc ) {
        /* Disable Timer Output */
        timer_B->CTRLB &= (~TCB_CCMPEN_bm);
        return;
      }
    }
    #if defined(TCF0)
      if ((digital_pin_timer & 0xF0) == TIMERF0) {
        if (((TCF0.CTRLB & 0x07) == 0x07) && ((PORTMUX.TCFROUTEA & PORTMUX_TCF0_gm) == (digital_pin_timer & 0x07))) {
          if (digital_pin_timer & 0x08) {
            TCF0.CTRLC &= 0xFD;
          } else {
            TCF0.CTRLC &= 0xFE;
          }
          return;
        }
      }
    #endif
    #if defined(DAC0)
      if (digital_pin_timer == DACOUT) {
        _setInput(portnum, bit_mask);
        DAC0.CTRLA &= ~0x41; // clear we want to turn off the DAC in this case
      }
    #endif
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
  /* Set output to value
  This now runs even if port set INPUT in order to emulate
  the behavior of digitalWrite() on classic AVR devices, where
  you could digitalWrite() a pin while it's an input, to ensure
  that the value of the port was set correctly when it was
  changed to an output. Code in the wild relies on this behavior. */

  if (val == LOW) { /* If LOW */
    port->OUTCLR = bit_mask;
  } else if (val == CHANGE) { /* If TOGGLE
     * For the pullup setting part below
     * we need to know if it's been set high or low
     * otherwise the pullup state could get out of
     * sync with the output bit. Annoying! But we should
     * have to read it before writing OUTTGL, since that can
     * have a 1 clock delay. So read first + invert */
    val = !(port->OUT & bit_mask);
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
  /* Turn off PWM if applicable
   * If the pin supports PWM output, we need to turn it off.
   * Better to do so AFTER we have set PORTx.OUT to what we
   * want it to be when we're done. The glitch would be short
   * (though non-negligible since all these functions are, of
   * course, slow - the worst case being a TCD pin currently
   * analogWritten() 255, then digitallyWritten() to HIGH, which
   * would turn it off for the time between turnOffPWM() and
   * PORT->OUTCLR)
   * Since there's no penalty, why make a glitch we don't have to? */
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void digitalWriteFast(uint8_t pin, uint8_t val) {
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
  // Originally the Arduino core this was derived from turned off PWM on the pin
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


inline __attribute__((always_inline)) int8_t digitalReadFast(uint8_t pin) {
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


void openDrain(uint8_t pin, uint8_t state) {
  check_valid_digital_pin(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN)  return;
  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);
  port->OUTCLR = bit_mask;
  if (state == LOW)
    port->DIRSET = bit_mask;
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
  if (pin == NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  PORT_t *portstr;
  portstr = (PORT_t *)(0x400 + (0x20 * port));

  if (val == LOW)
    vport->DIR |= mask;
  else if (val == CHANGE)
    portstr->DIRTGL = mask;
  else// FLOAT
    vport->DIR &= ~mask;
}

inline __attribute__((always_inline)) void pinModeFast(uint8_t pin, uint8_t mode) {
  check_constant_pin(pin);
  if (!__builtin_constant_p(mode)) {
    badArg("mode must be constant when used with pinModeFast");
  } else {
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinModeFast must be INPUT, OUTPUT, INPUT_PULLUP");// or OUTPUT_PULLUP");
    }
  }
  check_valid_digital_pin(pin);         // generate compile error if a constant that is not a valid pin is used as the pin
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  volatile uint8_t *pin_ctrl = (volatile uint8_t *) (0x410 + digital_pin_to_port[pin] * 0x20 + digital_pin_to_bit_position[pin]);
  if (mode == OUTPUT) {
    vport->DIR |= mask;
  } else {
    vport->DIR &= ~mask;
    // 1 clock to set direction
  }
  if (mode == INPUT_PULLUP) {
    *pin_ctrl |= PORT_PULLUPEN_bm;
  } else if (mode == INPUT) {
    *pin_ctrl &= ~PORT_PULLUPEN_bm;
    // and 5 to switch the damned pullup.
  }
}
