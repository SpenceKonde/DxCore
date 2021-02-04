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
    VREF.ADC0REF=(VREF.ADC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}


void DACReference(uint8_t mode)
{
  check_valid_analog_ref(mode);
  if (mode < 7 && mode !=4) {
    VREF.DAC0REF=(VREF.DAC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}
/*
void ACReference(uint8_t mode)
{
  if (mode < 7 && mode !=4) {
    VREF.ACREF=(VREF.ACREF & ~(VREF_REFSEL_gm))|(mode);
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
    pin=pin&0x7F;
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
    ADC0.MUXPOS=0x40;
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
    ADC0.CTRLA=(ADC0.CTRLA&(~ADC_RESSEL_gm)) | ADC_RESSEL_12BIT_gc;
  } else {
    ADC0.CTRLA=(ADC0.CTRLA&(~ADC_RESSEL_gm)) | ADC_RESSEL_10BIT_gc;
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
  uint8_t bit_pos  = digitalPinToBitPosition(pin);
  if(bit_pos == NOT_A_PIN) return;
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
  uint8_t digital_pin_timer =  digitalPinToTimer(pin);
  if (digital_pin_timer != 0 && !(digital_pin_timer & PeripheralControl)) {
    return; //this timer has been talken over by the user
  }
  uint8_t* timer_cmp_out;
  #if defined(NO_GLITCH_TIMERD)
  if (digital_pin_timer != TIMERD0){
  #endif
    if(val <= 0){ /* if zero or negative drive digital low */
      return digitalWrite(pin, LOW);
    } else if(val >= 255){  /* if max or greater drive digital high */
      return digitalWrite(pin, HIGH);
    }
  #if defined(NO_GLITCH_TIMERD)
  }
  #endif

  TCB_t *timer_B;
  //TCA_t *timer_A;
  /* Find out Port and Pin to correctly handle port mux, and timer. */
  switch (digital_pin_timer) {
    case TIMERA0:
    /* Calculate correct compare buffer register */
    if (bit_pos>2) {
      bit_pos-=3;
      timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.HCMP0)) + (bit_pos<<1);
      (*timer_cmp_out) = (val);
      TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_HCMP0EN_bp + bit_pos));
    } else {
      timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.LCMP0)) + (bit_pos<<1);
      (*timer_cmp_out) = (val);
      TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
    }
    break;

  #ifdef TCA1
    case TIMERA1:
    /* Calculate correct compare buffer register */
    if (bit_pos>2) {
      bit_pos-=3;
      timer_cmp_out = ((uint8_t*) (&TCA1.SPLIT.HCMP0)) + (bit_pos<<1);
      (*timer_cmp_out) = (val);
      TCA1.SPLIT.CTRLB |= (1 << (TCA_SPLIT_HCMP0EN_bp + bit_pos));
    } else {
      timer_cmp_out = ((uint8_t*) (&TCA1.SPLIT.LCMP0)) + (bit_pos<<1);
      (*timer_cmp_out) = (val);
      TCA1.SPLIT.CTRLB |= (1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
    }
    break;
  #endif

    case TIMERB0:
    case TIMERB1:
    case TIMERB2:
    case TIMERB3:
    case TIMERB4:

      /* Get pointer to timer, TIMERB0 order definition in Arduino.h*/
      //assert (((TIMERB0 - TIMERB3) == 2));
      timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - TIMERB0));

      /* set duty cycle */
      timer_B->CCMPH = val;

      /* Enable Timer Output */
      timer_B->CTRLB |= (TCB_CCMPEN_bm);

      break;

  #if defined(DAC0)
    case DACOUT:
      #ifdef DAC0_DATAH
        DAC0.DATAH=val;
      #else
        DAC0.DATA=val;
      #endif
      DAC0.CTRLA=0x41; //OUTEN=1, ENABLE=1
      break;
  #endif

  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
    case TIMERD0:
    {
      uint8_t fc_mask;
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
        } else if(val >= 255){
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
        fc_mask = (0x10 << (bit_pos & 0x03));
      #else
        // tinyAVR 1-series
        #ifdef USE_TIMERD_WOAB
          // Future feature to support TCD0-driven PWM on PA4/PA5.
          if (bit_pos > 3) {
            fc_mask = (bit_pos == 5 ? 0x20 : 0x10);
          } else {
            fc_mask = (bit_pos == 1 ? 0x80 : 0x40);
          }
        #else
          fc_mask = (bit_pos == 1 ? 0x80 : 0x40);
        #endif
      #endif
      val = 255-val;
      uint8_t temp = TCD0.CMPBCLRL;
      temp = TCD0.CMPBCLRH;  // intentional use of the comma operator
      //
      // will this read both, only retaining the high byte? Need to read both to see high register because 16-bit
      // even though only need to get high because 16-bit register. Reading just high doesn't work
      if (temp) {
        val <<= 1;
        if (temp == 3) val <<= 1; // 1019 or 2039
#if (F_CPU>=32000000)             // At 32+ MHz it's less unreasonable to use.
        if (temp == 7) val <<= 1; // 2039
#endif
      }

      uint8_t oldSREG=SREG;
      cli();
      // interrupts off... wouldn't due to have this mess interrupted and messed with...

      /* For flexible timer D PWM pins or USE_TIMERD_WOAB, this may need to be adapted */
      // as long as I don't support putting WOC on WOB if only WOA is being used, no adaptation needed.
      // And yeah, I'm already giving you extra freedom with these pins, shut up and be happy or takeOverTCD0()!
      if (bit_pos & 1) {
        //TCD0.CMPBSET=((255 - val) << 1) - 1;
        TCD0.CMPBSET= val - 1;
      } else {
        //TCD0.CMPASET=((255 - val) << 1) - 1;
        TCD0.CMPASET= val - 1;
      }
      /* Check if channel active, if not, have to turn it on */
      if (!(TCD0.FAULTCTRL & fc_mask)) {
        TCD0.CTRLA &= ~TCD_ENABLE_bm;       // stop the timer
        _PROTECTED_WRITE(TCD0.FAULTCTRL,(fc_mask | TCD0.FAULTCTRL));
        while(!(TCD0.STATUS & 0x01)) {;}    // wait until it can be re-enabled
        TCD0.CTRLA |= TCD_ENABLE_bm;        // re-enable it
      } else {
        /* if it's already on, all we have to do is sync! */
        TCD0.CTRLE = TCD_SYNCEOC_bm;
      }

      #if defined(NO_GLITCH_TIMERD0)
        volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(digitalPinToPortStruct(pin), bit_pos);
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
      SREG=oldSREG; // Turn interrupts back on, if they were off.
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
  /* Okay, seriously? The datasheets and io headers disagree here */
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc; /* |0x03; // do these bits need to be set or don't they? Does this even WORK on tinyAVR?! */
}
#ifdef TCA1
void takeOverTCA1() {
  TCA1.SPLIT.CTRLA = 0;          // Stop TCA0
  PeripheralControl &= ~TIMERA1; // Mark timer as user controlled
                                 // Reset TCA0
  /* Okay, seriously? The datasheets and io headers disagree here */
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc; /* |0x03; // do these bits need to be set or don't they? Does this even WORK on tinyAVR?! */
}
#endif
void takeOverTCD0() {
  TCD0.CTRLA = 0;                     // Stop TCD0
  _PROTECTED_WRITE(TCD0.FAULTCTRL,0); // Turn off all outputs
  PeripheralControl &= ~TIMERD0; // Mark timer as user controlled
}
