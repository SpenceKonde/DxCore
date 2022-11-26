#include "Arduino.h"
#include "DxCore.h"

// *INDENT-OFF*
void configXOSC32K(X32K_OPT_t settings, X32K_ENABLE_t enable) {
  uint8_t newval = settings | enable;
  // if any of the bits are "enable protected" we need to turn off the external crystal/clock.
  if ((CLKCTRL.XOSC32KCTRLA ^ newval) & (CLKCTRL_SEL_bm | CLKCTRL_CSUT_gm)) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA & 0xFE); // disable external crystal
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm); // unclear if this is immediately cleared or not...
  }
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, newval);
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
  uint32_t verifytime = 500 + ((csut == 3) ? 2000 : (500 * csut));
  uint32_t startedAt = millis();
  while ((millis() - startedAt < verifytime) && (!(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)));
  if (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) {
    return 0;
  }
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

bool setTCA0MuxByPort(uint8_t port) {
  if (port < 7) {
    TCA0.SPLIT.CTRLB = 0; // disconnect
    uint8_t base_pin = portToPinZero(port);
    uint8_t max_pin = min((base_pin + 6 > NUM_DIGITAL_PINS ? NUM_DIGITAL_PINS : base_pin + 6), portToPinZero(port + 1)) - 1;
    for (byte i = base_pin; i < (min(max_pin, base_pin + 6); i++)) {
      turnOffPWM(i);
    }
    PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & (~PORTMUX_TCA0_gm)) | (port & PORTMUX_TCA0_gm);
    return true;
  }
  return false;
}

bool setTCA0MuxByPin(uint8_t pin) {
  if (digitalPinToBitPosition(pin) < 6) {
    return setTCA0MuxByPort(digitalPinToPort(pin));
  }
  return false;
}

#ifdef TCA1
  bool setTCA1MuxByPort(uint8_t port, bool __attribute__((unused)) takeover_only_ports_ok = false) {
    #if defined(DB_64_PINS)
      if (!((port == 1) || (port == 6) || ((port == 2) || (port == 4)))) {
        return false;
      }
      // not one of the 4 mapping options that we have on DB64. TCA1 mapping
      // for this port doesn't exist, port is invalid, or not a port if not caught by above #if
    #else
      // if not a DB, the PORTG mapping option doesn't work per errata...
      if (port != 1 && port != 2) {
        return false;
      }
    #endif
    // AND with group mask cuts off the unwanted low bit leaving us with the 2 high bits which is what we care about
    TCA1.SPLIT.CTRLB = 0; // disconnect all PWM channels
    uint8_t base_pin = portToPinZero(port);
    uint8_t max_pin = min((base_pin + 6 > NUM_DIGITAL_PINS ? NUM_DIGITAL_PINS : base_pin + 6), portToPinZero(port + 1)) - 1;
    for (byte i = base_pin; i < max_pin; i++) {
      turnOffPWM(i);
    }
    PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & (~PORTMUX_TCA1_gm)) | ((port << 2) & PORTMUX_TCA1_gm);
    return true;
  }

  bool setTCA1MuxByPin(uint8_t pin, bool  __attribute__((unused)) takeover_only_ports_ok = false) {
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
      return setTCA1MuxByPort(digitalPinToPort(pin), takeover_only_ports_ok);
    }
    return false;
  }
#endif // TCA1

bool setTCD0MuxByPort(uint8_t port, bool __attribute__((unused)) takeover_only_ports_ok = false) {
  #if defined(__AVR_DD__)
    if (!(port == 0 || port == 5 || port == 4))
      return false;
    if (port == 5) {
      port = 2;
    } else if (port == 4) {
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

bool setTCD0MuxByPin(uint8_t pin, bool takeover_only_ports_ok = false) {
  #if defined(__AVR_DD__)
    uint8_t bitmask = digitalPinToBitMask(pin);
    if (bitmask == NOT_A_PIN) {
      return false;
    }
    uint8_t port = digitalPinToPort(pin);
    if (bitmask & 0x0F) {
      if (port != 5) {
        return false;
      }
    } else if (port == 3) {
      if (!(bitmask & 0x30)) {
        return false;
      }
    } else if (!(bitmask & 0xF0)) {
      return false;
    }
  #elif !defined(TCD0)
    badCall("This part does not have a type D timer!");
  #else
    uint8_t bitpos=digitalPinToBitPosition(pin);
    if (bitpos < 4 || bitpos > 7 ) {
      return false;
    }
      return setTCD0MuxByPort(digitalPinToPort(pin), takeover_only_ports_ok); // See errata; appears to be broken on all parts, not just 128k ones. So, if it's not pin 4-7,
    }
    return false; // it's definitely no good. If it is 4-7, pass the other function to check port (though we could optimize further here, since
  #endif               // chips that one might want to call this for don't exist, let's not bother :-)
}


int16_t getMVIOVoltage() {
  if (getMVIOStatus() == MVIO_OKAY) {
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
      uint16_t retval=tempval; // truncate leading 0's.
      return retval + (retval >> 2);
    }
  } else {
    // sadly, if it's undervoltage, you can't get read of VDDIO2 this way!
    return getMVIOStatus();
  }
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
          Serial.print(F("Woah, you've told the tools menu that you're sure you set the fuse to enabled... but you didn't. It's disabled. "));
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
        dbgserial.println(F("The bits should be either 01 (enable) or 10 (disable). Never 11 or 00. Microchip specifies no behavior."))
        dbgserial.println(F("If you are using a third party tool or IDE to set the fuses, it is misconfigured."));
      }
    }
    return retval;
  }
#else
  uint8_t getMVIOStatus(__attribute__((unused))bool debugmode __attribute__ ((unused)) HardwareSerial &dbgserial {
    return MVIO_UNSUPPORTED;
  }
#endif
// *INDENT-ON*
