/*  (C) Spence Konde 2022
 * wiring_extra.cpp contains a few functions that are the sort of thing you'd expect in wiring.c or similar
 * but which need to be in a cpp file because they use overloading or are just not closely related to any of
 * those files' other functions.

 * This is part of DxCore - github.com/SpenceKonde/DxCore
 * This is free software, LGPL 2.1 see ../../LICENSE.md for details.
 *************************************************************/
#include "Arduino.h"

#if defined(LTODISABLED)
  void badArg(__attribute__((unused))const char* c) {
    return;
  }
  void badCall(__attribute__((unused))const char* c) {
    return;
  }
#endif

void badCore(const char*)   __attribute__((error("")));

#if !defined(LTODISABLED)
void checkCore(uint8_t a) {
  if (!__builtin_constant_p(a)) {
    badCore("If LTO is disabled, you *MUST* inform the core by defining DISABLELTO.");
  }
}
void _checkCore() {
  checkCore(1);
}
#endif
void __pinconfigure(const uint8_t digital_pin, uint16_t pin_config) {

  uint8_t bit_mask = digitalPinToBitMask(digital_pin);
  if(bit_mask == NOT_A_PIN || !pin_config) { // Return if digital pin is invalid or the other parameters or out to zero
    return;
  }
  uint8_t bit_pos  = digitalPinToBitPosition(digital_pin);
  volatile uint8_t *portbase = (volatile uint8_t*) digitalPinToPortStruct(digital_pin);

  // Write to selected pin direction register
  uint8_t setting = pin_config & 0x03; // Mask out direction bits (DIR, DIRSET, DIRCLR, DIRTGL)
  if(setting)
    *(portbase + setting) = bit_mask;

  // Write to selected output register
  pin_config >>= 2;
  setting = pin_config & 0x03;
  if(setting) {
    *(portbase + 4 + setting) = bit_mask;
  }

  // Return if there is nothing more to configure
  if(!(pin_config & 0x3FFC)) {
    return;
  }

  /* Now it gets to the fun part: The bits of PINnCTRL!
   * For this we need to disable interrupts
   * (for efficiency) we only read the register once
   * modify that value, then write it back at the end.
   */

  uint8_t oldSREG = SREG;             // Store SREG
  cli();
  pin_config >>= 2;
  uint8_t pinncfg = *(portbase + 0x10 + bit_pos);
  // Input sense configuration (ISC)
  if (pin_config & 0x08) {
    pinncfg = (pinncfg & 0xF8) | (pin_config & PORT_ISC_gm);
  }
  // Pullup resistor
  uint8_t temp = pin_config & 0x30;
  if(temp) {                          // Toggle of pullup? I guess...
    if(temp == 0x30) {
      pinncfg ^= PORT_PULLUPEN_bm;    // Toggle pullup
    } else if(temp == 0x20) {
      pinncfg &= ~(PORT_PULLUPEN_bm); // Clear pullup
    } else {
      pinncfg |= PORT_PULLUPEN_bm;    // Set pullup
    }
  }
  pin_config >>= 8;
  #ifdef PORT_INLVL_bm
  /* This feature is present on the AVR DB/DD-series, and headers suggest EA-series.
   * See Ref_digital.md in the online documentation for more information.
   *
   * Set or clear only - toggle not supported.
   * I question the usefulness of the other PINnCTRL toggles./
   * I do not question the value of an INLVL toggle; It's approximately zero.
   *
   * If you want a toggle INLVL option, you're in an X-Y problem and are going about something
   * sufficiently wrong that giving you what you think you need does you a disservice.
   * If you disagree with this design decision please create an issue and explain the use case that would make this
   * a worthwhile feature.
   */
  temp = pin_config & 0x03;
  if (temp) {                       // We don't support toggling INLVL; It is not plausibly useful.
    if (temp == 0x01) {
      pinncfg |=   0x40;            // set INLVL
    } else {
      pinncfg &= ~(0x40);           // clear INLVL
    }
  }
  #endif
  temp = pin_config & 0x0C;
  if(temp) {                          // Toggle of INVEN? I guess...
    if(temp == 0x0C)
      pinncfg ^= PORT_INVEN_bm;       // Toggle INVEN
    else if(temp == 0x08)
      pinncfg &= ~(PORT_INVEN_bm);    // Clear INVEN
    else
      pinncfg |= PORT_INVEN_bm;       // Set INVEN
  }
  // Write to PINnCTRL register
  *(portbase + 0x10 + bit_pos) = pinncfg;

  // Restore SREG
  SREG = oldSREG;
}
/* Error Checking */
void _pinconfigure(uint8_t pin, uint16_t pin_config) {
  check_valid_digital_pin(pin);

  #if defined(HARDWIRE_INPUT_ONLY)
    if   (__builtin_constant_p(pin) && pin == HARDWIRE_INPUT_ONLY) {
      if (__builtin_constant_p(pin_config)) {
        if (pin_config & 0x01) {
          badArg("This pin cannot be set as an output because of hardware constraints on the board selected and must always be INPUT only");
        }
      } else {
        if (pin_config & 0x01) {
          pin_config = pin_config & 0xFFFC;
        }
      }
    } else {
      if (__builtin_constant_p(pin_config)) {
        if (pin_config & 0x01) {
          if (pin == HARDWIRE_INPUT_ONLY) {
            pin_config = pin_config & 0xFFFC;
          }
        }
      } else {
        if (pin_config & 0x01 && pin == HARDWIRE_INPUT_ONLY) {
          pin_config = pin_config & 0xFFFC;
        }
      }
    }
  #endif
  __pinconfigure(pin, pin_config);
}
/* Allow calls with manually OR'ed constants */
void pinConfigure(uint8_t digital_pin, uint16_t pin_config) {
  _pinconfigure(digital_pin, pin_config);
}
/* This may end up somewhere else (like in the library*/
#if defined(PORTA_EVGENCTRL) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.
  uint8_t _setEventPin(uint8_t pin, uint8_t chan) {
    // Works the same was as
    uint8_t temp = digitalPinToPort(pin)
    if (temp != NOT_A_PIN && (chan + 1) < 3) {
      volatile uint8_t* p;
      p=(volatile uint8_t*)(uint16_t)(digitalPinToPortStruct(temp));
      p += 0x18; // now p pointing to evgenctrl.
      uint8_t oldsreg = SREG;
      cli();
      uint8_t r = *p; // r is current setting
      if (chan == 255) {
        if (!(r & 0x08)) {
          chan = 0;
        } else if (!(r & 0x80)) {
          chan = 1;
        } else {
          chan = PERIPHERAL_IN_USE;
        }
      }
      if (chan < 2) {
        uint8_t bp = 0x08 | digitalPinToBitPosition(pin);
        if (chan) {
          _SWAP(bp);
          r &= 0x0F;
        } else {
          r &= 0xF0;
        }
        r |= bp
      }
      *p = r;
      SREG = oldsreg;
      return chan;
    }
    return 255;
  }
  uint8_t _getPortEventConfig(uint8_t port) {
    uint8_t temp = digitalPinToPort(pin)
    return *(((uint8_t*) temp) + 0x18);
  } // just shorthand for looking up the port and returning it's EVGENCTRL value

  uint8_t _setRTCEventChan(uint8_t val, uint8_t chan) {
    /* val is one of:
      RTC_EVGEN0SEL_OFF_gc      = (0x00<<0),  // No Event Generated
      RTC_EVGEN0SEL_DIV4_gc     = (0x01<<0),  // CLK_RTC divided by 4
      RTC_EVGEN0SEL_DIV8_gc     = (0x02<<0),  // CLK_RTC divided by 8
      RTC_EVGEN0SEL_DIV16_gc    = (0x03<<0),  // CLK_RTC divided by 16
      RTC_EVGEN0SEL_DIV32_gc    = (0x04<<0),  // CLK_RTC divided by 32
      RTC_EVGEN0SEL_DIV64_gc    = (0x05<<0),  // CLK_RTC divided by 64
      RTC_EVGEN0SEL_DIV128_gc   = (0x06<<0),  // CLK_RTC divided by 128
      RTC_EVGEN0SEL_DIV256_gc   = (0x07<<0),  // CLK_RTC divided by 256
      RTC_EVGEN0SEL_DIV512_gc   = (0x08<<0),  // CLK_RTC divided by 512
      RTC_EVGEN0SEL_DIV1024_gc  = (0x09<<0),  // CLK_RTC divided by 1024
      RTC_EVGEN0SEL_DIV2048_gc  = (0x0A<<0),  // CLK_RTC divided by 2048
      RTC_EVGEN0SEL_DIV4096_gc  = (0x0B<<0),  // CLK_RTC divided by 4096
      RTC_EVGEN0SEL_DIV8192_gc  = (0x0C<<0),  // CLK_RTC divided by 8192
      RTC_EVGEN0SEL_DIV16384_gc = (0x0D<<0),  // CLK_RTC divided by 16384
      RTC_EVGEN0SEL_DIV32768_gc = (0x0E<<0)   // CLK_RTC divided by 32768

      chan is one of:
      0  (channel 0)
      1  (channel 1)
      -1 (whichever is unused)

      This may return:
      0  (channel 0 was set)
      1  (channel 1 was set)
      PERIPHERAL_IN_USE (you asked for an unused channel, but they are both in use already generating different frequencies)
      CHANNEL0_UNCHANGED (nothing was set - you asked for an unused channel, but channel 0 is already set to the specified value)
      CHANNEL1_UNCHANGED (nothing was set - you asked for an unused channel, but channel 1 is already set to the specified value)
      NOT_A_CHANNEL (nothing was set - a channel number that isn't a channel was specified)

      Intended usage:
      int8_t temp = _setRTCEventChan(myvalue, -1);
      if (temp < 0) {
        // handle error
      } else {
        mychannel = temp;
      }

      Then when using the channel, bitwise AND it with 0x01, so you get 0 or 1 from a CHANNEL0_UNCHANGED or CHANNEL1_UNCHANGED.
      volatile uint8_t* mygenerator = &EVSYS_CHANNEL0; // in practice you'd be hopefully calling something to find a free generator...

      *mygenerator = ((mychannel & 0x01) ? EVSYS_PITEV1 : EVSYS_PITEV0);

      Later, when done with the RTC channel, you can release it with:

      _setRTCEventChan(0, mychannel);

      This will leave it unchanged if it returned CHANNEL0_UNCHANGED or CHANNEL1_UNCHANGED because in that case something else was already using the RTC for something
      and you were just piggybacking on that, so you wouldn't want to trash that.
      Note that this scheme only works if the first thing using the timer event channels doesn't stop the channel befopre
    */
    if   (__builtin_constant_p(val)) {
      if (val >= 0x0F) {
        badArg("First argument to _setRTCEventChan must be less than 16, but it is known at compile-time that an invalid value will be passed.");
        return NOT_A_CHANNEL;
      }
    } else {
      if (val >= 0x0F) {
        return NOT_A_CHANNEL;
      }
    }
    if   (__builtin_constant_p(chan)) {
      if (chan != 255 && chan != 0 && chan != 1) {
        badArg("Second argument to _setRTCEventChan must be 0, 1, or 255 (-1), but it is known at compile-time that an invalid value will be passed.");
        return NOT_A_CHANNEL;
      }
    } else {
      if (chan > 2) && chan != 255 {
        return NOT_A_CHANNEL;
      }
    }
    uint8_t oldsreg = SREG;
    cli(); //gotta disable interrupts while using the evgenctrl value - this needs to be atomic
    uint8_t r = RTC.PITEVGENCTRLA; // this approach takes 5 clocks and 8 bytes plus the manipulations. Doing it in situ would at best be similar, but could at worst be
    if (chan == 255) {
      uint8_t valsw = val;
      _SWAP(valsw);
      if         (((r & 0x0F) == val)) {
        chan = CHANNEL0_UNCHANGED; // 64
      } else if  ((r & 0xF0) == valsw) {
        chan = CHANNEL1_UNCHANGED; // 65
      } else if (!(r & 0x0F)) {
        chan = 0;
      } else if (!(r & 0xF0)) {
        chan = 1;
      } else {
        chan = PERIPHERAL_IN_USE;
      } // chan = 255/-1 has been transformed into 0 or 1 if there was an available generator output, or PERIPHERAL_IN_USE/254/-2 otherwise
    }
    // Notice that chan is now 0, 1, 64, 65, or 254.
    if (chan < 2) {
      if (chan) {  // chan = 1
        _SWAP(val); // low nybble is channel 0, high is channel 1
        r &= 0x0F;
      } else {     // chan = 0
        r &= 0xF0;
      }
      r |= val;
      RTC.PITEVGENCTRLA = r;
    }
    SREG = oldsreg;
    return chan; // hence this will return the channel or PERIPHERAL_IN_USE.
  }
  uint8_t _getRTCEventConfig() { //simply returns the RTC channel configuration.
    return RTC.PITEVGENCTRLA;
  }
  uint8_t _RTCPrescaleToVal(uint16_t prescale) {
    if ((prescale & (prescale - 1)) != 0) {
      return NOT_A_CHANNEL;
    }
    if (prescale < 4) {
      return NOT_A_CHANNEL;
    }
    uint8_t ret = 255;
    __asm__ __volatile__ (
      "cpi  %1A,   0"   "\n\t" // see if low byte is 0.
      "brne .+6"        "\n\t" // if low byte is not equal to 0, then high byte is 0, so we don't need to loop over a 2 byte value
      "ldi   %0,   6"   "\n\t" // if it is, then value must be 7 or more, so load 6 into it
      "mov  %1A, %1B"   "\n\t" // and copy the high byte which we know has a single 1 bit...
      "inc  %0"         "\n\t" // Increment ret. After this, is 7, 8, 9 ... if low byte was zero (and high byte is now where low byte was), or 0, 1, 2 ... if it wasn't.
      "lsr  %1A"        "\n\t" // shift the byte we care about (handled above). We know that if the low byte was non zero, the first two bits must be 0 so we want an extra lsr.
      "brcc .-6"        "\n\t" // goes back to the inc unless carry bit was set by the lsr shifting out a 1. So after each increment we check if a carry bit was set.
                               // The brne lands on the lsr if low byte wasn't 0, shifting out the first 0, then it increments ret to 0 and shifts out second 0,
                               // So if original prescale was 4, that would get shifted into the carry bit as soon as ret was incremented to 1. Perfect.
                               // And if the prescale was 256, 6 will increment to 7 as a 1 is shifted out, Perfect.
      :"+d" ((uint8_t)ret),
      "+d" ((uint16_t)prescale) //doesn't matter that we shit on prescale, it's not referenced again and was passed by value, so what we do to it doesn't matter.
    );
    return ret;
  }
  uint16_t _getRTCPrescale(uint8_t chan) {
    if (chan < 2) {
      uint8_t readval = (chan ? 0xF0 : 0x0F);
      if (chan == 1) {
        _SWAP(readval);
      }
      if (readval) {
        return ((uint16_t) 2) << readval;
      }
      return 0;
    } else {
      return -1;
    }
  }
#endif
