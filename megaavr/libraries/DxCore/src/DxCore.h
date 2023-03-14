#ifndef DXCORE_H
#define DXCORE_H
#include <Arduino.h>

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


#ifndef __cplusplus
void configXOSC32K(X32K_OPT_t settings, X32K_ENABLE_t enable);
#endif
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that you may care. )

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
// Any other value should be a can't-happen

/*****************************
 MVIO
*****************************/


uint16_t getMVIOVoltage();
// Return value is the MVIO voltage in millivolts (if > 1000) or is one of the below error codes.
// If it is lower, this is instead one or more of the below error codes
// (notice that each error code has only one high bit set. Multiple of these are often OR'ed together)

uint8_t getMVIOStatus();
// Returns either MVIO_OKAY (0) or an error code, depending on the state of MVIO

#ifndef __cplusplus
uint8_t initOPAMP_MVIO(uint16_t voltage, uint8_t opamp, uint8_t options, int16_t dacref);
#endif

// These are returned by either initOPAMP_MVIO or getMVIOStatus() and getMVIOVoltage().
#define MVIO_OKAY             (0x00) // If no other bits set, also configured correctly.
#define MVIO_UNDERVOLTAGE     (0x01) // If no other bits set, configured correctly, but insufficient voltage.
#define MVIO_BAD_FUSE         (0x20) // MVIO fuse bits are in an invalid state
#define MVIO_DISABLED         (0x40) // MVIO is disabled. If no other bits set and this was returned by getMVIOStatus(), all is configured correctly.
//                                      Otherwise, we cannot meaningfully fulfil the request as a result.
#define MVIO_UNSUPPORTED      (0x80) // Why call this function? MVIO doesn't exist on this part. In the case of initOPAMP_MVIO(), this can indicate that either MVIO or OPAMPs are missing.
//                                      Note that this will generate a compiler warning with more detail as well.
// These are only returned by getMVIOStatus() and getMVIOVoltage()
#define MVIO_SETTING_MISMATCH (0x04) // Setting of fuses doesn't match tools menu selection.
#define MVIO_IMPOSSIBLE_CFG   (0x08) // For some reason or another, this is not supposed to be possible with the Arduino IDE (or possibly at all)
#define MVIO_MENU_SET_WRONG   (0x10) // Not only is the menu in disagreement with the fuses, you picked the option that said you were really sure.
// Only returned by initOPAMP MVIO()
#define MVIO_VOLTAGE_TOO_HIGH (0x02) // Requested MVIO voltage exceeds the recommended maximum voltage of the chip.
#define MVIO_NO_VALID_REF     (0x03) // MVIO_UNDERVOLTAGE | MVIO_VOLTAGE_TOO_HIGH - MVIO_SELECT_REF option used, but there is no internal voltage reference that can provide the requested voltage
#define MVIO_REF_TOO_LOW      (0x04) // Requested MVIO voltage is higher than the current reference and MVIO_SELECT_REF not used so we can't change it.

// Valid MVIO options to pass to initOPAMP_MVIO()'s third argument
#define MVIO_SELECT_REF       (0x01) // Determine the lowest internal reference voltage that is high enough, and set the DAC reference to that.


/*****************************
Timer pin mux
*****************************/
/* The ByPort functions expect a number between 0 and 6, which is the number returned when digitalPinToPort() is called on a pin within that port or,
 * equivalently, one of the constants PA, PB, PC, PD, PE, PF, or PG.
 * PA = 0, PG = 6.
 * The ByPin functions expect a valid pin number.
 * Either way, these will return false if the pin or port is not valid or is not (part of) a valid mux option for the specified timer.
 * Otherwise, these will turn off all timer PWM outputs and set the portmux to the correct mux option to output on the requested pin or port.
 * Note that this does mean that we will turn off the PWM currently configured, even if the current mux option is not changed.
 * For example:
  setTCA0MuxByPort(PA); // Set TCA portmux to PA (0).
  analogWrite(PIN_PA2,128); // This enables PWM on PA2
  setTCA0MuxByPort(PA); // But this immediately turns it off.
 * Few applications will require setting the portmux for a given timer more than once.
 */

#if defined(TCA0)
bool setTCA0MuxByPort(uint8_t port);
bool setTCA0MuxByPin(uint8_t pin);
#endif
#if defined(TCA1)
bool setTCA1MuxByPort(uint8_t port);
bool setTCA1MuxByPin(uint8_t pin);
#endif // TCA1
#if defined(TCD0)
bool setTCD0MuxByPort(uint8_t port);
bool setTCD0MuxByPin(uint8_t pin);
#endif
int8_t timerToDigitalPin(uint8_t timer, uint8_t channel);

/*****************************
resets
*****************************/

inline void SoftwareReset() {
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
}
// Reset immediately. The bootloader, if present, will run.

inline void ResetWithWDT() {
  _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}
// Reset after a very short delay. The bootloader, if present, will not run.

// C++ prototypes. C-definitions are grouped above by function.
#ifdef __cplusplus
}
uint8_t initOPAMP_MVIO(uint16_t voltage, uint8_t opamp = 0, uint8_t options = 0, int16_t dacref = -1);
void configXOSC32K(X32K_OPT_t = X32K_HIGHPWR_START2S, X32K_ENABLE_t = X32K_ENABLED);
// in ADCErrors.cpp
// ADC error interpretation helper functions
int8_t analogCheckError(int16_t val);
int8_t analogCheckError(int32_t val);
bool printADCRuntimeError(int32_t error, HardwareSerial &__dbgser = Serial);
bool printADCRuntimeError(int16_t error, HardwareSerial &__dbgser = Serial);
#endif


#endif
