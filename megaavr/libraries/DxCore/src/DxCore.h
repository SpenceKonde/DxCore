#ifndef DXCORE_H
#define DXCORE_H
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

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
void configXOSC32K(X32K_TYPE_t settings, X32K_ENABLE_t enable);

// void disableXOSC32K()
// disables the external 32.768 kHz oscillator
#define disableXOSC32K() _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, (CLKCTRL.XOSC32KCTRLA & 0xFE))


// since CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

uint8_t enableAutoTune();
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 1 second startu time and normal (not low power) crystal.
// Returns 1 if autotune was not successfully enabled - waited for CSUT + 0.5 seconds, and status still reported XOSC32K as not running/stable!
// Returns 255 (-1) if the current main clock source isn't the internal HF oscillator (presumably it's either external clock/crystal)
// under this circumstance, autotune will not impact the main clock - and the main clock, in fact, is likely more accurate than autotune would achieve.
// Returns 0 if autotune is successfully enabled within the time permitted by XOSC32KCTRLA's currently configured CSUT bits.

int8_t disableAutoTune();
// Returns 255 (-1) if autotune was not enabled.
// Returns 0 if autotune is successfully disabled.




int8_t getMVIOVoltage();
// returns ADC or MVIO error code, or voltage in millivolts;


#ifdef __cplusplus
}
#endif

// passed a port number (eg, digitalPinToPort() from pin, or number 0-6)
// or pin number, respectively, set the set the specified mux to the mapping that includes that pin after making sure nothing on the pins it's getting moved to is currenttly outputting it's own PWM.
bool setTCA0MuxByPort(uint8_t port);
bool setTCA0MuxByPin(uint8_t pin);

#define MVIO_DISABLED       (0x40)
#define MVIO_BAD_FUSE       (0x20)
#define MVIO_UNDERVOLTAGE   (0x01)
#define MVIO_OKAY           (0x00)
#define MVIO_UNSUPPORTED    (0x80)
#define MVIO_MENU_SET_WRONG (0x10)
#define MVIO_IMPOSSIBLE_CFG (0x80)

#ifdef __cplusplus
  #ifdef TCA1
    bool setTCA1MuxByPort(uint8_t port, bool takeover_only_ports_ok = false);
    bool setTCA1MuxByPin(uint8_t pin, bool takeover_only_ports_ok = false);
  #endif // TCA1
  bool setTCD0MuxByPort(uint8_t port, bool takeover_only_ports_ok = false);
  bool setTCD0MuxByPin(uint8_t pin, bool takeover_only_ports_ok = false);

  uint8_t getMVIOStatus(bool printInfo = 1);
#endif

#endif
