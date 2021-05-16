#ifndef DXCORE_H
#include <Arduino.h>
typedef enum X32K_TYPE {
  X32K_LOWPWR_START31MS   = (CLKCTRL_CSUT_1K_gc  | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START500MS  = (CLKCTRL_CSUT_16K_gc | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START1S     = (CLKCTRL_CSUT_32K_gc | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START2S     = (CLKCTRL_CSUT_64K_gc | CLKCTRL_LPMODE_bm),
  X32K_HIGHPWR_START31MS  = (CLKCTRL_CSUT_1K_gc),
  X32K_HIGHPWR_START500MS = (CLKCTRL_CSUT_16K_gc),
  X32K_HIGHPWR_START1S    = (CLKCTRL_CSUT_32K_gc),
  X32K_HIGHPWR_START2S    = (CLKCTRL_CSUT_64K_gc),
  X32K_EXTCLK             = (CLKCTRL_SEL_bm)
} X32K_TYPE_t;

typedef enum X32K_ENABLE {
  X32K_DISABLED = (0x00),
  X32K_ENABLED  = (0x01),
  X32K_ALWAYSON = (0x81)
} X32K_ENABLE_t;

// void configXOSC32K(X32K_TYPE_t, X32K_ENABLE_t)
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that this likely matters!)
void configXOSC32K(X32K_TYPE_t settings, X32K_ENABLE_t enable) {
  uint8_t newval = settings | enable;
  //if any of the bits are "enable protected" we need to turn off the external crystal/clock.
  if ((CLKCTRL.XOSC32KCTRLA ^ newval) & (CLKCTRL_SEL_bm | CLKCTRL_CSUT_gm)) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA & 0xFE); //disable external crystal
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm); //unclear if this is immediately cleared or not...
  }
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, newval);
}

// void disableXOSC32K()
// disables the external 32.768 kHz oscillator
void disableXOSC32K() {
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, (CLKCTRL.XOSC32KCTRLA & 0xFE));
}


// since CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

// uint8_t enableAutoTune()
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 1 second startu time and normal (not low power) crystal.
// Returns 1 if autotune was not successfully enabled - waited for CSUT + 0.5 seconds, and status still reported XOSC32K as not running/stable!
// Returns 255 (-1) if the current main clock source isn't the internal HF oscillator (presumably it's either external clock/crystal)
// under this circumstance, autotune will not impact the main clock - and the main clock, in fact, is likely more accurate than autotune would achieve.
// Returns 0 if autotune is successfully enabled within the time permitted by XOSC32KCTRLA's currently configured CSUT bits.

uint8_t enableAutoTune() {
  if ((CLKCTRL.MCLKCTRLA & 0x0F) != 0) {
    return 0xFF;
  }
  if (CLKCTRL.XOSC32KCTRLA & 0x01) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, X32K_HIGHPWR_START1S | X32K_ENABLED);
  }
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL.OSCHFCTRLA | 0x01);
  uint8_t csut = (CLKCTRL.XOSC32KCTRLA & CLKCTRL_CSUT_gm) >> 4;
  uint32_t verifytime = 500 + (csut == 3 ? 2000 : 500 * csut);
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
    TCA0.SPLIT.CTRLB = 0; //disconnect
    uint8_t base_pin = digitalPortToPinZero(port);
    uint8_t max_pin = min((base_pin + 6 > NUM_DIGITAL_PINS ? NUM_DIGITAL_PINS : base_pin + 6), digitalPortToPinZero(port + 1)) - 1;
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

bool setTCA1MuxByPort(uint8_t port, bool takeover_only_ports_ok = false) {
  #if defined(DB_64_PINS)
  if (!((port == 1) || (port == 6) || (((port == 2) || (port == 4)) && takeover_only_ports_ok))) {
    return false;
  }
  // not one of the 4 working mapping options that we have on DB64. TCA1 mapping
  // for this port doesn't exist, port is invalid, or not a port if not caught by above #if
  #else
  //if not a DB, the PORTG mapping option doesn't work per errata...
  if (port != 1 && (!(port == 2  && takeover_only_ports_ok))) {
    return false;
  }
  #endif
  // AND with group mask cuts off the unwanted low bit leaving us with the 2 high bits which is what we care about
  TCA1.SPLIT.CTRLB = 0; //disconnect all PWM channels
  uint8_t base_pin = digitalPortToPinZero(port);
  uint8_t max_pin = min((base_pin + 6 > NUM_DIGITAL_PINS ? NUM_DIGITAL_PINS : base_pin + 6), digitalPortToPinZero(port + 1)) - 1;
  for (byte i = base_pin; i < max_pin; i++) {
    turnOffPWM(i);
  }
  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & (~PORTMUX_TCA1_gm)) | ((port << 2) & PORTMUX_TCA1_gm);
  return true;
}
bool setTCA1MuxByPin(uint8_t pin, bool takeover_only_ports_ok = false) {
  uint8_t port = digitalPinToPort(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  #if defined(DB_64_PINS)
  // AVR128DB, AVR64DB work with the high MUX options
  if (((port == 1 || port == 6) && (bit_mask & 0x3F)) || (bit_mask & 0x70)) {
  #else  // AVR128DA64 definitely do not work. AVR64DA64 untested.
  if (((port == 1) && (bit_mask & 0x3F)) || (bit_mask & 0x70)) {
  #endif // And those are the only 4 parts in the product line for which those pins exist. *INDENT-ON*
    // PORTB and PORTG have full-service TCA1 (well, not PG on the 128DA63 up to at least the A8 die
    // rev). for those, bit_mask will be 0x01, 0x02, 0x04, 0x08, 0x10, or 0x20 - but not 0x40 or
    // 0x80. Hence test against 0x3F works. For the others, it is either 0x10, 0x20, or 0x40 so
    // test against 0x70; the port function will check that the non-B/G port is valid.
    return setTCA1MuxByPort(digitalPinToPort(pin), takeover_only_ports_ok);

  }
  return false;
}
#endif

bool setTCD0MuxByPort(uint8_t port, bool takeover_only_ports_ok = false) {
  /*
    if ((!(port < 2 || port > 4)) || (port >6))
      return false;
    if (port > 4)
      port -= 3; //0, 1 left as is, 2, 3, 4 got return'ed out of. 5, 6 get turned into 2 and 3.
    PORTMUX.TCDROUTEA = port;
    return true;
  */
  if (port == 0 || takeover_only_ports_ok) { // See errata; appears to be broken on all parts, not just 128k ones. Return false unless the one working port was requested.
    PORTMUX.TCDROUTEA = 0;
    return true;
  }
  return false;

}

bool setTCD0MuxByPin(uint8_t pin, bool takeover_only_ports_ok = false) {
  if (digitalPinToBitPosition(pin) & 0xF0) {
    return setTCD0MuxByPort(digitalPinToPort(pin), takeover_only_ports_ok); // See errata; appears to be broken on all parts, not just 128k ones. So, if it's not pin 4-7,
  }
  return false; // it's definitely no good. If it is 4-7, pass the other function to check port (though we could optimize further here, since
}               // chips that one might want to call this for don't exist, let's not bother :-)

#define MVIO_DISABLED (-128)
#define MVIO_BAD_FUSE (-64)
#define MVIO_UNDERVOLTAGE (1)
#define MVIO_OKAY (0)
#define MVIO_UNSUPPORTED (-1)
#ifdef MVIO
  // File this under "abuse of the ternary operator"
  #define getMVIOStatus() ((FUSE.SYSCFG1 & 0x18) == 0x10 ? MVIO_DISABLED : ((FUSE.SYSCFG1 & 0x18) == 0x08 ? (MVIO.STATUS ? MVIO_OKAY : MVIO_UNDERVOLTAGE) : MVIO_BAD_FUSE))

#else
  #define getMVIOStatus() MVIO_UNSUPPORTED
#endif
