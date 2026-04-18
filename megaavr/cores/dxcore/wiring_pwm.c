/* wiring_pwm.c
 *
 * why did we have PWM stuff mixed with ADC stuff in a file named analog?
 *
 *
 *
 **********************************************************************************************/

#include "wiring_private.h"
#include "pins_arduino.h"

//#define TCECOND (true)



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
    if ((__PeripheralControl & TIMERE0)&& (bit_mask < 0x10)) {
      uint8_t usetce0 = 0;
      uint8_t tcemux = PORTMUX.TCEROUTEA;
      if (tcemux < 7 ) {
        if (portnum == tcemux) {
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
        if (portnum== PA &&  bit_mask > 2) {
          usetce0 = 1;
          bit_mask >>= 2;
        }
      }
      if (usetce0) {
        if (bit_mask < 0x10) {
          _SWAP(bit_mask);
        }
        usetce0 = TCE0.CTRLB;
        usetce0 &= 0x0F;
        usetce0 |= bit_mask;
        TCE0_CTRLB = usetce0;
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
        uint8_t muxval = ((PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm) >> PORTMUX_TCD0_gp );
        #if !defined(__AVR_DA__)
          uint8_t usetcd0 = 0;
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
        uint8_t ctrla = DAC0.CTRLA;
        if (!(ctrla & 0x01)) {
          // DAC off already, done
          return;
        }
        ctrla &= ~0x41;
        DAC0.CTRLA = ctrla;
        // Now what does other PWM do when it turns off a timer?
        // analogWrite(pin,100); TurnOffPWM(pin);
        // leads to a pin that is usually set output
        // and which can detect it's input levels.
        PORTD.PIN6CTRL &= 0xF0; // re-enable input
        VPORTD.DIR |= 0x40; //set output


      }
    #endif
  }
}


#if defined(TCA0)
  void takeOverTCA0() {
    #if defined(MILLIS_USE_TIMERA0)
      stop_millis();
    #endif
    TCA0.SPLIT.CTRLA = 0;          // Stop TCA0
    __PeripheralControl &= ~TIMERA0; // Mark timer as user controlled
    TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA0
    /* Okay, seriously? The datasheets and io headers disagree here for tinyAVR
       about whether the low bits even exist! Much less whether they need to be
       set - but if they are not set, it will not work */
  }

  void resumeTCA0() {
    TCA0.SPLIT.CTRLA = 0;         // Stop TCA0
    TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA0
    init_TCA0();                  // reinitialize TCA0
    __PeripheralControl |= TIMERA0; // Mark timer as core controlled
    #if defined(MILLIS_USE_TIMERA0)
      restart_millis();              // If we stopped millis for takeover, restart
    #endif
  }
#endif
#if defined(TCA1)
  void takeOverTCA1() {
    #if defined(MILLIS_USE_TIMERA1)
      stop_millis();
    #endif
    TCA1.SPLIT.CTRLA = 0;               // Stop TCA1
    __PeripheralControl &= ~TIMERA1;      // Mark timer as user controlled
    TCA1.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA1
  }

  void resumeTCA1() {
    TCA1.SPLIT.CTRLA = 0;         // Stop TCA1
    TCA1.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA1
    init_TCA1();                  // reinitialize TCA1
    __PeripheralControl |= TIMERA1; // Mark timer as core controlled
    #if defined(MILLIS_USE_TIMERA1)
      restart_millis();              // If we stopped millis for takeover, restart
    #endif
  }
#endif
#if defined(TCD0)
  void takeOverTCD0() {
  #if !defined(MILLIS_USE_TIMERD0)
    TCD0.CTRLA = 0;                     // Stop TCD0
    _PROTECTED_WRITE(TCD0.FAULTCTRL,0); // Turn off all outputs
    __PeripheralControl &= ~TIMERD0;      // Mark timer as user controlled
  #else
    badCall("TCD0 takeover not permitted when TCD0 is millis source");
    /* Note that it's just TCD0 we protect like this... With TCA's, the user has
       a much better chance of being able to put it back together with the
       millis() control functions. */
  #endif
}

void resumeTCD0() {
  badCall("Resuming core control of type D timer not supported.");
}
#endif


#if defined(TCE0)
  void takeOverTCE0() {
    #if defined(MILLIS_USE_TIMERA0)
      stop_millis();
    #endif
    TCE0.CTRLA = 0;          // Stop TCA0
    __PeripheralControl &= ~TIMERE0; // Mark timer as user controlled
    TCE0.CTRLESET = TCE_CMD_RESET_gc | 0x03; // Reset TCA0
    /* Okay, seriously? The datasheets and io headers disagree here for tinyAVR
       about whether the low bits even exist! Much less whether they need to be
       set - but if they are not set, it will not work */
  }

  void resumeTCE0() {
    TCE0.CTRLA = 0;         // Stop TCA0
    TCE0.CTRLESET = TCE_CMD_RESET_gc | 0x03; // Reset TCA0
    init_TCE0();                  // reinitialize TCA0
    __PeripheralControl |= TIMERE0; // Mark timer as core controlled
    #if defined(MILLIS_USE_TIMERE0)
      restart_millis();              // If we stopped millis for takeover, restart
    #endif
  }
#endif



inline __attribute__((always_inline)) void check_valid_duty_cycle(int16_t val) {
  if (__builtin_constant_p(val)) {
    if (val < 0 || val >255)
      badArg("analogWrite cannot produice duty cycle higher than 100% or below 0%");
  }
}


uint8_t digitalPinToTimerNow(uint8_t p) {
  uint8_t bit_pos = digitalPinToBitPosition(p);
  if (bit_pos == NOT_A_PIN) return NOT_ON_TIMER;     /* Use bit position to check for invalid pins */
  uint8_t portnum= digitalPinToPort(p);                /* If bit_pos is valid, port will be too      */
  #if defined(TCA0)
    uint8_t tcamux = PORTMUX.TCAROUTEA;
    if ( bit_pos < 6) {                                /* SPLIT MODE TCA output is on pins 0-5       */
      if ( (__PeripheralControl & TIMERA0)) {              /* make sure user hasn't taken over TCA0      */
        if (((tcamux & PORTMUX_TCA0_gm) == portnum)) {    /* TCA0 mux is EASY - same as the port number */
          return TIMERA0;
        }
      }
      #if defined(TCA1)
      if (__PeripheralControl & TIMERA1) {               /* make sure user hasn't taken over TCA1      */
        tcamux &= PORTMUX_TCA1_gm;
        #if (_AVR_PINCOUNT > 48) && (defined(__AVR_DB__)) // Only the  has the PORTG mapping working
          if ((tcamux == 0 && portnum== PB ) || (tcamux == 0x18 && portnum== PG))
        #elif (!defined(__AVR_EA__)) // AVR Dx with 48 pins, AVR Ex with 48 pins, ERRATA'ed AVR128DA64
          if ((tcamux == 0 && portnum== PB ))
        #elif defined(__AVR_EA__)
          #if (_AVR_PINCOUNT==48)
            if ((tcamux == 0 && portnum== PB ))
          #else
            if (0)
          #endif
        #else
          #error "Unhandled case - has TCA1 but is not a DA, DB, or EA."
          if (0)
        #endif
        {
          return TIMERA1;                              /* mux options, not 3-channel ones on bit 4:6 */
        }
      }
      #endif
    }
  #endif
  #if defined(TCA1)
    if (bit_pos >= 4 && bit_pos < 7 && ((__PeripheralControl & TIMERA1))) {
      uint8_t tca1mux = PORTMUX.TCAROUTEA & PORTMUX_TCA1_gm;
      #if defined(__AVR_DA__) || (defined(__AVR_DB__) && _AVR_PINCOUNT < 64)
        if ((tca1mux == 0x01 && portnum== PC))
      #elif defined(__AVR_DB__)
        if ((tca1mux == 0x01 && portnum== PC) || (tca1mux == 0x02 && portnum== PE))
      #elif defined(__AVR_EA__) && _AVR_PINCOUNT == 48
        if ((tca1mux == 0x01 && portnum== PC) || (tca1mux == 0x02 && portnum== PE) || (tca1mux == 0x04 && portnum== PA) || (tca1mux == 0x05 && portnum== PD))
      #elif defined(__AVR_EA__) /* Pincount < 48, hence no PE */
        if ((tca1mux == 0x01 && portnum== PC) || (tca1mux == 0x04 && portnum== PA) || (tca1mux == 0x05 && portnum== PD))
      #else
        #error "Can't happen reached in pwm logic"
      #endif
      {
        return TIMERA1;
      }
    }
  #endif
  #if defined(TCE0)
    if ((__PeripheralControl & TIMERE0) && (bit_pos < 4)) {
      uint8_t tcemux = PORTMUX.TCEROUTEA;
      #if defined(WEX0)
        if (tcemux < 7 ) {
          if (portnum == tcemux && bit_pos < 4) {
            return TIMERE0;
          }
        /* Invalid MUX option set, timer is not currently functional.
        } else if (tcemux == 7) {
        */
        } else if (tcemux == 8) {
          if (bit_pos < 2 && (portnum== PC || portnum== PA)) {
            return TIMERE0_MUX9;
          }
        } else if (tcemux == 9) {
          if (portnum== PA && ( bit_pos > 1 && bit_pos < 6)) {
            return TIMERE0_MUX8;
          }
        }
      }
    #else // TCE just ain't himself when WEX isn't around...
      if (tcemux == portnum && bit_pos < 3) {
        return TIMERE0;
      }

    #endif
  #endif
  uint8_t timer = digitalPinToTimer(p);
  #if defined(TCD0)
    if (__PeripheralControl & TIMERD0) {
      if ((timer & TIMERD0) == TIMERD0) {
        byte tcdmux = (PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm);
        if (tcdmux == (timer & 0x07)) {
          return timer;
        }
      }
    }
  #endif
  #if defined(TCF0)
    if ((timer & TIMERF0) == TIMERF0) {
      if (((PORTMUX.TCFROUTEA & PORTMUX_TCF0_gm) == (timer & 0x07))) {
        return timer;
      }
    }
  #endif
  if ((timer & TIMERB0) == TIMERB0) { /* Finally check TCBn, if we made it here w/out returning */
    TCB_t* timer_B;
    timer_B = ((TCB_t *)&TCB0 + ((timer & 0x07))); /* get timer struct */
    if (((timer_B->CTRLB) &  TCB_CNTMODE_gm) != TCB_CNTMODE_PWM8_gc )
      return NOT_ON_TIMER; /* If the timer isn't in PWM mode, user has reconfigured
                              it, and nothing good can come of trying to use it. */
  }
  return timer;
}

void analogWrite(uint8_t pin, int val) {
  check_valid_digital_pin(pin);   // Compile error if pin is constant and isn't a pin.
  check_valid_duty_cycle(val);    // Compile error if constant duty cycle isn't between 0 and 255, inclusive. If those are generated at runtime, it is truncated to that range.
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) return; //this catches any run-time determined pin that isn't a pin.
  if ((val == 0 || val == 255)) {
    digitalWrite(pin,val ? HIGH : LOW);
    return;
  }
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
      // Now have ctrlb sorted. But we also need to do work on the offset
      if (offset & 0x07){ // 0b0000 0xxx
        offset &= 0xFE;   // WO0-2 ok - 0x0000 0xx0 0x01 -> 0x00, 0x02 -> 0x02, 0x04 -> 0x04
      } else {
        offset <<= 1;     //0b0xxx 0000
        _SWAP(offset);    //0b0000 0xxx
        offset |= 0x01;   //0b0000 0xx1 OK! 0x08 -> 0x01, 0x10 -> 0x03 0x20 -> 0x05
      }
      // THERE!! FINALLY!
      // Write the value to the register.
      *(((volatile uint8_t*) &(timer_A->SPLIT.LCMP0)) + offset) = val;
      // and ctrlb to ctrlb
      uint8_t t = timer_A->SPLIT.CTRLB;
      uint8_t oldsreg = SREG;
      cli();
      t |= ctrlb;
      timer_A->SPLIT.CTRLB = t;
      SREG = oldsreg;
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
      _setOutput(portnum, bit_mask);
      return;
    }

  #elif defined(TCE0) && defined(WEX0) // The TCE is WEX Luther's right-hand timer; in LA, where WEX hasn't been seen, TCE just isn't the same
    if ((__PeripheralControl & TIMERE0)&& (bit_mask < 0x10)) {
      uint8_t usetce0 = 0;
      uint8_t tcemux = PORTMUX.TCEROUTEA;
      uint8_t bit_mask_orig = bit_mask;
      if (tcemux < 7 ) {
        if (portnum == tcemux) {
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
        uint8_t dtiswap = WEX0.SWAP;
        volatile uint16_t *cmpreg = &TCE0_CMP0;
        if (bit_mask < 0x10){
          dtiswap &= ~bit_mask;
        } else {
          _SWAP(bit_mask);
          dtiswap |= bit_mask;
        }

        if (bit_mask >= 0x02) {
          cmpreg++;
        }
        if (bit_mask >= 0x04) {
          cmpreg++;
        }
        if (bit_mask >= 0x08) {
          cmpreg++;
        }
        _SWAP(bit_mask);
        bit_mask |= ((~bit_mask) & TCE0_CTRLB);
        TCE0_CTRLB = bit_mask;
        WEX0.SWAP = dtiswap;
        _setOutput(portnum, bit_mask_orig);
        *cmpreg = val;
        return;
      }
    }
  #elif defined(TCE0) // WEXles TCE on the LA needs different handling. It looks to me as if the WEXless TCE is very much like a TCA with the wonderful split mode removed. It's even back to 3 channels.
         #error "can't happen in supported parts yet"             // We can probably reuse the code for the TCA, or use it as a model, because with the likely mux layout, it's going to be like TCA0 only with half the pins.
  #endif
  /* Now we use the table in variant */
  uint8_t digital_pin_timer = digitalPinToTimer(pin);
  if (digital_pin_timer) {
    #if defined(TCD0)
      // Dx-series
      if (((digital_pin_timer & 0xC0) == 0x40) && (__PeripheralControl & TIMERD0)) { // TCD
        uint8_t tcdmux = digital_pin_timer & 0x07;
        uint8_t muxval = ((PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm) >> PORTMUX_TCD0_gp );
        #if !defined(__AVR_DA__)
          uint8_t usetcd0 = 0;
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
          uint8_t temp = TCD0.CMPBCLRL;
          temp = TCD0.CMPBCLRH;
          // Read both, only retaining the high byte. Need to read both to see high byte because 16-bit register
          // Reading just high doesn't work. Checking for CMPBCLR = 509, 1019, or at 32 MHz+, 2039 or 4079 for which we need to shift
          // the duty cycle left to match
          if (temp) {   // TOP > 254
            val <<= 1;  // leftshift once is good for 509
            if (temp   >= 0x03) {
              val <<= 1;  // 1019, 2039 or 4079
              #if F_CPU >= 32000000
                if (temp >= 0x07) {
                  val <<= 1;  // 2039
                  if (temp == 0x0F) {
                    val <<= 1;  // 4079
                    val = 4080 - val;
                  } else {
                    val = 2040 - val;
                  }
                } else {
                  val = 1020 - val;
                }
              #else
                val = 1020 - val;
                } else {
              #endif
              val = 510 - val;
            }
          } else {
            val = 255 - val;
          }
          // interrupts off while this runs - we really don't want this interrupted!
          uint8_t oldSREG = SREG;
          cli();
          if (digital_pin_timer & 0x10) {
            TCD0.CMPBSET = val - 1;
          } else {
            TCD0.CMPASET = val - 1;
          }
          /* Check if channel active, if not, have to turn it on */
          if (!(TCD0.FAULTCTRL & fc_mask)) {
            /*-----------------------------------------------------------------------------------------
             * need to be careful here - analogWrite() can be called by a class constructor, for
             * example in which case the timer hasn't been started yet. We must not start it in this
             * case, as it would then fail to initialize and have the wrong clock prescaler and other
             * settings. Similarly, in any other situation where the timer isn't running when we started
             * the most likely result of being automatically started by an analogWrite() is naught but
             * woe and misery.
             * Instead, we should do everything else, and when the timer is next enabled, the PWM will
             * be configured and waiting. This is also probably what users would expect and hope to
             * happen if they are modifying TCD0 registers themselves. Though per core docs, we make
             * no promises in that case, the fact that the fix for a call to analogWrite() in a class
             * constructor (something that is not proscribed by docs, and hence is supposed to work)
             * makes that case less bad is an added bonus.
             *---------------------------------------------------------------------------------------*/
            uint8_t temp2 = TCD0.CTRLA;
            TCD0.CTRLA = temp2 & (~TCD_ENABLE_bm);
            while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
            _PROTECTED_WRITE(TCD0.FAULTCTRL, (fc_mask | TCD0.FAULTCTRL));
            // while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
            TCD0.CTRLA = temp2; // re-enable it if it was enabled
          } else {
            TCD0.CTRLE = TCD_SYNCEOC_bm; // it was already on - just set new value and set sync flag.
          }
          SREG = oldSREG; // Turn interrupts back on, if they were off.
          _setOutput(portnum, bit_mask);
          return;
        }
      }
    #endif
    if ((digital_pin_timer & 0xF0) == 0x10 ) {
      /* its on a TCB */
      TCB_t * timer_B;
      timer_B = &TCB0 + (digital_pin_timer & 0x07);
      if (((timer_B->CTRLB) & TCB_CNTMODE_gm) == TCB_CNTMODE_PWM8_gc ) {
        /* set duty cycle */
        #if defined(ERRATA_TCB_CCMP) && ERRATA_TCB_CCMP == 0
          timer_B->CCMPH = val;
        #else
          timer_B->CCMPL = timer_B->CCMPL;   // load temp register with the period, 254 have to first make sure temp register holds 254
          timer_B->CCMPH = val;              /* We can leave interrupts on - only a read of the count in the ISR would mess things up.
           * That is a wacky corner case. If they have timer in 8-bit PWM mode, and they write the value in with another call, yet ALSO
           * insist on reading the timer value from within an ISR, yes that's a race condition, and it will shit on the compare value
           * This case was of course promptly hit by the stock arduino megaavr implementation. */
        #endif
        /* Enable Timer Output */
        timer_B->CTRLB |= (TCB_CCMPEN_bm);
        _setOutput(portnum, bit_mask);
        return;
      }
    }
    #if defined(TCF0)
      if ((digital_pin_timer & TIMERF0) == TIMERF0) {
        if (((TCF0.CTRLB & 0x07) == 0x07) && ((PORTMUX.TCFROUTEA & PORTMUX_TCF0_gm) == (digital_pin_timer & 0x07))) {
          if (digital_pin_timer & 0x08) {
            TCF0.CMP1 = val;
            TCF0.CTRLC |= 0x02;
          } else {
            TCF0.CMP0 = val;
            TCF0.CTRLC |= 0x01;
          }
          _setOutput(portnum, bit_mask);
          return;
        }
      }
    #endif
    #if defined(DAC0)
      if (digital_pin_timer == DACOUT) {
        uint8_t ctrla = DAC0.CTRLA;
        /* Previously there was a bunch of logic to check if we were writing 0 or 255 so we could turn off the DAC.
          That was a can't-happen, we already caught that at the start and diverted to digitalWrite and returned
          in the case that 0 or 255 was passed as the second argument.
         */
        ctrla |= 0x41;
        VPORTD.DIR &=  ~0x40; // make sure the output drivers are off, don't want the DAC to fight that.
        uint8_t pd6ctrl = PORTD.PIN6CTRL;
        pd6ctrl &= 0xF0; //clear opullup enable AND input mode,
        pd6ctrl |= PORT_ISC_INPUT_DISABLE_gc; // per datasheet, input must be disabled.
        PORTD.PIN6CTRL = pd6ctrl;
        // Now set the value and write control.
        #if defined(DAC0_DATAH)
          DAC0.DATAH = val;
          DAC0.CTRLA = ctrla; // OUTEN = 1, ENABLE = 1, but don't trash run stby
        #else
          DAC0.DATA = val;
          DAC0.CTRLA = ctrla; // OUTEN = 1, ENABLE = 1, but don't trash run stby
        #endif

        return;
      }
    #endif
  }
  /* Execution is only here if:
      a. No portmux aware tmer pointed at the pin
      b. they didn't pass 0 or 255 as the second argument
      c. there is no timer listed in the timer table either.
  */
  if (val > 127) {
    _setValueLow(portnum,bit_mask);
  } else {
    _setValueLow(portnum,bit_mask);
  }
  _setOutput(portnum, bit_mask);
}
