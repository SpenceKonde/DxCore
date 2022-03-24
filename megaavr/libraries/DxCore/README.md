# DxCore Library
This library provides wrappers around a few chip features that didn't seem appropriate to put into the API at large, but which also did not seem large enough for a library. The examples may also include examples of using chip functionality with no library or wrapper at all; see the specific examples for more information. In all of these functions, a "successful" return value is 0, while potential failures are non-zero values. While this may initially seem backwards, it means that one can use the value returned directly in an `if` statement to check for success/failure, rather than comparing to a "success" value, yet the returned value also contains the full reason for the failure if that is needed. This is the same convention used by program "exit code" or "exit status" - for the same reason.

## PWM stuff
The Type A timer(s) and Type D timer on the device gets a pair of functions:
```c++
bool setTCA0MuxByPort(uint8_t port);
bool setTCA0MuxByPin(uint8_t pin);

// Parts with a TCA1 only (48 and 64-pin)
bool setTCA1MuxByPort(uint8_t port, bool takeover_only_ports_ok);
bool setTCA1MuxByPin(uint8_t pin, bool takeover_only_ports_ok);

bool setTCD0MuxByPort(uint8_t port, bool takeover_only_ports_ok);
bool setTCD0MuxByPin(uint8_t pin, bool takeover_only_ports_ok);

```
The first argument passed to the `setTCxnMuxByPort/Pin` functions is, respectively the port number (0, 1, 2, 3, 4, 5, or 6 for ports A-G), or a pin number. The port number is the same value returned by digitalPinToPort() for a pin number; if the port is valid and supports output from that timer, the appropriate PORTMUX register is set to do so. For `setTCxnMuxByPin()` it verifies that the pin is is one which can output PWM from the specified timer. Note that as of 2/5/2021, there does not appear to be any silicon available in which any option other than the first (`PORTA`) works for TCD0 PORTMUX. Only that option is considered valid for that timer until such a time as working silicon becomes available and this code can test for it; it is included to offer guidance as to the planned interface.

The second, optional `takeover_only_ports_ok` argument, is available only when there are options which cannot be used through the builtin API functions (mostly analogWrite() - it only supports PWM output from TCA's in split mode on a mapping option with all 6 outputs pointed to a pin, not the two options for TCA1 with only three pins available). Unless `true` is passed as this argument, these functions will not select one of those options.

Returns `true` if an argument referring to a valid port was passed and a value was assigned to `PORTMUX.TCAROUTEA` (whether or not that was different from the value it previously had). `false` if an invalid port/pin, or one which cannot output PWM on the specified pin was specified. If the PORTMUX register is set, it will also turn off PWM output on all of the pins covered by the newly selected mapping before connecting it.



## Multi-Voltage IO
The AVR DB-series supports a new feature, Multi-Voltage IO (MVIO). When enabled by the appropriate fuse settings, the I/O voltage used for PORTC will be the voltage applied to the VDDIO2 pin (actually in my tests, this was always the case - regardless of whether it was enabled by the fuse - the fuse didn't internally connect them or anything, and there was no increased current that would suggest damage was occurring. However, with the fuse set to single supply mode, PORTC would behave erratically when VDDIO2 was too low, insead of handling that situation gracefully). A new function is provided, `getMVIOStatus()`, which returns a value depending on whether MVIO is supported, whether it is enabled, and if so, whether VDDIO2 voltage is high enough that the MVIO pins (PORTC) are available; MVIO requires VDDIO2 to be at least 1.65V

This was expanded significantly in 1.4.8, because on the AVR-DD series, enabling MVIO disables some pin functionality, and a mismatch could result in analogRead() returning bogus values or the Comparator library giving incorrect results.

This returns one of the following constants:
```c++
MVIO_DISABLED        0x40 // 64
MVIO_BAD_FUSE        0x20 // 32
MVIO_UNDERVOLTAGE    0x01 // 1
MVIO_OKAY            0x00 // 0
MVIO_UNSUPPORTED     0x80 // 128
MVIO_MENU_SET_WRONG  0x10 // 16
MVIO_IMPOSSIBLE_CFG  0x08 // 8
```


By default (if no argument is passed to getMVIOStatus() it will not use "debugging mode", and will not print any output, even when severely broken conditions are found. Passing any non-zero value will enable debugging mode; this is written with the assumption that a human is reading the output of Serial, and, in the event of serious misconfigurations, will print out warning text. If it is being used with Serial connected to something else, and/or nobody is watching that serial port, no argument should be passed. As long as it its always called without non-zero argument, there will be no output to serial and the optimizer will be able to remove those print lines. In any event, even debugging mode will only generate any output in a small number of severely broken situations where core behavior in other areas will be impacted:
1. If you are using Optiboot, and have set the MVIO tools menu to the "Enabled, and "burn bootloader" has for sure done w/this selected." or "Disabled, and "burn bootloader" has for sure done w/this selected.", but the fuse is set to the opposite of what you specified. This causes breakage, because choosing that option tells the core to assume that it is configured that way (instead of checking the fuse - if you choose simply "Enabled" or "Disabled", when we need to know if MVIO is enabled, we check the fuse; it is not unexpected that an Optiboot configuration might not match the menu options, because they are only set on bootload (the bootloader cannot write it's own fuses). This saves save flash and improve performance - but will cause breakage if the assumption you told it to make is not valid.
2. If you are not using Optiboot (and hence are uploading through UPDI), the SYSCFG1 fuse is set upon every upload. Thus, it should not be possible to end up with code on the chip that was compiled for MVIO being enabled when it wasn't or vice versa. In the event that this is detected, it will print a message and return `MVIO_MENU_SET_WRONG | MVIO_IMPOSSIBLE_CFG`  (numeric value 0x18).
3. If under any circumstances, the MVIO bits are set to a value that does not correspond to a valid MVIO setting, `MVIO_IMPOSSIBLE_CFG` will be returned and an warning message will be printed.

Except for situation 1, which is most likely user error, these situations can arise only when using a third party IDE that not configured to set the fuses, or is configured to set them incorrectly, or if that IDE does not pass the required define (`MVIO_ENABLED`) along to the compiler to tell it when MVIO is enabled. If cases 2 or 3 ever occur with the Arduino IDE, that is a serious bug that should be reported promptly along with any information on how you were able to make it happen.

Multiple errors can be combined if they apply. `MVIO_MENU_SET_WRONG` is returned alone in case 1 above, while in case 2, it is returned along with `MVIO_IMPOSIBLE_CFG` to indicate that it is not supposed to be possible to get the chip into this confiuration. It is also returned in weird situations that they datasheet says are impossible, for example if the system is set to single supply mode, but `MVIO.STATUS != 1` or `MVIO.INTFLAGS != 0`, directly contrary to the datasheet (MVIO chapter, functional description, under Initialization).

These numeric values mean that `if(getMVIOStatus()) {...}` will run the conditional statements if MVIO is **not** enabled and working. Similarly, `if(getMVIOStatus()==0) {...}` will run the conditional if MVIO **is** supported and enabled, whether or not there is an appropriate VDDIO2 voltage supplied. The below example shows how one might print human readable description of the MVIO status:

```c++
void checkMVIO() {
  uint8_t status=getMVIOStatus(1);
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
typedef enum X32K_TYPE {
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

typedef enum X32K_ENABLE {
    X32K_DISABLED = (0x00),
    X32K_ENABLED  = (0x01),
    X32K_ALWAYSON = (0x81),
} X32K_ENABLE_t;
```

```c++
void configXOSC32K(X32K_TYPE_t, X32K_ENABLE_t)
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that this likely matters!)
// since CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

void disableXOSC32K()
// disables the external 32.768 kHz oscillator

uint8_t enableAutoTune()
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 1 second startu time and normal (not low power) crystal.
// Returns 1 if autotune was not successfully enabled - waited for CSUT + 0.5 seconds, and status still reported XOSC32K as not running/stable!
// Returns 255 (-1) if the current main clock source isn't the internal HF oscillator (presumably it's either external clock/crystal)
// under this circumstance, autotune will not impact the main clock - and the main clock, in fact, is likely more accurate than autotune would achieve.
// Returns 0 if autotune is successfully enabled within the time permitted by XOSC32KCTRLA's currently configured CSUT bits.

uint8_t disableAutoTune()
// Returns 255 (-1) if autotune was not enabled.
// Returns 0 if autotune is successfully disabled.
```

```c
// example

void doStuffWithOSC() {
  configXOSC32K();
  Serial.print("Current external 32K osc: ");
  Serial.printHexln(CLKCTRL.XOSC32KCTRLA);
  Serial.println("We can't tell if it's on until we try to use it unless using a Rev. A5 or later AVR128DB - so we won't mess with that mess!")
  if(enableAutoTune()) {
    // if enableAutoTune() is non-zero, that's a failure... enabling external oscillator FAILED - unsurprising, since hardly anyone uses a 32 kHz CLOCK, which is what we tried...
    Serial.println("failed to enable autotune w/external 32kHz osc set to CLOCK");
    if (disableAutoTune()) {
      // true = fail
      Serial.println("Can't disable autotune - it's already off!");
    }
    disableXOSC32K();
    Serial.println("Disabling the non-functional external oscillator");
  }
  // Who would have thought, trying to use a crystal that's not connected wouldn't work?
  // Let's try something that will, if a crystal is present: enable external crystal, with the safest settings
  configXOSC32K(X32K_HIGHPWR_START2S,X32K_ENABLED);

  if(enableAutoTune()) {
    // if this is true... enabling external oscillator FAILED - with safe settings this time
    Serial.println("Also failed safe settings too - settings were: ");
    Serial.printHexln(CLKCTRL.XOSC32KCTRLA);
    disableXOSC32K();
    Serial.println("Disabling the external oscillator - we give up!");

  } else {
    if (disableAutoTune()) {
      Serial.println("Can't disable autotune - strange, it should be on! ");
    } else {
      Serial.println("Disabled autotube... ")
    }
  }
  Serial.println("Okay, we're done now! ");
  disableXOSC32K();
  Serial.println("Disabling the external oscillator - we give up!");

  // Done!
}
```
