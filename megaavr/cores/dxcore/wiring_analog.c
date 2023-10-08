/*  OBLIGATORY LEGAL BOILERPLATE
 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
 either version 2.1 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this library;
 if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*//*
  wiring_analog.c - analog input and output
  Servo.h - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
  Copyright (c) 2005-2006 David A. Mellis, modified 2010 by David Sproul,
  and at least one other individual, since *someone* ported it to "megaavr"
  (aka, modern AVR), and it wasn't me. Finally, Since megaTinyCore was released
  in 2018 this has been extensively modified first for mTC, and then later for DxC
  by Spence Konde (2018-2023).

  This file is included with megaTinyCore and DxCore; note that unlike some files,
  but like most of the "core" files, while some pieces of this code have been shared
  between the two, and DxC was forked from mTC, the differing demands of the two platforms
  are such that an omnibus file is not tenable.
*/


#include "wiring_private.h"
#include "pins_arduino.h"
#include "Arduino.h"
#include <avr/pgmspace.h>


/* magic value passed as the negative pin to tell the _analogReadEnh() (which implements both th new ADC
 * functions) to tell them what kind of mode it's to be used in. This also helps with providing useful and accurate
 * error messages and codes at runtime, since we have no other way to report such.
 */

#define SINGLE_ENDED 254


inline __attribute__((always_inline)) void check_valid_analog_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    if (
    #if defined(ADC_TEMPERATURE)
        pin != ADC_TEMPERATURE &&
    #endif
    #if defined(ADC_GROUND)
        pin != ADC_GROUND &&
    #endif
    #if defined(ADC_DACREF0)
        pin != ADC_DACREF0 &&
    #endif
    #if defined(ADC_DACREF1)
        pin != ADC_DACREF1 &&
    #endif
    #if defined(ADC_DACREF2)
        pin != ADC_DACREF2 &&
    #endif
    #if defined(ADC_VDDDIV10)
        pin != ADC_VDDDIV10 &&
    #endif
    #if defined(ADC_VDDDIV10)
        pin != ADC_VDDIO2DIV10 &&
    #endif
    #if defined(ADC_DAC0) // 1-series
        pin != ADC_DAC0 &&
    #endif
        true)
    { // if it is one of those constants, we know it is valid. Otherwise, make sure it's a valid channel.
      if (pin > 0x80) { // given as a channel, not a pin, but not one of the special inputs???
        pin &= 0x7f;
        if (pin > ADC_MAXIMUM_PIN_CHANNEL)
          badArg("analogRead called with constant channel number which is neither a valid internal source nor an analog pin");
      }
      pin = digitalPinToAnalogInput(pin);
      if (pin == NOT_A_PIN) {
        badArg("analogRead called with constant pin that is not a valid analog pin");
      }
    }
  }
}

inline __attribute__((always_inline)) void check_valid_analog_ref(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (!(mode == EXTERNAL || mode == VDD || mode == INTERNAL1V024 || mode == INTERNAL2V048 || mode == INTERNAL4V1 || mode == INTERNAL2V5))
    badArg("analogReference called with argument that is not a valid analog reference");
  }
}

inline __attribute__((always_inline)) void check_valid_enh_res(uint8_t res) {
  if (__builtin_constant_p(res)) {
    if (res < 0x80) {
      if (res < ADC_NATIVE_RESOLUTION_LOW) {
            badArg("When a resolution is passed to analogReadEnh, it must be at least the minimum native resolution (8 bits)");
      } else if (res > ADC_MAX_OVERSAMPLED_RESOLUTION) {
        badArg("The highest resolution obtainable on these parts through oversampling and decimation with a single ADC operation 13 bits on 0/1-series or 17 on 2-series");
      }
  #if defined(__AVR_EA__)
    } else if ((res & 0x7F) > 0x0A) {
  #else
    } else if ((res & 0x7F) > 0x06) {
  #endif
      badArg("Accumulation number invalid - use one of the ADC_ACC_# constants for raw (undecimated) accumulated readings");
    }
  }
}

inline __attribute__((always_inline)) void check_valid_duty_cycle(int16_t val) {
  if (__builtin_constant_p(val)) {
    if (val < 0 || val >255)
      badArg("analogWrite cannot produice duty cycle higher 100% or below 0%");
  }
}

inline __attribute__((always_inline)) void check_valid_resolution(uint8_t res) {
  if (__builtin_constant_p(res))
  #if defined(__AVR_EA__)
    if (res != 8 && res != 10 && res != 12)
      badArg("analogReadResolution called with invalid argument - valid options are 8, 12, or 10 (compatibility mode).");
  #else
    if (res != 8 && res != 10)
      badArg("analogReadResolution called with invalid argument - valid options are 8 or 10.");
  #endif
}


#if defined(__AVR_EA__)
  // need a variable to store the resolution selected for analogRead - 8, 10, or 12,
  // as well as other options, such as whether to autodisable the PGA or leave it running.
  // Default resolution 10 bits, turn off PGA enabled
  // decoded as 0bPxxxRRRR
  // RRRR = resolution, 8, 10, or 12
  // P = 1 for PGA auto-shutoff.
  // C = 1 to chop signs. Sign chopping can only be used on Series and Burst measurements
  // (this includes analogReadEnh() if you specify a resolution that triggers oversampling and
  // decimation). But when enabled appropriately, it will swap the positive and negative
  // pins on successive measurements. The result is, especially when measuring very small differences
  // with a large gain and low reference, much smaller offset error.
  // Sign Chopping support is not yet implemented, but this is how it will be.
  // Unless we determine that we can just leave it on and have it only use it for
  // the burst/series modes (in which case, it will probably be made the default)
  // we will turn it on in analogReadEnh/Diff (_analogReadEnh() specifically)
  // and then turn it off before returning, when we restore other ADC settings.
  static uint8_t _analog_options = 0x80 | 10;
#endif

#ifdef DAC0
  void DACReference(uint8_t mode) {
    check_valid_analog_ref(mode);
    VREF.DAC0REF = mode | (VREF.DAC0REF & (~VREF_REFSEL_gm));
  }
#else
  void DACReference(__attribute__ ((unused))uint8_t mode) {
    badCall("DACreference is not available - this part does not have a DAC");
  }
#endif

/*----------------------------------------------------------------------------
 * Core ADC functionality implementations
 *----------------------------------------------------------------------------
 * Because the 2-series ADC is so radically different from the 0/1-series
 * (and, for that matter, just about everything else), it was most efficient
 * to separate out the core of the ADC stuff, where the viable shared code was
 * virtually nothing, and put it all in one part of the file, rather than the
 * normal practice of having the function defined in one place but with the
 * parts that are different controlled by #if's. Here, almost every line is
 * different!
 *
 * The only other time this has happened in the history of the AVR instruction
 * set, with any perpheral, is when the modern/AVRxt parts were introduced...
 * I hope we're not heading for total rewrites being required every few years.
 * I think this is just a transitional state; I couldn't see how to scale the
 * ADC like the AVR Dx-series has to something with this level of capability
 * either.
 *
 * ----------------------------------------------------------------------------
 * Functions:
 *
 * int16_t analogRead(uint8_t pin)
 *     The standard analogRead(). Single-ended, and resolution set by
 *     analogReadResolution(), default 10.
 *     large negative values are returned in the event of an error condition.

 * bool analogReadResolution(uint8_t resolution)
 *     Sets resolution for the analogRead() function. Unlike stock version,
 *     this returns true/false. If it returns false, the value passed was
 *     invalid, and resolution is set to the default, 10 bits.
 *     ONLY 8, 10 and 12 (for 2-series only) are valid. To use
 *     any other value, you must use analogReadEnh(). The way iut is done on
 *     the Zero/Due/Etc is 1) wasteful of limited resources, and 2) dishonest
 *     because when you ask for a resolution it can't provide, it will lie and
 *     pad the value with zeros.
 * bool analogSampleDuration(uint8_t dur)
 *     Sets sampling duration (SAMPLEN on 0/1 or SAMPDUR on 2-series)
 *     For high impedance voltage sources, use a longer sample length.
 *     For faster reads, use a shorter sample length. Passing 0 will
 *     approximately double analogRead() speed.
 *     Returns true if value is valid; on 0/1-series maximum is 31 (33 ADC clock
 *     sampling duration); on 2-series any 8-bit value is valid, and duration is
 *     this many plus 0.5 (without PGA) or 1 (with PGA) ADC clocks.
 *     ADC clock targeted by default at startup is 1~1.25 MHz on 0/1, and ~2 MHz on 2-series.
 * uint8_t getAnalogSampleDuration()
 *     Trivial macro (located in Arduino.h). Returns the argument that you would pass to
 *     analogSampleDuration to get the current setting back (useful before
 *     changing it to something exotic)
 * void ADCPowerOptions(uint8_t options)
 *     0 - disable PGA, turned on and off in future as above.
 *     1 - enable PGA ahead of time for the next measurement after which it will be turned off as above.
 *     2 - disable PGA, and in future turn if off immediately after use.
 *     3 - enable PGA, leave it on until explicitly turned off. This is a power hog.
 * int32_t analogReadEnh(uint8_t pin, int8_t res = ADC_NATIVE_RESOLUTION,
 *                                                          uint8_t gain = 0)
 *     Enhanced analogRead(). Still single-ended, res is resolution in bits,
 *     which range from 8 to the largest value that can be obtained from using
 *     the accumulation feature and a single "start" command to oversample x4
 *     per extra bit, followed by decimation/2 per extra bit (the math is
 *     described in Microchip/Atmel app notes). This maximum is 13 bits for
 *     0/1-series parts, and 17 bits for 2-series parts.
 *     Alternately, to get the raw accumulated ADC readings, pass one of the
 *     ADC_ACC_n constants for the second argument where n is a power of 2 up
 *     to 64 (0/1-series) 128 (Dx-series) or 1024 (2-series).
 *     On 2-series (but not Dx-series parts, which don't have a PGA) the PGA
 *     can be used. I was shocked when I saw that the PGA could be used with
 *     single-ended reads too, but they explicitly state that. Valid options
 *     for gain are 0 (PGA disabled), 1 (unity gain, but can improve performance
 *     of burst reads under some circumstances, and powers of 2 up to 16)
 * int32_t analogReadDiff(uint8_t pos, uint8_t neg,
 *                      int8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0)
 *     Uses the differential ADC functionality to take a differential ADC
 *     measurement between the specified positive and negative pins. Only pins
 *     on PORTA (tinyAVR 2-series) or on PORTD/PORTE (AVR Dx-series) can be the
 *     negative input to the ADC
 *     res works the same as for analogReadEnh.
 *     gain specifies that the programmable gain amplifier should be used.
 *     If gain = 0, the PGA will not be used. Valid options are 1, 2, 4, 8
 *     or 16 to configure the PGA to amplify by that factor.
 *
 *----------------------------------------------------------------------------
 * Errors:
 * analogReadEnh() or analogReadDiff() can return:
 *   ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL (-2100000000)
 *     The selected pin has no analog input capability, does not exist,
 *     or, if it's a channel number (from ADC_CH(channel number), it
 *     does not exist or is otherwise invalid.
 *   ADC_ENH_ERROR_BUSY (-2100000001)
 *     Not yet enabled - will be an optional error in future update, maybe.
 *     The ADC is already taking a reading - either in free-running mode
 *     or through some user-implemented interrupt driven ADC routine.
 *   ADC_ENH_ERROR_INVALID_SAMPLE_LENGTH (-2100000002)
 *     Not yet enabled - will be an optional error in future update, maybe.
 *     On the 0/1-series parts, if sample length is specified, it must be
 *     below 0x1F (only 5 bits are used). That corresponds to 33 ACD clocks
 *     On the 2-series parts, this error is never returned, because all
 *     values between 0 and 255, inclusive, are valid.
 *     Currently, instead of returning an error, this will just set sample
 *     length to the maximum supported value.
 *   ADC_ENH_ERROR_RES_TOO_LOW (-2100000003)
 *     Resolution must not be lower than the lowest native resolution that
 *     the part supports (Core presents this as ADC_NATIVE_RESOLUTION_LOW)
 *     Get 8-bits and divide it if this is required; this is a rare
 *     use-case, and it is not appropriate to force the overhead on
 *     everyone else.
 *   ADC_ENH_ERROR_RES_TOO_HIGH (-2100000004)
 *    The 0/1-series can get 3 extra bits by oversampling 64x and right-
 *    shifting 3 times. The 2-series can get 5 by oversampling by a whopping
 *    1024x (taking 13-160 ms depending on options) and then right-shifting
 *    5 times. Thus, maximums are 13 and 17. Alternately, one of the
 *    ADC_ACC_n constants may be used, where n is 2, 4, 8, 16, 32, 64, or
 *    on 2-series only, 128, 256, 512, or 1024 - in this case, that many
 *    samples will be taken through burst/accumulation mode and returned
 *    without decimation.
 * analogReadDiff() can also return:
 *   ADC_ENH_ERROR_BAD_NEG_PIN (-2100000005)
 *    Returned by analogReadDiff when the requested pin for the negative
 *    input is not valid. Negative pins can only be on PORTA (for tinyAVR)
 *    or PORTD/PORTE (for Dx-series), and the selection of available
 *    channels is more limited.
 *
 * printADCRuntimeError(uint32_t error, &HardwareSerial DebugSerial)
 *   Prints a text description of an error returnedby analogRead,
 *   analogReasdEnh(),or analogReadDiff() to the specified serial device
 *   Ex:
 *    printADCRuntimeError(-2100000003, &Serial);
 *    will print "ADC_ENH_ERROR_RES_TOO_LOW"
 *    Will print nothing and return false if the result wasn't an error
 *   This function is not an efficient user of flash space, it is meant as a
 *   debugging aid.
 * analogIsError(int16_t from analogRead or int32_t from analogReadEnh)
 *   Returns 1 (true) if the value, assuming it came from an analogRead/Enh
 *   function call, is not an analog reading but instead an error code.
 ****************************************************************************/
#if defined(__AVR_EA__)
  /*****************************************************
  START 2-series analogRead/analogReadXxxx functions
  *****************************************************/
  #define SINGLE_ENDED 254
  inline bool analogReadResolution(uint8_t res) {
    check_valid_resolution(res);
      bool temp = (res == 8 || res == 10 || res == 12);
      _analog_options = (_analog_options & 0xF0) | (temp ? res : 10); // just set that variable. The new ADC only wants to be told the resolution when it starts a conversion
      return temp;
  }
  int8_t getAnalogReadResolution() {
    return _analog_options & 0x0F;
  }
  void analogReference(uint8_t mode) {
    check_valid_analog_ref(mode);
    #if defined(STRICT_ERROR_CHECKING)
      if (mode > 7) return;
    #else
      mode &= 7;
    #endif
    if (mode != 1 && mode != 3) {
      ADC0.CTRLC = mode;
    }
    // Uh? Is that it? That was, ah, a tiny bit simpler.
  }
  inline uint8_t getAnalogReference() {
    return ADC0.CTRLC & ADC_REFSEL_gm;
  }
  inline uint8_t getDACReference() {
    return VREF.DAC0REF & VREF_REFSEL_gm;
  }
  int16_t analogRead(uint8_t pin) {
    check_valid_analog_pin(pin);
    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    } else {
      pin &= 0x3F;
    }
    #if PROGMEM_SIZE < 8096
      if (pin > 0x33) { // covers most ways a bad channel could come about
    #else
      if (pin > NUM_ANALOG_INPUTS && ((pin < 0x30) || (pin > 0x33))) {
    #endif
      return ADC_ERROR_BAD_PIN_OR_CHANNEL;
    }
    if (!ADC0.CTRLA & 0x01) return ADC_ERROR_DISABLED;

    if (ADC0.COMMAND & ADC_START_gm) return ADC_ERROR_BUSY;
    // gotta be careful here - don't want to shit ongoing conversion - unlikle classic AVRs
    // they say there is no buffering here!
    /* Select channel */
    ADC0.MUXPOS = pin; // VIA bit = 0;
    uint8_t command = (_analog_options & 0x0F) > 8 ? 0x11 : 0x01;
    /* Start conversion */
    ADC0.COMMAND = command;

    /* Wait for result ready */
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    // if it's 10 bit compatibility mode, have to rightshift twice.
    if ((_analog_options & 0x0F) == 10) {
      int16_t temp = ADC0.RESULT;
      temp >>= 2;
      return temp;
    }
    return ADC0.RESULT;
  }


  inline __attribute__((always_inline)) void check_valid_negative_pin(uint8_t pin) {
    if (__builtin_constant_p(pin)) {
      if (pin < 0x80) {
        // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
        pin = digitalPinToAnalogInput(pin);
      }
      pin &= 0x3F;
      if (pin != 0x33 && pin != 0x31 && pin != 0x30 && pin > 0x07) {
        badArg("Invalid negative pin - valid options are ADC_GROUND, ADC_VDDDIV10, ADC_DACREF0, or any pin on PORTA.");
      }
    }
  }
  bool analogSampleDuration(uint8_t sampdur) {
    // any uint8_t is a legal value...
    ADC0.CTRLE = sampdur;
    return true;
  }
  uint8_t getAnalogSampleDuration() {
    return ADC0.CTRLE;
  }
  /*NYI
  uint8_t ADCOptions(uint8_t options) {
    // 0b____CCFF
    // FF = Freerunning mode
    // Note that in freerun mode the ADC cannot be used for analogRead, you need to either define an ISR, or poll status and read the register directly
    // 00 = do nothing.
    // 01 = stop current conversion (very useful in freerun mode)
    // 10 = turn off freerun
    // 11 = turn on freerun
    */
   /*TODO: Figure out what happens if you have sign chopping on when it's not available. Does it blow up everything? Or is it just ignored?
    return
  }
  */

  void ADCPowerOptions(uint8_t options) {
    // 0b SSEEPPLL
    // SS = run standby
    // 00 = no change to run standby
    // 01 = no change to run standby
    // 10 = turn off run standby
    // 11 = turn on run standby

    // EE = ENABLE
    // 00 = Do not enable or disable ADC.
    // 01 = Do not enable or disable ADC.
    // 10 = Disable the ADC.
    // 11 = Enable the ADC.

    // LL = LOWLAT
    // 00 = Do nothing.  No change to whether ADC enabled or LOWLAT bit.
    // 01 = Do nothing.  No change to whether ADC enabled or LOWLAT bit.
    // 10 = LOWLAT on. No change to whether ADC enabled.
    // 11 = LOWLAT off. No change to whether ADC enabled.
    // PP = PGA stay-on
    // 00 = No action
    // 01 = Turn off PGA, settings unchanged. It will be enabled next time is requested, but will not automatically turn off.
    // 10 = Turn on PGA, and don't turn it off automatically.
    // 11 = turn off PGA now and automatically after each use
    uint8_t temp = ADC0.CTRLA; //performance.
    if (options & 0x02) {
      ADC0.CTRLA = 0; // hopwfully workaround lowlat errata by ensuring that everything is turned off.
      // and configuring lowlat mode.
      if (options & 0x01) {
        ADC0.CTRLA |=  ADC_LOWLAT_bm;
        temp |= ADC_LOWLAT_bm;
      } else {
        ADC0.CTRLA &= ~ADC_LOWLAT_bm;
        temp &= ~ADC_LOWLAT_bm;
      }
    }
    if (options & 0x20) {
      if (options & 0x10) {
        temp |= 1; // ADC on
      } else {
        temp &= 0xFE; // ADC off
      }
    }
    if (options & 0x80) {
      if (options & 0x40) {
        temp |= 0x80; // run standby
      } else {
        temp &= 0x7F; // no run standby
      }
    }
    ADC0.CTRLA = temp; //now we apply enable and standby, and lowlat has been turned on, hopefully that's good enough for the errata.
    if (options & 0x04) { // turn off PGA.
      ADC0.PGACTRL &= ~ADC_PGAEN_bm;
      if (options & 0x08)  {
        _analog_options &= 0x7F;
      } else {
        _analog_options |= 0x80;
      }
    } else { // not instruction to turn off PGA
      if (options & 0x08) { // is it in fact an instruction to turn on the PGA and leave it on?
        ADC0.PGACTRL |= ADC_PGAEN_bm; // turn on the PGA
        _analog_options &= 0x7F;      // turn off the auto-shutoff. If they told us to turn it on explicitly, surely they don't want us to turn it off of our own accord.
      }
    }
    // What a mess!
  }


  int32_t _analogReadEnh(uint8_t pin, uint8_t neg, uint8_t res, uint8_t gain) {
    if (!(ADC0.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
    uint8_t sampnum;
    if (res > 0x80) { // raw accumulation
      sampnum = res & 0x7F;
      if (sampnum > 10) return ADC_ENH_ERROR_RES_TOO_HIGH;
    } else {
      if (res < ADC_NATIVE_RESOLUTION_LOW) return ADC_ENH_ERROR_RES_TOO_LOW;
      if (res > 17) return ADC_ENH_ERROR_RES_TOO_HIGH;
      sampnum = (res > ADC_NATIVE_RESOLUTION ? ((res - ADC_NATIVE_RESOLUTION) << 1) : 0);
    }

    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    } else {
      pin &= 0x3F;
    }
    #if PROGMEM_SIZE < 8096
      if (pin > 0x33) { // covers most ways a bad channel could come about
    #else
      if (pin > NUM_ANALOG_INPUTS && ((pin < 0x30) || (pin > 0x33))) {
    #endif
      return ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL;
    }
    if (neg != SINGLE_ENDED) {
      if (neg < 0x80) {
        // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
        neg = digitalPinToAnalogInput(neg);
      } else {
        neg &= 0x3F;
      }

      #if PROGMEM_SIZE < 8096
        if (neg > 0x33)  // covers most ways a bad channel could come about
      #else
        if (neg > NUM_ANALOG_INPUTS && ((neg != 0x30) && (neg != 0x33) && (neg != 0x31)))
      #endif
      {
        return ADC_DIFF_ERROR_BAD_NEG_PIN;
      }
      neg &= 0x3F;

    }
    pin &= 0x3F;

    if (ADC0.COMMAND & ADC_START_gm) return ADC_ENH_ERROR_BUSY;
    if (gain != 0) {
      uint8_t gainbits = 0;
      while (gain > 1) {
        gain >>= 1;
        gainbits+= 32;
      }
      ADC0.PGACTRL = (ADC0.PGACTRL & ~ADC_GAIN_gm) | gainbits | ADC_PGAEN_bm;
    }

    ADC0.MUXPOS = (gain ? ADC_VIA_PGA_gc:0) | pin;
    if (neg != SINGLE_ENDED) {
      ADC0.MUXNEG = (gain ? ADC_VIA_PGA_gc:0) | neg;
    }
    ADC0.CTRLF = sampnum;
    uint8_t command = ((neg != SINGLE_ENDED)?0x80:0) | ((res == 8) ? ADC_MODE_SINGLE_8BIT_gc : (res > ADC_NATIVE_RESOLUTION ? ADC_MODE_BURST_gc : ADC_MODE_SINGLE_12BIT_gc)) | 1;
    ADC0.COMMAND = command;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    int32_t result = ADC0.RESULT;

    if (res < 0x80 && res > ADC_NATIVE_RESOLUTION) {
      uint8_t shift = res - ADC_NATIVE_RESOLUTION ; // - 1;
      while (shift) {
        result >>= 1;
        shift--;
      }
      // Sanity checks
      // uint8_t roundup = result&0x01;
      // result >>= 1;
      // result += roundup;
    } else if (res == 8) {
      ; // do nothing
    } else if (res < ADC_NATIVE_RESOLUTION) {
      uint8_t shift = ADC_NATIVE_RESOLUTION - res;
      result >>= shift;
    }

    // res > 0x80 (raw accumulate) or res == 8, res == 12 need no adjustment.
    if (_analog_options & 0x80) { // this bit controls autoshutoff of PGA.
      ADC0.PGACTRL &= ~ADC_PGAEN_bm;
    }
    return result;
  }

  inline int32_t analogReadEnh(uint8_t pin, uint8_t res, uint8_t gain) {
    check_valid_enh_res(res);
    check_valid_analog_pin(pin);
    if (__builtin_constant_p(gain)) {
      if (gain != 0 && gain != 1 && gain != 2 && gain != 4 && gain != 8 && gain != 16){
        badArg("The requested gain is not available on this part, accepted values are 0, 1, 2, 4, 8 and 16.");
      }
    }
    return _analogReadEnh(pin, SINGLE_ENDED, res, gain);
  }

  inline int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain) {
    check_valid_enh_res(res);
    check_valid_analog_pin(pos);
    check_valid_negative_pin(neg);
    if (__builtin_constant_p(gain)) {
      if (gain != 0 && gain != 1 && gain != 2 && gain != 4 && gain != 8 && gain != 16){
        badArg("The requested gain is not available on this part, accepted values are 0, 1, 2, 4, 8 and 16.");
      }
    }
    return _analogReadEnh(pos, neg, res, gain);
  }

  static const PROGMEM int16_t adc_prescale_to_clkadc[0x11] =  {(F_CPU /  2000L),(F_CPU /  4000L),(F_CPU /  6000L),(F_CPU /  8000L),
  /* Doesn't get copied to ram because these all */     (F_CPU / 10000L),(F_CPU / 12000L),(F_CPU / 14000L),(F_CPU / 16000L),
  /* have fully memory mapped flash. Calculated  */     (F_CPU / 20000L),(F_CPU / 24000L),(F_CPU / 28000L),(F_CPU / 32000L),
  /* at compile time (we get to use division!)   */     (F_CPU / 40000L),(F_CPU / 48000L),(F_CPU / 56000L),(F_CPU / 64000L),1};


  /*
  Frequency in kHz.
  If options & 1 == 1, will set frequencies outside of safe operating range
  Otherwise, will be constrained to between 300 and 3000 (if internal reference used) or 300 and 6000 if not.
  Note: analogReference does NOT check this! So set the clock speed after reference if you want that guardrail.
  0 takes action, and -1 sets to default.
  */
  int16_t analogClockSpeed(int16_t frequency, uint8_t options) {
    if (frequency == -1) {
      frequency = 2750;
    }
    if (frequency > 0) {
      if ((options & 0x01) == 0) {
        frequency = constrain(frequency, 300, ((ADC0.CTRLC & 0x04) ? 3000 : 6000));
      }
      uint8_t prescale = 0;
      for (uint8_t i = 0; i < 16; i++) {
        int16_t clkadc = pgm_read_word_near(&adc_prescale_to_clkadc[i]);
        prescale = i;
        if ((frequency >= clkadc) || (adc_prescale_to_clkadc[i + 1] < ((options & 0x01) ? 2 : 300))) {
          ADC0.CTRLB = prescale;
          break;
        }
      }
    }
    if (frequency < 0) {
      return ADC_ERROR_INVALID_CLOCK;
    }
    return adc_prescale_to_clkadc[ADC0.CTRLB];
  }


/*---------------------------------------------------
 * END Ex-series analogRead/analogReadXxxx functions
 *--------------------------------------------------*/
#else
  /*****************************************************
  START Dx-series analogRead/analogReadXxxx functions
  *****************************************************/

void analogReference(uint8_t mode) {
  check_valid_analog_ref(mode);
  if (mode < 7 && mode != 4) {
    VREF.ADC0REF = (VREF.ADC0REF & ~(VREF_REFSEL_gm))|(mode);
  }
}

int16_t analogRead(uint8_t pin) {
  check_valid_analog_pin(pin);
  if (pin < 0x80) {
    pin = digitalPinToAnalogInput(pin);
    if (pin == NOT_A_PIN) {
      return ADC_ERROR_BAD_PIN_OR_CHANNEL;
    }
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
    if (pin != 0x40 && pin != 0x48 && pin > 0x0F) { /* Not many options other than pins are valid */
      badArg("Invalid negative pin - valid options are ADC_GROUND, ADC_DAC0, or any pin on PORTD or PORTE.");
    }
  }
}

bool analogSampleDuration(uint8_t dur) {
    ADC0.SAMPCTRL = dur;
    return true;
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
  if (!(ADC0.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
  // 5/16 - uhhhh if it's disabled, maybe we should re-enable it, take the reading, and our cleanup will turn it off again...
  //        Could lead to difficult to debug performance issues though, whereas this would tell them
  //        the minute they tried to that they had the ADC off. The startup time of the ADC is not negligible in many applications.
  uint8_t sampnum;
  if (res & 0x80) {           // raw accumulation
    sampnum = res & 0x7F;       // strip high bit and treat as negative number
    if (sampnum > 7)  return  ADC_ENH_ERROR_RES_TOO_HIGH; //
  } else {
    if (res < 8)                              return   ADC_ENH_ERROR_RES_TOO_LOW;
    if (res > ADC_MAX_OVERSAMPLED_RESOLUTION) return  ADC_ENH_ERROR_RES_TOO_HIGH;
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
    ADC0.MUXNEG = neg;
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
  int32_t result = ADC0.RES; // This should clear the flag
  /******************************
   *  Phase 3: Post-processing  |
   *****************************/
  if (res < 0x80 && res != ADC_NATIVE_RESOLUTION && res != ADC_NATIVE_RESOLUTION_LOW) { /* Result needs to be decimated or divided */
    // Logic more complicated here vs megaTinyCore because we have to contend with the fact that truncation has already occurred.
    if (res > ADC_NATIVE_RESOLUTION) {
      uint8_t resbits = res * 2 - ADC_NATIVE_RESOLUTION;
      // The number of bits of resolution we would be getting and need to decimate if there was no truncation.
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
  #if (defined(ERRATA_ADC_PIN_DISABLE) && ERRATA_ADC_PIN_DISABLE != 0)
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
    if (res != 10 && res != 12) {
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
    ADC0.CTRLC = newpresc;
    uint16_t f = pgm_read_word_near(&adc_prescale_to_clkadc[newpresc]);
    uint8_t ctrld = 0;
    if(f >= 2500) {
      ctrld = ADC_INITDLY_DLY256_gc;
    } else if(f > 1280) {
      ctrld = ADC_INITDLY_DLY128_gc;
    } else if(f > 640) {
      ctrld = ADC_INITDLY_DLY64_gc;
    } else if (f > 320) {
      ctrld = ADC_INITDLY_DLY32_gc;
    } else {
      ctrld = ADC_INITDLY_DLY16_gc;
    }
    ADC0.CTRLD = ctrld;
  } else if (frequency != 0) {
    return ADC_ERROR_INVALID_CLOCK;
  }
  return pgm_read_word_near(&adc_prescale_to_clkadc[ADC0.CTRLC & ADC_PRESC_gm]);
}

#endif
/* From here on down is PWM stuff */

// PWM output only works on the pins with hardware support.
// These are defined in the appropriate pins_arduino.h
// for the variant in use. On all other pins, the best we
// can do is output a HIGH or LOW except on PIN_PD6, which
// is the DAC output pin.
#if defined(TCA1)
  const PROGMEM uint8_t _tcaonemux[8] = {0x20, 0x00, 0x08, 0x28, 0x10, 0xFF, 0x18}; // just enough order that no efficient operation works! Really?
  // mux(port) = 4, 0, 1, 5, 2, -, 3, - ?
  // port(mux) = 1, 2, 4, 6, 0, 3, -, - ?
#endif

#if defined(TCD0)
  const PROGMEM uint8_t _tcdmux[8]={0, 1, 5, 6, 4, -1, -1, -1};
#endif

void analogWrite(uint8_t pin, int val) {
  check_valid_digital_pin(pin);   // Compile error if pin is constant and isn't a pin.
  check_valid_duty_cycle(val);    // Compile error if constant duty cycle isn't between 0 and 255, inclusive. If those are generated at runtime, it is truncated to that range.
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) return; //this catches any run-time determined pin that isn't a pin.
  uint8_t offset = 0;
  uint8_t portnum  = digitalPinToPort(pin);
  uint8_t portmux_tca = PORTMUX.TCAROUTEA;
  TCA_t* timer_A = NULL;
  #if defined(TCA1)
  uint8_t threepin = 0;
  #endif
  // It could be a TCA0 or 1 or TCA1 mux 0 or 3;
  #if (defined(TCA1))
    if  (bit_mask < 0x40 && ((portmux_tca & 0x07) == portnum) && (__PeripheralControl & TIMERA0)) {
      timer_A = &TCA0;
    } else if ((__PeripheralControl & TIMERA1) && ((portmux_tca & 0x38) == pgm_read_byte_near(&_tcaonemux[portnum]))) {
      if (portnum == 1 || portnum == 6) {
        if (bit_mask < 0x40) {
          timer_A = &TCA1;
        }
      } else if ((bit_mask & 0x70) && portnum != 5) {
        timer_A = &TCA1;
        threepin = 1;
      }
    }

  #else
    if  ((bit_mask < 0x40) && ((portmux_tca == portnum) && (__PeripheralControl & TIMERA0))) {
      timer_A = &TCA0;
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
    GPIOR2 = ctrlb;
    uint8_t t = timer_A->SPLIT.CTRLB;
    uint8_t oldsreg = SREG;
    cli();
    t |= ctrlb;
    GPIOR3 = t;
    timer_A->SPLIT.CTRLB = ctrlb;
    GPIOR1 = timer_A->SPLIT.CTRLB;
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
    //#if !defined(__AVR_DD__)
      _setOutput(portnum, bit_mask);
    //#endif
  } else {
    TCB_t *timer_B;
    // TCA_t *timer_A;
    uint8_t digital_pin_timer = digitalPinToTimer(pin);
    switch (digital_pin_timer) {
      case NOT_ON_TIMER:{
        if (val < 128) {
          _setValueLow(portnum, bit_mask);
        } else {
          _setValueHigh(portnum, bit_mask);
        }
        break;
      }
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
            timer_B->CCMPH = val;              /* We can leave interrupts on - only a read of the count in the ISR would mess things up.
             * That is a wacky corner case. If they have timer in 8-bit PWM mode, and they write the value in with another call, yet ALSO
             * insist on reading the timer value from within an ISR, yes that's a race condition, and it will shit on the compare value */
          #endif
          /* Enable Timer Output */
          timer_B->CTRLB |= (TCB_CCMPEN_bm);
          return;
        } // if it's not, we don't have PWM on this pin!
        break;
      #if defined(DAC0)
        case DACOUT: {
          _setInput(portnum, bit_mask);
          uint8_t ctrla = DAC0.CTRLA;
          if (val == 0 || val == 255) {
            ctrla &= ~0x41; // clear we want to turn off the DAC in this case
          }
          volatile uint8_t* pinctrl_ptr = (volatile uint8_t*) 0x0476; // PD6 PINnCTRL;
          *pinctrl_ptr |= PORT_ISC_INPUT_DISABLE_gc;
          #if defined(DAC0_DATAH)
            DAC0.DATAH = val;
            DAC0.CTRLA |= 0x41; // OUTEN = 1, ENABLE = 1, but don't trash run stby
          #else
            DAC0.DATA = val;
            DAC0.CTRLA |= 0x41; // OUTEN = 1, ENABLE = 1, but don't trash run stby
          #endif
          return;
        }
      #endif
      #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
      // Else, it's on TCD0
        default: {
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
          On tinyAVR 1-series, WOA/WOB is on PA4/PA5, and WOC, WOD is on PC0/PC.
          In the past the same copy of this function was used for both cores. That has become untenable
          ***************************************/
          // Dx-series
          uint8_t port = digitalPinToPort(pin);
          uint8_t tcdmux = pgm_read_byte_near(&_tcdmux[(digital_pin_timer & 0x07)]);
          // First, if TCD portmux busted, but it's not set to 0, we can't get PWM, don't try
          uint8_t fc_mask = bit_mask ;
          #if defined(ERRATA_TCD_PORTMUX) && ERRATA_TCD_PORTMUX == 0
            if ((tcdmux != PORTMUX.TCDROUTEA && ((digital_pin_timer & 0x44) != 0x44 ))) {
              break;
            }
            if (!(tcdmux & 0x04)) {
              if (bit_mask < 0x10) { //cpi
                fc_mask <<= 4;// swap andi, hopefully.
              }
            } else {
              if (port == 3) { //cpse rjmp .+4
                fc_mask <<= bit_mask << 2; // lsr lsr
              }
            }
          #else
            if (((tcdmux & 0x07) != 0)) {
              /* On these parts, there is no available silicon with a working TCD portmux! */
              if (val < 128) {
                _setValueLow(portnum, bit_mask);
              } else {
                _setValueHigh(portnum, bit_mask);
              }
              _setOutput(portnum, bit_mask); // remove this or replace with errata test if it turns out that the direction override is errata and will befixed.
              break;
            }
          #endif

          // 128 input  with the max set to 1019 should get us 509. This was WRONG.
          // We need to subtract from 256 now, leaving us with a number from 1 to 256
          uint8_t temp = TCD0.CMPBCLRL;
          temp = TCD0.CMPBCLRH;
          //
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

        #if defined(NO_GLITCH_TIMERD0)
          volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(port), digitalPinToBitPosition(pin));
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
        }
      #endif
    }
  // now hastily set the pin output with this quickie macro since we know alll we need in order to do so now.
  }
  //_setOutput(portnum, bit_mask);
}
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

uint8_t digitalPinToTimerNow(uint8_t p) {
  uint8_t bit_pos = digitalPinToBitPosition(p);
  if (bit_pos == NOT_A_PIN) return NOT_ON_TIMER;     /* Use bit position to check for invalid pins */
  uint8_t port = digitalPinToPort(p);                /* If bit_pos is valid, port will be too      */
  if ( bit_pos < 6) {                                /* SPLIT MODE TCA output is on pins 0-5       */
  #if defined(TCA1)
    uint8_t tcamux = PORTMUX.TCAROUTEA;
    if ( __PeripheralControl & TIMERA0) {              /* make sure user hasn't taken over TCA0      */
      if (((tcamux & PORTMUX_TCA0_gm) == port)) {    /* TCA0 mux is EASY - same as the port number */
        return TIMERA0;
      }
    }
    tcamux &= 0x18;
    if (__PeripheralControl & TIMERA1) {               /* make sure user hasn't taken over TCA0      */
      if ((tcamux == 0 && port == PB ) || (tcamux == 0x18 && port == PG)) { /* supports only 6-ch  */
        return TIMERA1;                              /* mux options, not 3-channel ones on bit 4:6 */
      }
    }
  #elif defined(TCA0)
    if (__PeripheralControl & TIMERA0) {               /* here we don't need to store tcamux */
      if ((PORTMUX.TCAROUTEA & PORTMUX_TCA0_gm) == port) { /* because it is only used once */
        return TIMERA0;
      }
    }
  #else
    /* EB-series doesn't have any TCA's!
     * Instead, they've got WEX and his crony TCE
     * we'll track their position from this lookout
     * Unless my crystal ball has given out again,
     * TCE and WEX will move much like a TCA, as a whole
     * Not like TCA channels on tiny, or TCB channels.
     */
  #endif
  }
  uint8_t timer = digitalPinToTimer(p);
  #if defined(TCD0)
    if ( __PeripheralControl & TIMERD0) {
      if (timer & TIMERD0) {
        byte tcdmux = (PORTMUX.TCDROUTEA & PORTMUX_TCD0_gm);
        if (tcdmux == (timer & ~TIMERD0)) {
          return TIMERD0;
        }
      }
    }
  #endif
  #if defined(TCF0)
    /* The EB also has something called a TCF that might generate waveform output.
     * Nothing is known about it other than it's home pins and that it's letter of
     * the alphabet. Time will tell if that F'ing timer is any good for PWM, or if
     * it's an F'ing utility timer like TCB - lord knows with how stingy they were
     * with peripherals on the EB, it needs it.
     */
  #endif
  if (timer & TIMERB0) { /* Finally check TCBn, if we made it here w/out returning */
    TCB_t* timer_B;
    timer_B = ((TCB_t *)&TCB0 + (timer - TIMERB0)); /* get timer struct */
    if (((timer_B->CTRLB) &  TCB_CNTMODE_gm) != TCB_CNTMODE_PWM8_gc )
      return NOT_ON_TIMER; /* If the timer isn't in PWM mode, user has reconfigured
                              it, and nothing good can come of trying to use it. */
  }
  return timer;
}
