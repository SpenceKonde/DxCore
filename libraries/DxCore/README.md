# DxCore Library
This library provides wrappers around a few chip features that didn't seem appropriate to put into the API at large, but which also did not seem large enough for a library. The examples may also include examples of using chip functionality with no library or wrapper at all; see the specific examples for more information. In all of these functions, a "successful" return value is 0, while potential failures are non-zero values. While this may initially seem backwards, it means that one can use the value returned directly in an `if` statement to check for success/failure, rather than comparing to a "success" value, yet the returned value also contains the full reason for the failure if that is needed. This is the same convention used by program "exit code" or "exit status" - for the same reason.

## GetMVIOStatus
A new macro is provided, `getMVIOStatus()`, which returns a value depending on whether MVIO is supported, enabled, and if so, whether VDDIO2 voltage is high enough that MVIO is enabled.

This returns one of the following constants:
```
MVIO_DISABLED       -128
MVIO_BAD_FUSE       -64
MVIO_UNDERVOLTAGE    1
MVIO_OKAY            0
MVIO_UNSUPPORTED    -1
```

These numeric values mean that `if(getMVIOStatus()){...}` will run the conditional statements if MVIO is not enabled and working. Similarly, `if(getMVIOStatus()>=0){...}` will run the conditional if MVIO is supported and enabled, whether or not there is an appropriate VDDIO2 voltage supplied.

```c
void checkMVIO() {
  int status=getMVIOStatus();
  switch(status) {
    case MVIO_DISABLED:
      Serial.println("MVIO supported on chip but disabled in fuses.");
      break;
    case MVIO_BAD_FUSE:
      Serial.println("MVIO supported on chip but relevant fuses in invalid state.");
      break;
    case MVIO_UNDERVOLTAGE:
      Serial.println("MVIO enabled, VDDIO2 voltage is too low to support MVIO.");
      break;
    case MVIO_UNSUPPORTED:
      Serial.println("MVIO is not supported by this part.");
      break;
    case MVIO_OKAY:
      Serial.println("MVIO is enabled and working!");
      break;
  }
}

```

## Autotune
Autotune allows use of an external 32kHz crystal to automatically tune the internal high frequency oscillator. This is only accurate to within ~ 0.4% (the size of the tuning step on the internal oscillator). A very simple bit of code using this library is shown below as well.


```c
typedef enum X32K_TYPE
{
    X32K_LOWPWR_START31MS = (CLKCTRL_CSUT_1K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START500MS = (CLKCTRL_CSUT_16K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START1S = (CLKCTRL_CSUT_32K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START2S = (CLKCTRL_CSUT_64K_gc|CLKCTRL_LPMODE_bm),
    X32K_HIGHPWR_START31MS = (CLKCTRL_CSUT_1K_gc),
    X32K_HIGHPWR_START500MS = (CLKCTRL_CSUT_16K_gc),
    X32K_HIGHPWR_START1S = (CLKCTRL_CSUT_32K_gc),
    X32K_HIGHPWR_START2S = (CLKCTRL_CSUT_64K_gc),
    X32K_EXTCLK = (CLKCTRL_SEL_bm)
} X32K_TYPE_t;

typedef enum X32K_ENABLE
{
    X32K_DISABLED = ( 0x00),
    X32K_ENABLED  = ( 0x01),
    X32K_ALWAYSON = ( 0x81),
} X32K_ENABLE_t;
```

`void configXOSC32K(X32K_TYPE_t, X32K_ENABLE_t)`
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that this likely matters!)
// since CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

`void disableXOSC32K()`
disables the external 32.768 kHz oscillator

`uint8_t enableAutoTune() `
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 1 second startu time and normal (not low power) crystal.
// Returns 1 if autotune was not successfully enabled - waited for CSUT + 0.5 seconds, and status still reported XOSC32K as not running/stable!
// Returns 255 (-1) if the current main clock source isn't the internal HF oscillator (presumably it's either external clock/crystal)
// under this circumstance, autotune will not impact the main clock - and the main clock, in fact, is likely more accurate than autotune would achieve.
// Returns 0 if autotune is successfully enabled within the time permitted by XOSC32KCTRLA's currently configured CSUT bits.

`uint8_t disableAutoTune()`
// Returns 255 (-1) if autotune was not enabled.
// Returns 0 if autotune is successfully disabled.


```c
// example

void doStuffWithOSC() {
  configXOSC32K();
  Serial.print("Current external 32K osc: ");
  Serial.printHexln(CLKCTRL.XOSC32KCTRLA);
  Serial.println("We can't tell if it's on until we try to use it unless using a Rev. A5 or later AVR128DB - so we won't mess with that mess!")
  if(enableAutoTune()){
    // if enableAutoTune() is non-zero, that's a failure... enabling external oscillator FAILED - unsurprising, since hardly anyone uses a 32 kHz CLOCK, which is what we tried...
    Serial.println("failed to enable autotune w/external 32kHz osc set to CLOCK");
    if (disableAutoTune()) {
      // true = fail
      Serial.println("Cant disable autotune - it's already off!");
    }
    disableXOSC32K();
    Serial.println("Disabling the non-functional external oscillator");
  }
  // Who would have thought, trying to use a crystal that's not connected wouldn't work?
  // Let's try something that will, if a crystal is present: enable external crystal, with the safest settings
  configXOSC32K(X32K_HIGHPWR_START2S,X32K_ENABLED);

  if(enableAutoTune()){
    //if this is true... enabling external oscillator FAILED - with safe settings this time
    Serial.println("Also failed safe settings too - settings were: ");
    Serial.printHexln(CLKCTRL.XOSC32KCTRLA);
    disableXOSC32K();
    Serial.println("Disabling the external oscillator - we give up!");

  } else {
    if (disableAutoTune()) {
      Serial.println("Cant disable autotune - strange, it should be on! ");
    } else {
      Serial.println("Disabled autotube... ")
    }
  }
  Serial.println("Okay, we're done now! ");
  disableXOSC32K();
  Serial.println("Disabling the external oscillator - we give up!");

  //Done!
}


```
