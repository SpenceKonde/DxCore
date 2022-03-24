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
#include <avr/pgmspace.h>

/* magic value passsed as the negative pin to tell the _analogReadEnh() (which implements both th new ADC
 * functions) to tell them what kind of mode it's to be used in. This also helps with providing useful and accurate
 * error messages and codes at runtime, since we have no other way to report such.                                 */

#define SINGLE_ENDED 254
inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if(__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN) {
      /* Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
       * While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
       * taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
       * Passing -1/255/NOT_A_PIN to the digital I/O functions is too likely to be intended                          */
      badArg("digital I/O function called is constant, but not a valid pin");
    }
  }
}

inline __attribute__((always_inline)) void check_valid_analog_ref(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (mode >= 7 || mode == 4)
      badArg("Analog reference requested is not a valid analog reference");
  }
}


inline __attribute__((always_inline)) void check_valid_enh_res(uint8_t res) {
  if (__builtin_constant_p(res)) {
    if (res < 0x80){
      if (res < ADC_NATIVE_RESOLUTION_LOW) {
            badArg("When a resolution is passed to analogReadEnh, it must be at least the minimum native resolution (8 bits)");
      } else if (res > ADC_MAX_OVERSAMPLED_RESOLUTION) {
        badArg("Requested resolution exceeds ADC capabilities. The highest resolution obtainable through oversampling and decimation is 15 bits (Dx-sereis) or 17 bits (Ex-series).");
      }
    } else if ((res & 0x7F) > 0x07) {
      badArg("Accumulation number invalid. Valid values are 8 - 15, or ADC_ACCn where n is a power of 2 between 2 and 128");
    }
  }
}

inline __attribute__((always_inline)) void check_valid_analog_pin(uint8_t pin) {
  if(__builtin_constant_p(pin)) {
  #ifdef MVIO
    if (!(pin == ADC_DAC0 || pin == ADC_GROUND || pin == ADC_TEMPERATURE || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2 || pin == ADC_VDDDIV10 || pin == ADC_VDDIO2DIV10))
  #else
    if (!(pin == ADC_DAC0 || pin == ADC_GROUND || pin == ADC_TEMPERATURE || pin == ADC_DACREF0 || pin == ADC_DACREF1 || pin == ADC_DACREF2))
  #endif
    { // if it is one of those constants, we know it is valid. Otherwise, make sure it's a valid channel.
      if (pin > 0x80) { // given as a channel, not a pin, but not one of the special inputs???
        pin &= 0x7f;
        if (pin > ADC_MAXIMUM_PIN_CHANNEL)
          badArg("analogRead called with constant channel number which is neither a valid internal source nor an analog pin");
      } else {
        pin = digitalPinToAnalogInput(pin);
      }
      if (pin == NOT_A_PIN) {
        badArg("analogRead called with constant pin that is not a valid analog pin");
      }
    }
  }
}

inline __attribute__((always_inline)) void check_valid_analog_pin_neg(pin_size_t pin) {
  if(__builtin_constant_p(pin)) {
    if (!(pin == ADC_DAC0 || pin == ADC_GROUND)) {
      // if it is one of those constants, we know it is valid. Otherwise, make sure it's a valid channel.
      if (pin > 0x80) { // high bit is set, indicating a channel number, not a pin, but not one of the special inputs?
        pin &= 0x7f;
          if (pin > ADC_MAXIMUM_PIN_CHANNEL) {
            badArg("analogReadDiff called with constant neg. channel that is neither a valid internal source nor an analog pin");
          }
      } else {
        pin = digitalPinToAnalogInput(pin);
      }
      if (pin == NOT_A_PIN) {
        badArg("analogReadDiff called with constant that is not a valid analog pin, ADC_DAC0 or ADC_GROUND");
      } else if(pin > ADC_MAXIMUM_NEGATIVE_PIN) {
        badArg("analogReadDiff called with constant negative pin that is not valid as a negative input. Only pins on PORTD or PORTE can be negative diff.input.");
      }
    }
  }
}


inline __attribute__((always_inline)) void check_valid_duty_cycle(int16_t val) {
  if (__builtin_constant_p(val)) {
    if (val < 0 || val >255)
      badArg("analogWrite cannot produice duty cycle higher 100% or below 0%");
  }
}


void analogReference(uint8_t mode) {
  check_valid_analog_ref(mode);
  if (mode < 7 && mode !=4) {
    VREF.ADC0REF = (VREF.ADC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}

void DACReference(uint8_t mode) {
  check_valid_analog_ref(mode);
  if (mode < 7 && mode !=4) {
    VREF.DAC0REF = (VREF.DAC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}

int16_t analogRead(uint8_t pin) {
  check_valid_analog_pin(pin);
  if (pin < 0x80) {
    pin = digitalPinToAnalogInput(pin);
    if(pin == NOT_A_PIN) return -1;

  }
  /* Select channel */
  ADC0.MUXPOS = ((pin & 0x7F) << ADC_MUXPOS_gp);
  /* Reference should be already set up */

  /* Start conversion */
  ADC0.COMMAND = ADC_STCONV_bm;

  /* Wait for result ready */
  while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));

  #if (defined(__AVR_DA__) && (!defined(NO_ADC_WORKAROUND)))
    // That may become defined when DA-series silicon is available with the fix
    ADC0.MUXPOS = 0x40;
  #endif
  return ADC0.RES;
}


inline __attribute__((always_inline)) void check_valid_negative_pin(uint8_t pin) {
  if(__builtin_constant_p(pin)) {
    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    }
    pin &= 0x3F;
    if (pin != 0x40 && pin != 0x48 && pin > 0x0F) { /* Damn really? that's the garbage set of internal sources for differential read, ground and the DAC?! */
      badArg("Invalid negative pin - valid options are ADC_GROUND, ADC_DAC0, or any pin on PORTD or PORTE.");
    }
  }
}

bool analogSampleDuration(uint8_t dur) {
    ADC0.SAMPCTRL = dur;
    return "true";
}


int32_t _analogReadEnh(uint8_t pin, uint8_t neg, uint8_t res, __attribute__ ((unused)) uint8_t gain) {
  /* Combined implementation for enhanced and differential ADC reads. Gain is ignored, these have no PGA.
   * However, note that if the user passed a constant gain argument, which is what almost anyone would
   * do, THAT would be a compile error. Only deviant code that dynamically determines the gain and
   * neglects to verify that the part in question has a PGA can end up in a situation where code may
   * think it is getting gain applied, but none is. I will ensure that it is documented that any code
   * that runs as part of a library or is shared with others, and which calls either of these analogRead____
   * functions with a non-zero gain, it is the responsibility of that code to verify that it is being
   * used on a system with gain available.
   * Can be split into 4 phases:
   *  1. Process input parameters, detect error conditions that couldn't be caught at compile-time.
   *  2. Apply settings and take reading - MUXPOS, MUXNEG, CTRLA, CTRLB registers are involved
   *    We want the user to be successful, so backup and restore the two registers that don't get
   *    rewritten every time the ADC is used anyway.
   *  3. Process the reading we took if needed - decimate where decimation is required and shift as
   *    as appropriate when the user has requested less than 12 bits of resolution.
   *  4. Restore registers we modified amd return results.
   */

  /*******************************
   *  Phase 1: Input Processing  |
   ******************************/
  // if (!(ADC0.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
  // 5/16 - uhhhhl if it's disabled, we should re-enable it, take the reading, and our cleanup will turn it off again...
  uint8_t sampnum;
  if (res & 0x80) { // raw accumulation
    sampnum=res & 0x7F;
    if (sampnum > 7) return ADC_ENH_ERROR_RES_TOO_HIGH;
  } else {
    if (res < 8) return ADC_ENH_ERROR_RES_TOO_LOW;
    if (res > 15) return ADC_ENH_ERROR_RES_TOO_HIGH;
    sampnum = (res > ADC_NATIVE_RESOLUTION ? ((res - ADC_NATIVE_RESOLUTION) << 1) : 0);
  }
  if (pin < 0x80) {
    // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
    pin = digitalPinToAnalogInput(pin);
  }
  else pin &= 0x7F;
  if (pin > 0x4B || (pin > ADC_MAXIMUM_PIN_CHANNEL &&  pin < 0x40))  {
    /*  If it's over 0x4B, it's not a valid option.
        If it's higher than the highest analog channel number, and lower than the start of internal sources, it's not valid.
        This does not check for internal sources that don't exist (all valid internal sources are checked for in the case of compile-time-known pins)
          Not worth wasting on the scale of 20-some-odd bytes of flash just to give an error to people who dynamically generate invalid analog pins
          and try to use them with analogReadEnh(), instead of just returning whatever we get from reading the bogus channel */
    return ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL;
  }
  if (ADC0.COMMAND & ADC_STCONV_bm) {
    return ADC_ENH_ERROR_BUSY;
  /*  Doing the busy check up here so that if we are doing differential read, we
      find out before calculating MUXNEG, which we can now assign as soon as calculated..
      Otherwise, we'd have to check busy-ness, then if it was a differential reading, set MUXNEG. */
  }
  if (neg != SINGLE_ENDED) {
    if (neg < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      neg = digitalPinToAnalogInput(neg);
    } else {
      neg &= 0x3F;
    }
    if (neg > ADC_MAXIMUM_NEGATIVE_PIN && neg != 0x40 &&  pin != 0x48)  {
      // Not all pins are valid as negative side of differential ADC measurements.
      // On DA and DB, the last ADC channel is 21 (0x15).  DD goes up to 31 (0x1F)
      return ADC_DIFF_ERROR_BAD_NEG_PIN;
    }
    ADC0.MUXNEG=neg;
    /*  OK to do since we have verified that it's not in mid-conversion. That doesn't
        explicitly break anything to change it during conversion, but it would change
        the channel, then determine that it could not take the measurement and return
        the error (unlike the ADC on the tinyAVR 2-series where the datasheet says you
        can change the channel during a conversion and corrupt the results). But, if
        it was busy because it was in free running mode, though, this would change the
        channel used for subsequent reads. That is why we check that the ADC is not
        busy before entering this block. */
  } // end neg != SINGLE_ENDED
  /********************************************
   *  Phase 2: Configure ADC and take reading  |
   ********************************************/
  ADC0.MUXPOS = pin;
  uint8_t _ctrlb = ADC0.CTRLB;
  uint8_t _ctrla = ADC0.CTRLA;
  ADC0.CTRLA = ADC_ENABLE_bm | (res == ADC_NATIVE_RESOLUTION_LOW ? ADC_RESSEL_10BIT_gc : 0) | (neg == SINGLE_ENDED ? 0 : ADC_CONVMODE_bm);
  ADC0.CTRLB = sampnum;

  ADC0.COMMAND = ADC_STCONV_bm;
  while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
  int32_t result = ADC0.RES;
  /******************************
   *  Phase 3: Post-processing  |
   *****************************/
  if (res < 0x80 && res != ADC_NATIVE_RESOLUTION && res != ADC_NATIVE_RESOLUTION_LOW) { /* Result needs to be decimated or divided */
    // Logic more complicated here vs megaTinyCore because we have to contend with the fact that truncation has already occurred.
    if (res > ADC_NATIVE_RESOLUTION) {
      uint8_t resbits = res * 2 - ADC_NATIVE_RESOLUTION;
      // The number of bits of resolution we would be getting and need to decimate if theere was no truncation.
      // result length in bits, but count the bits beyond native resolution twice, since each one needs a bit of decimation.
      uint8_t shift = res - ADC_NATIVE_RESOLUTION;
      // but if it exceeds 16 bits it gets truncated
      // Effectively there are three possible oversampling values: 13, 14, and 15, and they need 1, 2, amd 1 shift, respectively.
      if (resbits > 16) {
        shift -= (resbits - 16); // these "shifts" were already done for us by the hardware to make the number in the result register.
      }
      while (shift) {
        result >>= 1;
        shift--;
      }

    } else if (res == 8) {
      result >>= 2;
    } else { //((res == ADC_NATIVE_RESOLUTION - 1) || (res == ADC_NATIVE_RESOLUTION_LOW - 1))
      // 9 or 11 bit res, because people are weird and might do this
      result >>= 1;
    }
  } // end of resolutions that require postprocessing.

  /*******************************
   *  Phase 4: Cleanup + Return  |
   ******************************/
  #if (defined(__AVR_DA__) && (!defined(NO_ADC_WORKAROUND)))
    // That may become defined when DA-series silicon is available with the fix
    ADC0.MUXPOS = 0x40;
  #endif
  ADC0.CTRLB = _ctrlb;      // the user having something set in CTRLB is not implausuble
  ADC0.CTRLA = _ctrla;      // undo the mess we just made in ADC0.CTRLA
  return result;
}

int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain) {
  check_valid_enh_res(res);
  check_valid_analog_pin(pos);
  check_valid_negative_pin(neg);
  if (__builtin_constant_p(gain)){
    if (gain != 0) badArg("This part does not have an amplifier; gain must be 0 or omitted");
  }
  return _analogReadEnh(pos, neg, res, 0);
}

inline int32_t analogReadEnh(uint8_t pin, uint8_t res, uint8_t gain) {
  check_valid_enh_res(res);
  check_valid_analog_pin(pin);
  if (__builtin_constant_p(gain)) {
    if (gain != 0) badArg("This part does not have an amplifier; gain must be 0 or omitted");
  }
  return _analogReadEnh(pin,SINGLE_ENDED,res,0);
}

bool analogReadResolution(uint8_t res) {
  #if defined(STRICT_ERROR_CHECKING) /* not yet implemented, may be an optional error in a future version */
    if (!__builtin_constant_p(res)) {
      badArg("analogReadResolution should only be passed constant values");
    }
  #endif
  if (__builtin_constant_p(res)) {
    if (res !=10 && res != 12) {
      badArg("analogReadResolution called with invalid argument - valid options are 10 or 12.");
    }
  }
  if (res == 12) {
    ADC0.CTRLA = (ADC0.CTRLA & (~ADC_RESSEL_gm)) | ADC_RESSEL_12BIT_gc;
  } else {
    ADC0.CTRLA = (ADC0.CTRLA & (~ADC_RESSEL_gm)) | ADC_RESSEL_10BIT_gc;
    return (res == 10);
  }
  return true;
}
int8_t getAnalogReadResolution() {
  return ((ADC0.CTRLA & (ADC_RESSEL_gm)) == ADC_RESSEL_12BIT_gc) ? 12 : 10;
}
inline uint8_t getAnalogSampleDuration() {
  return ADC0.SAMPCTRL;
}

uint8_t getAnalogReference() {
  return VREF.ADC0REF & VREF_REFSEL_gm;
}

uint8_t getDACReference() {
  return VREF.DAC0REF & VREF_REFSEL_gm;
}
/*
Frequency in kHz.
If (options & 1) == 1, will set frequencies outside of safe operating range
Otherwise, will be constrained to between 300 and 3000 (if internal reference used) or 300 and 6000 if not.
Note: analogReference does NOT check this! So set the clock speed after reference if you want that guardrail.
0 takes action, and -1 sets to default.
*/
 static const int16_t adc_prescale_to_clkadc[0x0F] PROGMEM = {(F_CPU /   2000L),(F_CPU /   4000L),(F_CPU /  8000L),(F_CPU / 12000L),
                                                              (F_CPU /  16000L),(F_CPU /  20000L),(F_CPU / 24000L),(F_CPU / 28000L),
                                                              (F_CPU /  32000L),(F_CPU /  48000L),(F_CPU / 64000L),(F_CPU / 96000L),
                                                              (F_CPU / 128000L),(F_CPU / 256000L),1};


int16_t analogClockSpeed(int16_t frequency, uint8_t options) {
  if (__builtin_constant_p(frequency)) {
    if (frequency < -1) {
      badArg("Acceptable arguments to analogClockSpeed are -1 (set to default value) 0 (no change) or a positive integer frequency (in kHz)");
    }
  }
  if (frequency == -1) {
    frequency = 1450;
  }
  if (frequency > 0 ) {
    if (!(options & 0x01)) {
      frequency = constrain(frequency, 125, 2000);
    }
    uint8_t newpresc = 0;
    while (((int16_t)pgm_read_word_near(&adc_prescale_to_clkadc[newpresc])> frequency) &&  newpresc < 8 && (pgm_read_word_near(&adc_prescale_to_clkadc[newpresc + 1]) > ((options & 0x01) ? 1 : 125) )) {
      newpresc++;
    }
    ADC0.CTRLB = newpresc;
  } else if (frequency != 0) {
    return ADC_ERROR_INVALID_CLOCK;
  }
  return pgm_read_word_near(&adc_prescale_to_clkadc[ADC0.CTRLC & ADC_PRESC_gm]);
}

// PWM output only works on the pins with hardware support.
// These are defined in the appropriate pins_arduino.h
// for the variant in use. On all other pins, the best we
// can do is output a HIGH or LOW except on PIN_PD6, which
// is the DAC output pin.

void analogWrite(uint8_t pin, int val)
{
  check_valid_digital_pin(pin);   // Compile error if pin is constant and isn't a pin.
  check_valid_duty_cycle(val);    // Compile error if constant duty cycle isn't between 0 and 255, inclusive. If those are generated at runtime, it is truncated to that range.
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

  uint8_t digital_pin_timer =  digitalPinToTimer(pin); // NON-TCA timer!
  if (digital_pin_timer != 0 && !(digital_pin_timer & PeripheralControl)) {
    digital_pin_timer=0;
  }
  uint8_t* timer_cmp_out;
  #if defined(NO_GLITCH_TIMERD)
    if (digital_pin_timer != TIMERD0 && digital_pin_timer != DACOUT)
  #else
    if (digital_pin_timer != DACOUT)
  #endif
  { /* Opening of body for above */
    if(val <= 0){ /* if zero or negative drive digital low */
      return digitalWrite(pin, LOW);
    } else if(val >= 255){  /* if max or greater drive digital high */
      return digitalWrite(pin, HIGH);
    }
  }

  TCB_t *timer_B;
  // TCA_t *timer_A;
/*   Find out Port and Pin to correctly handle port mux, and timer.
 *switch (digital_pin_timer) {
 *  case TIMERA0:
 */


  if (bit_mask < 0x40 ) { // if could be on a TCA
    uint8_t portnum  = digitalPinToPort(pin);
    uint8_t tcaroute = PORTMUX.TCAROUTEA;

    if ((portnum == (tcaroute & (0x07))) && (PeripheralControl & TIMERA0)) {
      uint8_t offset = 0;
      if (bit_mask > 0x04) { // separate high from low timers
        bit_mask <<= 1;
        offset = 1;
      }
      /* TCA0 is on this port - it's our timer! */
      if      (bit_mask & 0x44) offset += 4;
      else if (bit_mask & 0x22) offset += 2;
      timer_cmp_out     = ((uint8_t*) (&TCA0.SPLIT.LCMP0)) + offset;
      (*timer_cmp_out)  = (val);
      TCA0.SPLIT.CTRLB |= bit_mask;
      return; // either way, we're done here, we set a pwm channel!
    }


    #ifdef TCA1
/*  case TIMERA1:
 */
    tcaroute &= (0x18);
    /*  What about TCA1? */
    if (((portnum == 6 && tcaroute == 0x18) || (portnum == 1 && tcaroute ==0)) && (PeripheralControl & TIMERA1)) {
      /* We are on TCA1 - Set pwm and return */
      uint8_t offset = 0;
      if (bit_mask > 0x04) { // separate high from low timers
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
  switch (digital_pin_timer) {
    case TIMERB0:
    case TIMERB1:
    case TIMERB2:
    case TIMERB3:
    case TIMERB4:

      /* Get pointer to timer, TIMERB0 order definition in Arduino.h*/
      // assert (((TIMERB0 - TIMERB3) == 2));
      timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - TIMERB0));
      // make sure the timer is in PWM mode
      if (((timer_B->CTRLB) & TCB_CNTMODE_gm) == TCB_CNTMODE_PWM8_gc ) {
        /* set duty cycle */
        #if defined(ERRATA_TCB_CCMP) && ERRATA_TCB_CCMP == 0
          timer_B->CCMPH = val; /* does not yet exist */
        #else
          timer_B->CCMPL = timer_B->CCMPL;   // load temp register with the period, 254 have to first make sure temp register holds 254
          timer_B->CCMPH = val;              /* We can leave interrupts on interrupts - only a read of the count in the ISR would mess things up.
           * That is a wacky corner case. If they have timer in 8-bit PWM mode, and they write the value in with another call
           * Yes that's a race conditions and the outside-of-isr call will win. But no corruption will happen, and it would be the same
           * as if the ISR was called 1 clock cycle sooner. No big deal. */
        #endif
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
        // DAC0.DATAL = 0xC0;
        DAC0.DATAH = val;
      #else
        DAC0.DATA = val;
      #endif
      DAC0.CTRLA=0x41; // OUTEN=1, ENABLE=1
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
      #if defined(__AVR_DA__) || defined(__AVR_DB__)
        // Dx-series
        #ifndef ERRATA_TCD_PORTMUX
          // hopefully that gets rendereed as swap, not 4 leftshifts
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
       * bit 0 or 4, WOB on 1 or 5, WOC on 0, 2, 4, or 6, and WOD on 1, 3, 5, or 7.
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
         * settings.Similarly, in any other situation where the timer isn't running when we started
         * the most likely result it being automatically started by an analogWrite() is naught but
         * woe and misery.
         * Instead, we should do everything else, and when the timer is next enabled, the PWM will
         * be configured and waiting. This is also probably what users would expect and hope to
         * happen if they are modifying TCD0 registers themselves. Though per core docs, we make
         * no promises in that case, the fact that the fix for a call to analogWrite() in a class
         * constructor (something that is not proscribed by docs) makes that case less bad is an
         * added bonus.
         *---------------------------------------------------------------------------------------*/
        uint8_t temp2 = TCD0.CTRLA;
        TCD0.CTRLA = temp2 & (~TCD_ENABLE_bm);
        while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
        _PROTECTED_WRITE(TCD0.FAULTCTRL, (bit_mask | TCD0.FAULTCTRL));
        // while(!(TCD0.STATUS & 0x01));    // wait until it can be re-enabled
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
  #if defined(MILLIS_USE_TIMERA0)
    stop_millis();
  #endif
  TCA0.SPLIT.CTRLA = 0;          // Stop TCA0
  PeripheralControl &= ~TIMERA0; // Mark timer as user controlled
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA0
  /* Okay, seriously? The datasheets and io headers disagree here for tinyAVR
     about whether the low bits even exist! Much less whether they need to be
     set - but if they are not set, it will not work */
}

void resumeTCA0() {
  TCA0.SPLIT.CTRLA = 0;         // Stop TCA0
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA0
  init_TCA0();                  // reinitialize TCA0
  PeripheralControl |= TIMERA0; // Mark timer as core controlled
  #if defined(MILLIS_USE_TIMERA0)
    restart_millis();              // If we stopped millis for takeover, restart
  #endif
}

#if defined(TCA1)
void takeOverTCA1() {
  #if defined(MILLIS_USE_TIMERA1)
    stop_millis();
  #endif
  TCA1.SPLIT.CTRLA = 0;               // Stop TCA1
  PeripheralControl &= ~TIMERA1;      // Mark timer as user controlled
  TCA1.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA1
}

void resumeTCA1() {
  TCA1.SPLIT.CTRLA = 0;         // Stop TCA1
  TCA1.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; // Reset TCA1
  init_TCA1();                  // reinitialize TCA1
  PeripheralControl |= TIMERA1; // Mark timer as core controlled
  #if defined(MILLIS_USE_TIMERA1)
    restart_millis();              // If we stopped millis for takeover, restart
  #endif
}
#endif

void takeOverTCD0() {
#if !defined(MILLIS_USE_TIMERD0)
  TCD0.CTRLA = 0;                     // Stop TCD0
  _PROTECTED_WRITE(TCD0.FAULTCTRL,0); // Turn off all outputs
  PeripheralControl &= ~TIMERD0;      // Mark timer as user controlled
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


uint8_t digitalPinToTimerNow(uint8_t p) {
  uint8_t bit_pos = digitalPinToBitPosition(p);
  if (bit_pos == NOT_A_PIN) return NOT_ON_TIMER;     /* Use bit position to check for invalid pins */
  uint8_t port = digitalPinToPort(p);                /* If bit_pos is valid, port will be too      */
  if ( bit_pos < 6) {                                /* SPLIT MODE TCA output is on pins 0-5       */
  #if defined(TCA1)
    uint8_t tcamux = PORTMUX.TCAROUTEA;
    if ( PeripheralControl & TIMERA0) {              /* make sure user hasn't taken over TCA0      */
      if (((tcamux & PORTMUX_TCA0_gm) == port)) {    /* TCA0 mux is EASY - same as the port number */
        return TIMERA0;
      }
    }
    tcamux &= 0x18;
    if (PeripheralControl & TIMERA1) {               /* make sure user hasn't taken over TCA0      */
      if ((tcamux == 0 && port == PB ) || (tcamux == 0x18 && port == PG)) { /* supports only 6-ch  */
        return TIMERA1;                              /* mux options, not 3-channel ones on bit 4:6 */
      }
    }
  #else
    if (PeripheralControl & TIMERA0) {               /* here we don't need to store tcamux */
      if ((PORTMUX.TCAROUTEA & PORTMUX_TCA0_gm) == port) { /* because it is only used once */
        return TIMERA0;
      }
    }
  #endif
  }
  uint8_t timer = digitalPinToTimer(p);
  /*
  if ( PeripheralControl & TIMERD0) {
    if (timer & TIMERD0) {
      byte tcdmux = (PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm);
      if (tcdmux == (timer & ~TIMERD0)) {
        return TIMERD0;
      }

  }
  */
  if (timer & TIMERB0) { /* Finally check TCBn, if we made it here w/out returning */
    TCB_t* timer_B;
    timer_B = ((TCB_t *)&TCB0 + (timer - TIMERB0)); /* get timer struct */
    if (((timer_B->CTRLB) &  TCB_CNTMODE_gm) != TCB_CNTMODE_PWM8_gc )
      return NOT_ON_TIMER; /* If the timer isn't in PWM mode, user has reconfigured
                              it, and nothing good can come of trying to use it. */
  }
  return timer;
}


// Not ready yet, but we ought to have something like this.

//uint8_t PWMoutputToPin(uint8_t timer, uint8_t channel, bool muxed) {

  /* valid values for timer start at line 300ish in Arduino.h.
   * NOT RECOMMENDED FOR CODE WHERE PERFORMNANCE IS IMPORTANT, no flash constrained settings. It's great for making library example code work.
   * valid values for channel are:
   * TCA: 0-5
   * TCB: 0 - this function tells you where PWM is currently piped, not where it could be piped.
   * TCD: 0-3
   */
/*
  If muxed is 0

  To efficiently test for error values, cast the result to an int8_t and verify that it's greater than 0.
  -1 indicates that a channel that the timer doesn't have was requested, but is not a compile time constant so we couldn't error then.
  -2 indicates that we found the pin that would work with that timer channel, but that's not the pin the timer is pointed at, and you need to fix that
  -3 indicates that you're requesting a channel that does not function due to outstanding errata on the chip. Go complain to microchip.

  if (__builtin_constant_p(channel)) {
    if (channel >=6) {
      badArg("PWMOutputToPin was called with an invalid value; no timer supports more than 6 channels, zero-indexed");
    }
    return NOT_A_PIN;
  }
  uint8_t retval = NOT_A_PIN
  //beyond that we shall make no assumptions!
  if (timer & TIMERD0) {
    if (channel > 3) {
      if (__builtin_constant_p(channel)) {
        badArg("TCDs only have channels 0~3, a channel outide that rane was requested!");
      }
      return NOT_A_PIN;
    } else if (PORTMUX.TCDROUTEA != (timer & 0x03)) {
      return TIMER_NOT_CONNECTED;
    }
    #if defined(ERRATA_TCD_PORTMUX)
      #if (!defined PIN_PA4)
        #error "Can't happen, all parts that have the TCD portmux errata have a PA4, so there is a bug in the core that should be reported promptly");
      if (PORTMUX.TCDROUTEA != 0) {
        return TIMER_BROKEN_ERRATA;
      }
      retval = PIN_PA4 + channel.
    #else
      for (uint8_t i = 0; i < PINS_COUNT; i++) {
        retval = i;
        if (digitalPinToTimer(i) == timer) {
          break;
        }
      }
    #endif
  } else if (timer & TIMERA0) {
    if (channel > 5) {
      return NOT_A_PIN; // TCA's never have more than 6 pins per mapping
    }
    for (uint8_t i = 0; i < PINS_COUNT) {
      uint8_t port = USARTROUTEA & 0x07;
      if (digitalPinToPort(pin) == port) {
        if (digitalPinToBitPostion(pin == channel)) {
          retval = i;
          break;
        }
      }
    }
  } else if (timer & TIMERB0) {
    if (channel) {
      return NOT_A_PIN; // TCBs have an alt pin but it's for the same channel, and the timer argument i different.
    } else {
      if (MILLIS_TIMER == timer) return PERIPHERAL_IN_USE;
      switch(timer) {
        case TIMERB0:
          #if defined(PIN_PA2)
            return PIN_PA2;
          #endif
          break;
        case TIMERB1:
          #if defined(PIN_PA3)
            return PIN_PA3;
          #endif
          break;

        case TIMERB2:
          #if defined(PIN_PB5)
            return PIN_PB5;
          #endif
          break;

        case TIMERB3:
          #if defined(PIN_PC0)
            return PIN_PC0;
          #endif
          break;

        case TIMERB4:
          #if defined(PIN_PG3)
            return PIN_Pxn;
          #endif
          break;
        }
      }
    }
  }
*/
