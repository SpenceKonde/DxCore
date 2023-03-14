#include "Arduino.h"
#include "DxCore.h"

// *INDENT-OFF*
void configXOSC32K(const X32K_OPT_t settings, const X32K_ENABLE_t enable) {
  #if (_AVR_PINCOUNT < 28 && CLOCK_SOURCE > 0)
    badCall("You cannot use an external low frequency crystal if system clock is from an external high frequency crystal without at least 28 pins.");
    badCall("How were you planning to wire the hardware? You can't have two crystals on PA0 and PA1, but this part doesn't have PF0/PF1.")
  #else
    uint8_t newval = settings | enable;
    // if any of the bits are "enable protected" we need to turn off the external crystal/clock.
    if ((CLKCTRL.XOSC32KCTRLA ^ newval) & (CLKCTRL_SEL_bm | CLKCTRL_CSUT_gm)) {
      _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA & 0xFE); // disable external crystal
      while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm); // unclear if this is immediately cleared or not...
    }
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, newval);
  #endif
}


uint8_t enableAutoTune() {
  if ((CLKCTRL.MCLKCTRLA & 0x0F) != 0) {
    return 0xFF;
  }
  if (CLKCTRL.XOSC32KCTRLA & 0x01) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, X32K_HIGHPWR_START2S | X32K_ENABLED);
  }
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL.OSCHFCTRLA | 0x01);
  uint8_t csut = (CLKCTRL.XOSC32KCTRLA & CLKCTRL_CSUT_gm) >> 4;
  uint16_t verifytime = 500 + ((csut == 3) ? 2000 : (500 * csut));
  #if defined(MILLIS_USE_TIMERNONE)
    while (verifytime && (!(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm))) {
      delay(1);
      verifytime--;
    }
    if (verifytime) {
      return 0;
    }
  #else
    uint32_t startedAt = millis();
    while ((millis() - startedAt < verifytime) && (!(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)));
    if (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) {
      return 0;
    }
  #endif
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL.OSCHFCTRLA & 0xFE);// turn it back off - crystal not working
  return 1;
}


// uint8_t disableAutoTune()
// Returns 255 (-1) if autotune was not enabled.
// Returns 0 if autotune is successfully disabled.
int8_t disableAutoTune() {
  if (CLKCTRL.OSCHFCTRLA & 0x01) {
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL.OSCHFCTRLA & 0xFE);
    return 0x00;
  } else {
    return 0xFF;
  }
}
#endif

bool setTCA0MuxByPort(const uint8_t port) {
  if (port < 7) {
    TCA0.SPLIT.CTRLB = 0; // disconnect all pins on the port from timer.

    uint8_t base_pin = portToPinZero(port);
    uint8_t max_pin = min((uint8_t)(NUM_DIGITAL_PINS - 1), base_pin + 5);
    for (byte i = base_pin; i < (min(max_pin, base_pin + 6); i++)) {
      turnOffPWM(i);
    }
    PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & (~PORTMUX_TCA0_gm)) | port;
    return true;
  }
  return false;
}

bool setTCA0MuxByPin(const uint8_t pin) {
  if (digitalPinToBitPosition(pin) < 6) {
    return setTCA0MuxByPort(digitalPinToPort(pin));
  }
  return false;
}

#ifdef TCA1
  bool setTCA1MuxByPort(const uint8_t port) {
    uint8_t three_pin = 0;
    uint8_t muxval = 0;
    #if defined(DB_64_PINS)
      //     B               G              C                    E    || ( EA )   )A
      if (!(port == 1) || (port == 6) || (port == 2) || (port == 4) ) {
        return false;
      } else {
        switch (port) {
          case 6: {
            muxval = 3
          }
          case 1: { //muxval either 0 or 3
            break;
          }
          case 2: {
            three_pin = 1;
            muxval = 1;
            break;
          }
          case 4: {
            three_pin = 1;
            muxval = 2;
            break;
          }
        }
      }
      // if it's not one of the 4 without a mux option on TCA1 on DB64
    #elif defined(__AVR_EA__)
      #if _AVR_PINCOUNT > 32
        // 48-pin EA-series.
        if (port == 1) {
          muxval = 0;
        } else
      #endif
      if (port == 2) {  // PORTC
        three_pin = 1;
        muxval = 1;
      } else if (port == 0) { // PORTA
        three_pin = 1;
        muxval = 4;
      } else if (port == 3) { // PORTD
        three_pin = 1;
        muxval = 5;
      } else {
        return false;
      }
    #else
      // It's not an EA, and it's not a DB with 64 pins, so it's gotta be an DA with 64 pins, or DA/DB with 48 pins, which are the same for PWM purposes - either we don't have the pins or the mux options are busted.
      if (port == 2) {
        three_pin = 1;
        muxval = 1;
      } else if (port != 1) {
        return false;
      }
    #endif
    // AND with group mask cuts off the unwanted low bit leaving us with the 2 high bits which is what we care about
    TCA1.SPLIT.CTRLB  = 0; // disconnect all PWM channels
    PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & (~PORTMUX_TCA1_gm)) | (muxval << 3);
    uint8_t base_pin  = portToPinZero(port);
    uint8_t max_pin   = base_pin + (three_pin ? 3 : 6);
    max_pin           = min((max_pin - 1), (NUM_DIGITAL_PINS - 1));
    for (byte i = base_pin; i < max_pin; i++) {
      // this is rather expensive
      turnOffPWM(i);
    }
    return true;
  }

  bool setTCA1MuxByPin(const uint8_t pin) {
    uint8_t port = digitalPinToPort(pin);
    uint8_t bit_mask = digitalPinToBitMask(pin);
    #if defined(DB_64_PINS)
      // AVR128DB, AVR64DB work with the high MUX options
      if (((port == 1   || port == 6) && (bit_mask & 0x3F)) || (bit_mask & 0x70)) {
    #else  // AVR128DA64 definitely do not work. AVR64DA64 untested.
      if (((port == 1 /*  errata  */) && (bit_mask & 0x3F)) || (bit_mask & 0x70)) {
    #endif // And those are the only 4 parts in the product line for which those pins exist. *INDENT-ON*
    // PORTB and PORTG have full-service TCA1 (well, not PG on the 128DA63 up to at least the A8 die
    // rev). for those, bit_mask will be 0x01, 0x02, 0x04, 0x08, 0x10, or 0x20 - but not 0x40 or
    // 0x80. Hence test against 0x3F works. For the others, it is either 0x10, 0x20, or 0x40 so
    // test against 0x70; the port function will check that the non-B/G port is valid.
      return setTCA1MuxByPort(port);
    }
    return false;
  }
#endif // TCA1

bool setTCD0MuxByPort(uint8_t port) {
  #if defined(__AVR_DD__)
    if (!(port == 0 || port == 5 || port == 3))
      return false;
    if (port == 5) {
      port = 2;
    } else if (port == 3) {
      port = 4;
    }
    PORTMUX.TCDROUTEA = port;
    return true;
  #else
    if (port == 0) { // See errata; appears to be broken on all parts, not just 128k ones. Return false unless the one working port was requested.
      PORTMUX.TCDROUTEA = 0;
      return true;
    }
  #endif
  return false;

}
// bool setTCD0MuxByPin(uint8_t pin)
bool setTCD0MuxByPin(uint8_t pin) {
  #if defined(__AVR_DD__)
    uint8_t bitmask = digitalPinToBitMask(pin);
    if (bitmask == NOT_A_PIN) {
      return false;
    }
    uint8_t port = digitalPinToPort(pin);
    if (bitmask & 0x0F) {
      if (port != 5) { //PORTF has it on pins 0-3
        return false;
      }
    } else {
      // If we're here, bitmask ==
      if (port == 5) {
        if (!(bitmask & 0x30)) {
          return false;
        }
      } else if (!(bitmask & 0xF0)) {
        return false;
      }
    }
    return setTCD0MuxByPort(digitalPinToPort(pin));
  #elif !defined(TCD0)
    badCall("This part does not have a type D timer!");
  #else
    // it's a DA/DB with busted mux, which makes this quite easy:
    uint8_t port = digitalPinToPort(pin);
    if (port != 0) { //only port a works!
      return false;
    }
    uint8_t bitmask=digitalPinToBitMask(pin);
    if (!(bitmask & 0xF0)) {
      return setTCD0MuxByPort(digitalPinToPort(pin)); // See errata; appears to be broken on all parts, not just 128k ones. So, if it's not pin 4-7, it's no good.
    }
  #endif
}


int16_t getMVIOVoltage() {
  uint8_t status = getMVIOStatus();
  if (status == MVIO_OKAY) {
    uint8_t tempRef = VREF.ADC0REF; // save reference
    analogReference(INTERNAL1V024);  // known reference
    analogRead(ADC_VDDIO2DIV10); // exercise the ADC with this reference
    int32_t tempval = analogReadEnh(ADC_VDDIO2DIV10,13); // 0-8191,  8191 = 10240mv (obv not possible im practice);  So we want a multipication by 1.25 if not an error.
    VREF.ADC0REF = tempRef; // restore reference
    if (tempval < 0) {
      tempval += 2099990000; // make error numbers from enhanced reads fit in int16.
      // errors will be numbered -10000, -10001, etc
      return tempval;
    } else {
      // temp val should thus be 0-4095 unless analogReadEnh is broken.
      // multiply by by 1.25 the fast way.
      uint16_t retval = tempval;
      return retval + (retval >> 2);
    }
  } else {
    // sadly, if it's undervoltage, you can't get read of VDDIO2 this way!
    return status;
  }
}

void _enable_opamp(const uint8_t opamp) {
  DAC0.CTRLA |= 0x41;
  OPAMP.TIMEBASE = F_CPU / 1000000;
  volatile uint8_t* opamp_ptr = (volatile using8_t*) 0x710 + 8 * opamp;
  *(opamp_ptr) = 0x05; //enabled and on
  *(opamp_ptr + 3) = 0x22; // INMUX selects OUTPUT as negative and DAC as positive voltage.
  *(opamp_ptr + 4) = 0x7F; // maximum settle time
  OPAMP.CTRLA = 1; //enable opamps
}

int16_t _refnum_to_max_dacout(const uint8_t refnum) {
  if (refnum >= 0x04) {
    return 0;
  } else if (refnum == 0) {
    return 925;
  } else if (refnum == 1) {
    return 1950;
  } else if (refnum == 3) {
    return 2400;
  } else if (refnum == 2) {
    return 4000;
  }
}

int16_t initMVIO_OPAMP(const int16_t voltage, const int8_t opamp,  const uint8_t options, const int16_t dacref) {
  #if (!defined(MVIO) || !defined(OPAMP))
    return MVIO_UNSUPPORTED;
  #else
    if FUSE.SYSCFG1 & 0x18 != 0x08 {
      return MVIO_DISABLED;
    }
    if (voltage < 1650) {
      return MVIO_UNDERVOLTAGE;
    }
    if (voltage > 5500 || (dacref > 5500 && (options & 0x01 == 0))) {
      return MVIO_VOLTAGE_TOO_HIGH;
    }
    if (options != MVIO_SELECT_REF) {
      if (dacref != -1 && dacref < 1750) {
        return (MVIO_REF_TOO_LOW | MVIO_UNDERVOLTAGE);
      } else {
        uint8_t tempRef = getDACReference();
        if (dacref == -1) {
          if (tempRef < 0x04) {
            int16_t maxvoltage = _refnum_to_max_dacout(tempRef);
            if (maxvoltage < voltage) {
              return MVIO_REF_TOO_LOW;
            }
            int32_t tempval = 1023 * voltage;
            tempval = tempval / maxvoltage;
            DAC0.DATA = tempval;
            _enable_opamp(opamp);
          } else { // otherwise it's set to an external or vdd reference, but we haven't told it the voltage
            return MVIO_IMPOSSIBLE_CFG;
          }
        } else {
          if (tempRef > 0x04) {
            int32_t tempval = 1023 * voltage;
            tempval = tempval / dacref;
            DAC0.DATA = tempval;
            _enable_opamp(opamp);
          } else {
            return MVIO_IMPOSSIBLE_CFG;
          }
        }
      }
    }
  } else {
    if (voltage > 4000) {
      return MVIO_NO_VALID_REF;
    } else {
      uint8_t ref = 0;
      while (ref < 4 && _refnum_to_max_dacout(ref) < voltage) {
        ref = (ref == 1 ? 3 : (ref == 3 ? 2 : (ref == 2 ? 4 : (ref + 1))));
      }
      if (ref >= 4) {
        return MVIO_NO_VALID_REF;
      }
      int32_t tempval = 1023 * voltage;
      tempval = tempval / _refnum_to_max_dacout(ref);
      DACReference(ref);
      DAC0.DATA = tempval;
      _enable_opamp(opamp);
    }
  }
  return MVIO_OK;
  #endif
}



#ifdef MVIO
  uint8_t getMVIOStatus(bool debugmode, HardwareSerial &dbgserial) {
    uint8_t retval = FUSE.SYSCFG1 & 0x18;
    if (retval == 0x08) {
      //great, it's enabled.
      retval = (MVIO.STATUS ? MVIO_OKAY : MVIO_UNDERVOLTAGE)
      #if !defined(MVIO_ENABLED) && defined(ASSUME_MVIO_FUSE) && defined(USING_OPTIBOOT)
        retval |= MVIO_MENU_SET_WRONG;
        if (debugmode) {
          dbgserial.println(F("Woah, you've told the tools menu that you're sure you set the fuse to disabled, but... you didn't"));
          dbgserial.println(F("Either change menu selection to enabled or the other disabled (which allows for mis-set fuses by checking), or burn bootloader if you want it disabled"));
          dbgserial.println(F("Some core functions relating to the MVIO pins will malfunction until you do one of those things."));
        }
      #elif !defined(MVIO_ENABLED) && defined(USING_OPTIBOOT)
        retval |= MVIO_SETTING_MISMATCH;
      #elif !defined(MVIO_ENABLED) && !defined(USING_OPTIBOOT)
        retval |= MVIO_MENU_SET_WRONG | MVIO_IMPOSSIBLE_CFG;
        if (debugmode) {
          dbgserial.println(F("Your tools submenu is set to disable MVIO, and you are not using Optiboot, so MVIO should have been disabled"));
          dbgserial.println(F("when the sketch was uploaded via UPDI. Yet that is not the case. If you are using a third party IDE, it is misconfigured."));
          dbgserial.println(F("You must not specify MVIO as being enabled or disabled if you have not set the fuses that way (details depend on IDE)"));
          dbgserial.println(F("If you are using Arduino IDE, this is a bug in DxCore that should be reported promptly."));
          dbgserial.println(F("Some core functions relating to the MVIO pins will malfunction until your fuses match the menu selection."));
        }
      #endif
    } else if (retval == 0x10) {
      retval = MVIO_DISABLED;
      if (MVIO.STATUS != 1 || MVIO.INTFLAGS !=0) {
        if (debugmode)
          dbgserial.println(F("Impossible hardware situation: MVIO is disabled by fuse, but MVIO.STATUS is not 1, directly contradicting the datasheet"));
          dbgserial.println(F("You should raise this issue with Microchip, and also create a DxCore issue to track it as it relates to the core."));
          return retval |= MVIO_IMPOSSIBLE_CFG;
        }
      }
      #if defined(MVIO_ENABLED) && defined(ASSUME_MVIO_FUSE) && defined(USING_OPTIBOOT)
        retval |= MVIO_MENU_SET_WRONG;
        if (debugmode) {
          dbgserial.println(F("Woah, you've told the tools menu that you're sure you set the fuse to enabled... but you didn't. It's disabled. "));
          dbgserial.println(F("Either change menu selection to disabled, or burn bootloader if you want it enabled"));
          dbgserial.println(F("Some core functions relating to the MVIO pins will malfunction until you do one of those things."));
        }
      #elif !defined(MVIO_ENABLED) && defined(USING_OPTIBOOT)
        retval |= MVIO_SETTING_MISMATCH;
      #elif defined(MVIO_ENABLED) && !defined(USING_OPTIBOOT)
        retval |= MVIO_MENU_SET_WRONG | MVIO_IMPOSSIBLE_CFG;
        if (debugmode) {
          dbgserial.println(F("Your tools submenu is set to enable MVIO, and you're not using optiboot, so MVIO should have been enabled"));
          dbgserial.println(F("when the sketch was uploaded via UPDI, but it's not. If you are using a third party IDE, it is misconfigured."));
          dbgserial.println(F("You must not specify MVIO as being enabled or disabled if you have not set the fuses that way (details depend on IDE)"));
          dbgserial.println(F("If you are using Arduino IDE, this is a bug in DxCore that should be reported promptly."));
          dbgserial.println(F("Some core functions relating to the MVIO pins will malfunction until your fuses match the menu selection."));
        }
      #endif
    } else {
      // The fuse was set to neither 0x10 or 0z08. Those are the only valid values according to the datasheet, and the only ones that
      // DxCore is supposed to set them to.
      retval = MVIO_BAD_FUSE | MVIO_IMPOSSIBLE_CFG;
      if (debugmode) {
        dbgserial.println(F("The MVIO bits in FUSE.SYSCFG1 are set to an invalid value. This should never happen."));
        dbgserial.println(F("The bits should be either 01 (enable) or 10 (disable). Never 11 or 00, whose behavior is undefined. "))
        dbgserial.println(F("If you are using a third party tool or IDE to set the fuses, it is misconfigured."));
      }
    }
    return retval;
  }
#else
  uint8_t getMVIOStatus(__attribute__((unused))bool debugmode, __attribute__ ((unused)) HardwareSerial &dbgserial) {
    return MVIO_UNSUPPORTED;
  }
#endif




// Not ready yet, but we ought to have something like this.

//int8_t timerToDigitalPin(uint8_t timer, uint8_t channel, bool muxed) {

  /* valid values for timer start at line 300ish in Arduino.h.
   * NOT RECOMMENDED FOR CODE WHERE PERFORMANCE IS IMPORTANT. It's great for making library example code work.
   * valid values for channel are:
   * TCA: 0-5
   * TCB: 0 - this function tells you where PWM is currently piped, not where it could be piped.
   * TCD: 0-3
   * valid values for mux are:
   * TCA0: 0-6 on 64pin parts, 0-5 on 48 pins parts, 0, 2, 3, and 5 on everything else: The mux value is the number of the port.
   * TCA: 0-1 on DA/DB 48, and in DA64 due to an erratum, 0-3 on DB64. EA series gets 2 more mux options so it looks like EA-series will get 0,1,4,5, though 0 will only be available on EA48s
   * TCB: 0 or 1
   */

  /*
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
        badArg("TCDs only have channels 0~3, a channel outide that range was requested!");
      }
      return NOT_A_PIN;
    } else if (PORTMUX.TCDROUTEA != (timer & 0x03)) {
      return TIMER_NOT_CONNECTED;
    }
    #if defined(ERRATA_TCD_PORTMUX)
      #if (!defined(PIN_PA4))
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
//}



// *INDENT-ON*
