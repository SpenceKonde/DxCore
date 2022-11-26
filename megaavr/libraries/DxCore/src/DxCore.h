#ifndef DXCORE_H
#define DXCORE_H
#include <Arduino.h>

// in ADCErrors.cpp
// ADC error interpretation helper functions
#ifdef __cplusplus
  int8_t analogCheckError(int16_t val);
  int8_t analogCheckError(int32_t val);
  bool printADCRuntimeError(int32_t error, HardwareSerial &__dbgser = Serial);
  bool printADCRuntimeError(int16_t error, HardwareSerial &__dbgser = Serial);
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum X32K_OPT {
  X32K_LOWPWR_START31MS   = (CLKCTRL_CSUT_1K_gc  | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START500MS  = (CLKCTRL_CSUT_16K_gc | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START1S     = (CLKCTRL_CSUT_32K_gc | CLKCTRL_LPMODE_bm),
  X32K_LOWPWR_START2S     = (CLKCTRL_CSUT_64K_gc | CLKCTRL_LPMODE_bm),
  X32K_HIGHPWR_START31MS  = (CLKCTRL_CSUT_1K_gc),
  X32K_HIGHPWR_START500MS = (CLKCTRL_CSUT_16K_gc),
  X32K_HIGHPWR_START1S    = (CLKCTRL_CSUT_32K_gc),
  X32K_HIGHPWR_START2S    = (CLKCTRL_CSUT_64K_gc),
  X32K_EXTCLK             = (CLKCTRL_SEL_bm)
} X32K_OPT_t;

typedef enum X32K_ENABLE {
  X32K_DISABLED = (0x00),
  X32K_ENABLED  = (0x01),
  X32K_ALWAYSON = (0x81)
} X32K_ENABLE_t;

// void configXOSC32K(X32K_TYPE_t, X32K_ENABLE_t)
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that this likely matters!)

// void disableXOSC32K()
// disables the external 32.768 kHz oscillator
#define disableXOSC32K() _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, (CLKCTRL.XOSC32KCTRLA & 0xFE))


// since CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

uint8_t enableAutoTune();
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 2 second startup time and normal (not low power) crystal.
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
void configXOSC32K(X32K_TYPE_t settings = X32K_HIGHPWR_START2S, X32K_ENABLE_t enable = X32K_ENABLED);
#else
void configXOSC32K(X32K_TYPE_t settings, X32K_ENABLE_t enable);
#endif

// passed a port number (eg, digitalPinToPort() from pin, or number 0-6)
// or pin number, respectively, set the set the specified mux to the mapping that includes that pin after making sure nothing on the pins it's getting moved to is currenttly outputting it's own PWM.
bool setTCA0MuxByPort(uint8_t port);
bool setTCA0MuxByPin(uint8_t pin);

#define MVIO_OKAY             (0x00) // If no other bits set, also configured correctly.
#define MVIO_UNDERVOLTAGE     (0x01) // If no other bits set, configured correctly.
#define MVIO_SETTING_MISMATCH (0x04) // Setting of fuses doesn't match tools menu selection.
#define MVIO_IMPOSSIBLE_CFG   (0x08) // For some reason or another, this is not supposed to be possible with the Arduino IDE (or possibly at all)
#define MVIO_MENU_SET_WRONG   (0x10) // Not only is the menu in disagreement with the fuses, you picked the option that said you were really sure.
#define MVIO_BAD_FUSE         (0x20) // MVIO fuse bits are in an invalid state
#define MVIO_DISABLED         (0x40) // MVIO is disabled. If no other bits set, all is configured correctly.
#define MVIO_UNSUPPORTED      (0x80) // Why call this function? MVIO doesn't exist on this part.

#ifdef __cplusplus
  #ifdef TCA1
    bool setTCA1MuxByPort(uint8_t port, bool __attribute__((unused)) takeover_only_ports_ok = false);
    bool setTCA1MuxByPin(uint8_t pin, bool __attribute__((unused)) takeover_only_ports_ok = false);
  #endif // TCA1
  bool setTCD0MuxByPort(uint8_t port, bool __attribute__((unused)) takeover_only_ports_ok = false);
  bool setTCD0MuxByPin(uint8_t pin, bool __attribute__((unused)) takeover_only_ports_ok = false);

  uint8_t getMVIOStatus(bool printInfo = 0, HardwareSerial &dbgserial = Serial);
#endif
// Reset immdiately using software reset. The bootloader, if present will run.

inline void SoftwareReset() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
// Reset after a very short delay. The bootloader, if present, will not run.
inline void ResetWithWDT() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

#endif
