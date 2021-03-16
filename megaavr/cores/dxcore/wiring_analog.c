/*
  wiring_analog.c - analog input and output
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

#include "wiring_private.h"
#include "pins_arduino.h"
#include "Arduino.h"

inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if(__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN)
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      badArg("digital I/O function called  is constant, but not a valid pin");
  }
}

inline __attribute__((always_inline)) void check_valid_analog_ref(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (mode >= 7 || mode == 4)
      badArg("Analog reference requested is not a valid analog reference");
  }
}


inline __attribute__((always_inline)) void check_valid_analog_pin(pin_size_t pin) {
  if(__builtin_constant_p(pin)) {
  #ifdef MVIO
    if (!(pin == ADC_DAC0 || pin == ADC_TEMPERATURE || pin == ADC_VDDDIV10 || pin == ADC_VDDIO2DIV10 || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2))
  #else
    if (!(pin == ADC_DAC0 || pin == ADC_TEMPERATURE || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2))
  #endif
    {
      pin = digitalPinToAnalogInput(pin);
      if (pin == NOT_A_PIN) {
        badArg("analogRead called with constant pin that is not a valid analog pin");
      }
    }
  }
}


inline __attribute__((always_inline)) void check_valid_duty_cycle(int16_t val) {
  if (__builtin_constant_p(val)) {
    if (val < 0 || val >255)
      badArg("analogWrite duty cycle called with a constant not between 0 and 255");
  }
}


void analogReference(uint8_t mode)
{
  check_valid_analog_ref(mode);
  if (mode < 7 && mode !=4) {
    VREF.ADC0REF = (VREF.ADC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}


void DACReference(uint8_t mode)
{
  check_valid_analog_ref(mode);
  if (mode < 7 && mode !=4) {
    VREF.DAC0REF = (VREF.DAC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}
/*
void ACReference(uint8_t mode)
{
  if (mode < 7 && mode !=4) {
    VREF.ACREF = (VREF.ACREF & ~(VREF_REFSEL_gm))|(mode);
  }
}
*/

int analogRead(uint8_t pin)
{
  #ifdef MVIO
  if (pin == ADC_DAC0 || pin == ADC_TEMPERATURE || pin == ADC_VDDDIV10 || pin == ADC_VDDIO2DIV10 || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2) {
  #else
  if (pin == ADC_DAC0 || pin == ADC_TEMPERATURE || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2) {
  #endif
    pin = pin & 0x7F;
  } else {
    pin = digitalPinToAnalogInput(pin);
    if(pin == NOT_A_PIN) return -1;
  /* Reference should be already set up */
  }
  /* Select channel */
  ADC0.MUXPOS = (pin << ADC_MUXPOS_gp);

  /* Start conversion */
  ADC0.COMMAND = ADC_STCONV_bm;

  /* Wait for result ready */
  while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));

  /* Combine two bytes */
  #if (defined(__AVR_DA__) && (!defined(NO_ADC_WORKAROUND)))
    // That may become defined when DA-series silicon is available with the fix
    ADC0.MUXPOS = 0x40;
  #endif
  return ADC0.RES;

}


//analogReadResolution() has two legal values you can pass it, 8 or 10 on these parts. According to the datasheet, you can clock the ADC faster if you set it to 8.
//like the pinswap functions, if the user passes bogus values, we set it to the default and return false.

inline void analogReadResolution(uint8_t res) {
  if (!__builtin_constant_p(res))
    badArg("analogReadResolution must only be passed constant values");
  if (res !=10 && res != 12)
    badArg("analogReadResolution called with invalid argument - valid options are 10 or 12.");
  if (res == 12) {
    ADC0.CTRLA = (ADC0.CTRLA & (~ADC_RESSEL_gm)) | ADC_RESSEL_12BIT_gc;
  } else {
    ADC0.CTRLA = (ADC0.CTRLA & (~ADC_RESSEL_gm)) | ADC_RESSEL_10BIT_gc;
  }
}


// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
  check_valid_digital_pin(pin);
  check_valid_duty_cycle(val);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if(bit_mask == NOT_A_PIN) return;
  // We need to make sure the PWM output is enabled for those pins
  // that support it, as we turn it off when digitally reading or
  // writing with them.  Also, make sure the pin is in output mode
  // for consistently with Wiring, which doesn't require a pinMode
  // call for the analog output pins.
  //
  // "Uuugh! I hate this kind of crap..." -Spence
  //
  pinMode(pin, OUTPUT);
  //if(val < 1){  /* if zero or negative drive digital low */

  //  digitalWrite(pin, LOW);

  //} else if(val > 255){ /* if max or greater drive digital high */

  //  digitalWrite(pin, HIGH);

  //} else {  /* handle pwm to generate analog value */
  /* Get timer */

  uint8_t digital_pin_timer =  digitalPinToTimer(pin); // NON-TCA timer!
  if (digital_pin_timer != 0 && !(digital_pin_timer & PeripheralControl)) {
    digital_pin_timer=0;
  }
  uint8_t* timer_cmp_out;
  #if defined(NO_GLITCH_TIMERD)
  if (digital_pin_timer != TIMERD0 && digital_pin_timer != DACOUT) {
  #else
  if (digital_pin_timer != DACOUT) {
  #endif
    if(val <= 0){ /* if zero or negative drive digital low */
      return digitalWrite(pin, LOW);
    } else if(val >= 255){  /* if max or greater drive digital high */
      return digitalWrite(pin, HIGH);
    }
  }

  TCB_t *timer_B;
  //TCA_t *timer_A;
/*   Find out Port and Pin to correctly handle port mux, and timer.
 *switch (digital_pin_timer) {
 *  case TIMERA0:
 */
  if (bit_mask < 0x40) { //if could be on a TCA
    uint8_t portnum  = digitalPinToPort(pin);
    uint8_t tcaroute = PORTMUX.TCAROUTEA;

    if ((portnum == (tcaroute & (0x07))) && (PeripheralControl & TIMERA0)) {
      uint8_t offset = 0;
      if (bit_mask > 0x04) { //separate high from low timers
        bit_mask <<= 1;
        offset = 1;
      }
      /* TCA0 is on this port - it's our timer! */
      if      (bit_mask & 0x44) offset += 4;
      else if (bit_mask & 0x22) offset += 2;
      timer_cmp_out     = ((uint8_t*) (&TCA0.SPLIT.LCMP0)) + offset;
      (*timer_cmp_out)  = (val);
      TCA0.SPLIT.CTRLB |= bit_mask;
      return; //either way, we're done here, we set a pwm channel!
    }


    #ifdef TCA1
/*  case TIMERA1:
 */
    tcaroute &= (0x18);
    /*  What about TCA1? */
    if (((portnum == 6 && tcaroute == 0x18) || (portnum == 1 && tcaroute ==0)) && (PeripheralControl & TIMERA1)) {
      /* We are on TCA1 - Set pwm and return */
      uint8_t offset = 0;
      if (bit_mask > 0x04) { //separate high from low timers
        bit_mask <<= 1;
        offset = 1;
      }
      /* TCA0 is on this port - it's our timer! */
      if      (bit_mask & 0x44) offset += 4;
      else if (bit_mask & 0x22) offset += 2;
      timer_cmp_out     = ((uint8_t*) (&TCA1.SPLIT.LCMP0)) + offset;
      (*timer_cmp_out)  = (val);
      TCA1.SPLIT.CTRLB |= bit_mask;
      return;
    }
//  break;
  #endif
  }
/*
 * switch (digital_pin_timer) {
 */
  switch (digital_pin_timer) {
    case TIMERB0:
    case TIMERB1:
    case TIMERB2:
    case TIMERB3:
    case TIMERB4:

      /* Get pointer to timer, TIMERB0 order definition in Arduino.h*/
      //assert (((TIMERB0 - TIMERB3) == 2));
      timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - TIMERB0));
      // make sure the timer is in PWM mode
      if (((timer_B->CTRLB) & TCB_CNTMODE_gm) == TCB_CNTMODE_PWM8_gc ) {
        /* set duty cycle */
        timer_B->CCMPH = val;
        /* Enable Timer Output */
        timer_B->CTRLB |= (TCB_CCMPEN_bm);
        return;
      } else {
        // if it's not, we don't have PWM on this pin!
        if (val < 128) {
          return digitalWrite(pin, LOW);
        } else {
          return digitalWrite(pin, HIGH);
        }
      }
      break;
  #if defined(DAC0)
    case DACOUT:
      #ifdef DAC0_DATAH
        //DAC0.DATAL = 0xC0;
        DAC0.DATAH = val;
      #else
        DAC0.DATA = val;
      #endif
      DAC0.CTRLA=0x41; //OUTEN=1, ENABLE=1
      break;
  #endif
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
    case TIMERD0:
    {
      #if defined(NO_GLITCH_TIMERD0)
        // "No glitch timerd" mode means that if analogWrite(pin,val) is called for a pin on a type D timer
        // with a duty cycle of 0% or 100%, instead of digitalWrite()'ing the pin, we will leave the timer
        // connected, and instead set the duty cycle to 0%. If the requested duty cycle is 100%, the pin
        // will then be inverted.
        //
        // If this is not defined, then the 0% and 100% cases will instead have been caught by the conditional
        // at the start of analogWrite().

        uint8_t set_inven = 0; // this will be set to 1 if we're setting the pin to a duty cycle of 100%
        if(val <= 0){
          val = 0;
        } else if (val >= 255){
          val = 0;
          set_inven = 1;
        }
      #endif
      /**************************************
      Determine the bit within TCD0.FAULTCTRL
      On Dx-series, WOA is always on bit 0 or bit 4 and so on
      On tinyAVR 1-series, WOA/WOB is on PA4/PA5, and WOC, WOD is on PC0/PC1.
      ***************************************/
      #ifndef MEGATINYCORE
        // Dx-series
        #ifndef ERRATA_TCD_PORTMUX
          //hopefully that gets rendereed as swap, not 4 leftshifts
          if (bit_mask < 0x10) bit_mask = bit_mask << 4;
        #endif
      #else
        // tinyAVR 1-series
        #ifdef USE_TIMERD_WOAB
          // Future feature to support TCD0-driven PWM on PA4/PA5.
          if (bit_mask < 0x0F) {
            bit_mask = (bit_mask == 0x02 ? 0x80 : 0x40);
          }
        #else
          bit_mask = (bit_mask == 0x02 ? 0x80 : 0x40);
        #endif
      #endif
      val = 255-val;
      uint8_t temp = TCD0.CMPBCLRL;
      temp = TCD0.CMPBCLRH;
      //
      // Read both, only retaining the high byte. Need to read both to see high byte because 16-bit register
      // Reading just high doesn't work. Checking for CMPBCLR = 509, 1019, or at 32 MHz+, 2039 or 4079 for which we need to shift
      // the duty cycle left to match
      if (temp) {   // TOP > 254
        val <<= 1;  // leftshift once is good for 509
        if (temp   >= 0x03) val <<= 1;  // 1019, 2039 or 4079
        #if (F_CPU>=32000000)           // At 32+ MHz it's less unreasonable to use.
          if (temp >= 0x07) val <<= 1;  // 2039
          if (temp == 0x0F) val <<= 1;  // 4079
        #endif
      }

      #if defined(NO_GLITCH_TIMERD0)
        volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(digitalPinToPortStruct(pin), digitalPinToBitPosition(pin));
        // if things aren't compile-time known, this is not a lightning fast operation.
        // We had been doing it closer to where we needed it, but there's no need to wait
        // until we have interrupts off to figure this out (though we do need them off when)
        // access it!)
      #endif
      // interrupts off while this runs - we really don't want this interrupted!
      uint8_t oldSREG = SREG;
      cli();
      /*-----------------------------------------------------------------------------------------
       * bit_mask & 0xAA? 0xAA = 0b10101010
       * This is true if the bitmask corresponds to an odd bit in the port, meaning it's
       * going to be driven by CMPB, otherwise by CMPA. On all existing parts, WOA is on
       * bit 0 or 4, WOB on 1 or 5, WOC on 2, 6, or 0, and WOD on 3, 7, or 1
       * Pins WOA and WOB are bound to CMPA and CMPB, but WOC and WOD can each be put on
       * either WOA or WOB. So if WOC is assigned to follow WOA and WOD to follow WOB, this
       * test gives the answer. This means, in theory, flexible PWM on TCD0 could be improved
       * by detecting the case where WOA or WOB is outputting PWM already, and the user then
       * calls analogWrite() on the other pin assigned to that channel, and we could swap that
       * pin to the other channel. But the code would be ugly (read: slow) and I don't think
       * the added capability would even be an improvement overall, because the cost in
       * terms of less consistent behavior is significant: the results become path-dependant,
       * since writing WOA, WOC, WOB in that order would result in WOC getting swapped to
       * WOB (now WOB and WOC would output same thing) while WOA, WOB, WOC would not, so
       * WOA and WOC would be the pair outputting the same thing). And then you'd need to
       * decide how to handle the above situation when the user then wrote to WOD.
       * Better to just declare that CMPA shall drive WOC, and CMPB shall drive WOD.
       *-----------------------------------------------------------------------------------------*/
      if (bit_mask & 0xAA) {
        TCD0.CMPBSET= val - 1;
      } else {
        TCD0.CMPASET= val - 1;
      }
      /* Check if channel active, if not, have to turn it on */
      if (!(TCD0.FAULTCTRL & bit_mask)) {
        /*-----------------------------------------------------------------------------------------
         * need to be careful here - analogWrite() can be called by a class constructor, for
         * example in which case the timer hasn't been started yet. We must not start it in this
         * case, as it would then fail to initialize and have the wrong clock prescaler and other
         * settings.
         * Instead, we should do everything else, and when the timer is next enabled, the PWM will
         * be configured and waiting. This is also probably what users would expect and hope to
         * happen if they are modifying TCD0 registers themselves. Though per core docs, we make
         * no promises in that case, the fact that this is better then is an added bonus.
         *---------------------------------------------------------------------------------------*/
        uint8_t temp2 = TCD0.CTRLA;
        TCD0.CTRLA = temp2 & (~TCD_ENABLE_bm;)
        _PROTECTED_WRITE(TCD0.FAULTCTRL, (bit_mask | TCD0.FAULTCTRL));
        while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
        TCD0.CTRLA = temp2; // re-enable it if it was enabled
      } else {
        TCD0.CTRLE = TCD_SYNCEOC_bm; // it was already on - just set new value and set sync flag.
      }

      #if defined(NO_GLITCH_TIMERD0)
        // In this mode, we need to check set_inven, and set INVEN if it was called with 100% duty cycle
        // and unset that bit otherwise.
        if (set_inven == 0){
          // we are not setting invert to make the pin HIGH when not set; either was 0 (just set CMPxSET > CMPxCLR)
          // or somewhere in between.
          *pin_ctrl_reg &= ~PORT_INVEN_bm;
        } else {
          // we *are* turning off PWM while forcing pin high - analogwrite(pin,255) was called on TCD0 PWM pin...
          *pin_ctrl_reg |= PORT_INVEN_bm;
        }
      #endif
      SREG = oldSREG; // Turn interrupts back on, if they were off.
      break;
    }
  #endif
    /* If non timer pin, or unknown timer definition. */
    /* do a digital write */
    case NOT_ON_TIMER:
    default:
      if (val < 128) {
        digitalWrite(pin, LOW);
      } else {
        digitalWrite(pin, HIGH);
      }
      break;
  }
}

void takeOverTCA0() {
  TCA0.SPLIT.CTRLA = 0;          // Stop TCA0
  PeripheralControl &= ~TIMERA0; // Mark timer as user controlled
                                 // Reset TCA0
  /* Okay, seriously? The datasheets and io headers disagree here for tinyAVR about whether the low bits even exist there! */
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // To answer earlier question that was here, yes, those low bits must be set!
}

#ifdef TCA1
void takeOverTCA1() {
  TCA1.SPLIT.CTRLA = 0;          // Stop TCA1
  PeripheralControl &= ~TIMERA1; // Mark timer as user controlled
                                 // Reset TCA1
  /* Okay, seriously? The datasheets and io headers disagree here for tinyAVR about whether the low bits even exist there! */
  TCA1.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // To answer earlier question that was here, yes, those low bits must be set!
}
#endif
void takeOverTCD0() {
  TCD0.CTRLA = 0;                     // Stop TCD0
  _PROTECTED_WRITE(TCD0.FAULTCTRL,0); // Turn off all outputs
  PeripheralControl &= ~TIMERD0; // Mark timer as user controlled
}


uint8_t digitalPinToTimerNow(uint8_t p) {
  uint8_t port = digitalPinToPort(p);
  uint8_t bit_pos = digitalPinToBitPosition(p);
  if ( bit_pos < 6) {
  #if defined(TCA1)
    uint8_t tcamux = PORTMUX.TCAROUTEA;
    if ( PeripheralControl & TIMERA0) {
      if (((tcamux & PORTMUX_TCA0_gm) == port)) {
        return TIMERA0;
      }
    }
    tcamux &= 0x18;
    if ( PeripheralControl & TIMERA1) {
      if ((tcamux == 0 && port == PB ) || (tcamux == 0x18 && port == PG)) {
        return TIMERA1;
      }
    }
  #else
    if ( PeripheralControl & TIMERA0) {
      if ((PORTMUX.TCAROUTEA & PORTMUX_TCA0_gm) == port) {
        return TIMERA0;
      }
    }
  #endif
  }
  /*
  if ( PeripheralControl & TIMERD0) {
    byte tcdmux = (PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm);
    if (tcdmux & 0x02) tcdmux +=3;          // Convert mux value to port
    if (port == tcdmux) {                   // Is the pin's port what mux pointed to?
      if (tcdmux==5) return (bit_pos < 4);  // PORTF is only PWM if 0-3
      return (bit_pos > 3);                 // All others 4-7
    }
  }
  */
  uint8_t timer = digitalPinToTimer(p);
  if (timer & TIMERB0) {
    TCB_t* timer_B;
    timer_B = ((TCB_t *)&TCB0 + (timer - TIMERB0));
    if (((timer_B->CTRLB) &  TCB_CNTMODE_gm) != TCB_CNTMODE_PWM8_gc )
      return NOT_ON_TIMER;
      // if the the timer isn't in PWM mode, then we don't actually have PWM here...
  }
  return timer;
}
