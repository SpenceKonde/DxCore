# DxCore Library
This library provides wrappers around a few chip features that didn't seem appropriate to put into the API at large, but which also did not seem large enough for a library. The examples may also include examples of using chip functionality with no library or wrapper at all; see the specific examples for more information. In all of these functions, a "successful" return value is 0, while potential failures are non-zero values. While this may initially seem backwards, it means that one can use the value returned directly in an `if` statement to check for success/failure, rather than comparing to a "success" value, yet the returned value also contains the full reason for the failure if that is needed. This is the same convention used by program "exit code" or "exit status" - for the same reason.

## PWM stuff
The Type A timer(s) and Type D timer on the device gets a pair of functions:
```c++
bool setTCA0MuxByPort(uint8_t port);
bool setTCA0MuxByPin(uint8_t pin);

// Parts with a TCA1 only (48 and 64-pin)
bool setTCA1MuxByPort(uint8_t port);
bool setTCA1MuxByPin(uint8_t pin);

bool setTCD0MuxByPort(uint8_t port);
bool setTCD0MuxByPin(uint8_t pin);

```
The first argument passed to the `setTCxnMuxByPort/Pin` functions is, respectively the port number (0, 1, 2, 3, 4, 5, or 6 for ports A-G), or a pin number. The port number is the same value returned by digitalPinToPort() for a pin number; if the port is valid and supports output from that timer, the appropriate PORTMUX register is set to do so. For `setTCxnMuxByPin()` it verifies that the pin is is one which can output PWM from the specified timer. Note that as of late 2022, only DD-series parts can use ports other than PORTA for TCD0. This will return `false` for ports other than PORTA on DA/DB parts.

Returns `true` if an argument referring to a valid port was passed and a value was assigned to `PORTMUX.TCAROUTEA` (whether or not that was different from the value it previously had). `false` if an invalid port/pin, or one which cannot output PWM from that timer was specified. If the PORTMUX register is set, it will also turn off PWM output on all of the pins covered by the newly selected mapping before connecting it.

## Multi-Voltage IO

The AVR DB-series and DD-series supports a new feature, Multi-Voltage IO (MVIO). When enabled by the appropriate fuse settings, the I/O voltage used for PORTC will be the voltage applied to the VDDIO2 pin (actually in my tests, this was always the case - regardless of whether it was enabled by the fuse - the fuse didn't internally connect them or anything, and there was no increased current that would suggest damage was occurring. However, with the fuse set to single supply mode, PORTC would behave erratically when VDDIO2 was too low, instead of handling that situation gracefully). A new function is provided, `getMVIOStatus()`, which returns a value depending on whether MVIO is supported, whether it is enabled, and if so, whether VDDIO2 voltage is high enough that the MVIO pins (PORTC) are available; MVIO requires VDDIO2 to be at least 1.65V - **Warning: Read that again. *IF MVIO IS TURNED OFF IN FUSES, VDDIO2 MUST BE CONNECTED TO VDD*.** Otherwise, you are entering the realm of **UNDEFINED BEHAVIOR** - quick tests showed nothing surprising happening as long as the VDDIO2 voltage was in range, but this is **NOT** recommended as a way to save power, as there is no guarantee that it does not risk damaging the chip. **When VDDIO2 was below the minimum voltage range, unconnected to VDD, and MVIO was disabled, the behavior of the port is not only undefined, but known to be in bizarre and very wrong** - think random results for reads, and likely output as well; **We cannot exclude the possibility that damage to the chip could be caused opperating in this incorrectly configured regime**; the datasheet uses the keyword "must" to describe the the need to connect VDD and VDDIO when MVIO is not enabled, not "should". That's a much more forceful word.

So don't do that. If MVIO is disabled, FFS, connect VDDIO2 to VDD like the datasheet says you must.
If MVIO is intended to be enabled, MAKE SURE THAT IT IS.

(unfortunately, there *isn't* a way to measure the voltage being supplied to the VDDIO2 pin if MVIO is disabled in fuses. VDDIO2DIV10 just reads as 0 - why is this unfotunate? Because, particularly with TQFP packages, it is possible for a cold solder joint that looks good to be invisible to visual inspection, and when you probe it with a multimeter, that will press down the pin and bring it into contact, and you will think it's connected, and run around in circles. Those sorts of bad connections are hard enough to detect on a normal I/O pin. But on a power pin that you cannot measure in any way, it is all but undetectable, unless you're specifically looking for it by trying to power an LED from PORTC, and then noticing that pressing down on the pin with the multimeter probe makes the LED turn on)

```c
/* If using MVIO */
void setup() {
  if (getMVIOStatus() & MVIO_DISABLED) {
    Serial.println("ERROR: MVIO is disabled. STOP!"); // You may replace this with your preferred method of indicating this error condition.
    while(1)
  }
  /// rest of setup goes here.
}
```

### Design considerations
* Use a single supply if you can. I'm not dissing MVIO here at all, you'll just have a simpler system that's more likely to work if you only have to worry about one supply rail.
* Keep in mind that you may have as few as THREE PINS that operate at the MVIO voltage, and at most 8. Note that these always contain a serial port, an SPI port, and an I2C port. On parts with 28 or 32 pins, both serial and I2C can be used at the same time, and on parts with more pins, all three can. An example of wise pin layount that is to be commended, especially considering that the decisions were cast in silicon back in 2016 (for the larger parts - the parts with only 3 pins had to be give an extra serial mux option on USART0 to use the port at all since they otherwise lost their TX pin - but like, with only 14 pins, 3 of them used by power, and 2 more by reset and updi... something had to give (up pins). At 20 pins they'd already amputated all the non-funky pins of PF, needed 4 more - so off with the low half of PD (they needed the high half cause that's where the DAC goes. Based on the precedent of ZCD3, they'd have had to call it DAC1 if they wanted the sole dac to come out any other pin), and all the pins of portA that aren't needed for the crystal were already on the chopping block for the 14-pin version, They didn't have much choice. Either something was gonna come off of PORTC, or the part would have had to grow a 21st pin. And so PC0 got the axe and USART0 took another glass of the PORTMUX-punch `*`, as did SPI0 to get, respectively a C1, C2, and a C1, C2, C3, F6 (reset) option.
* You may find that you don't have enough MVIO pins. AVR provides many solutions here. You only *need* to use MVIO or a level shifter if you need to drive a pin connected to a lower voltage part to that part's HIGH. Otherwise:
  * If you only need to read the pin, on a DD, DB, or EA, set the INLVL voltage to TTL (using either pinConfigure per the digital reference, or manually writing the PORTx.PINnCTRL register bit),
  * If you need to read the pin and the other part is at a higher voltage, you can usually use a resistor divider.
    * This does no favors for power consumption if a pin is normally HIGH though, but in that case, you can turn on the internal pullup and put a small signal diode with the band towards the high voltage device, so the pin will be pulled up to this device's VDD unless the external pin is driven low.
  * If the other part runs at a lower voltage, and you only need to drive the pin LOW - that is, a pullup to it's lower Vdd will suffice or it has an internal pullup - setting the pin LOW, and then setting pinMode to either INPUT or OUTPUT (so it switches between driving low, or high impedance), so it acts like an open drain output. see the openDrain() and openDrainFast() API extensions in the digital reference for a friendlier way to do this.
    * Even serial can do this on the TX line: Use ODME, and pull the lines up with external pullups to the power rail of the device you're talking to.
  * For I2C, on some parts (see the datasheet and/or wire documentation) there is an SMBus voltage option. This the I2C analog of the above mentioned INLVL option, and will make the thresholds vastly lower. Be sure in this case that you do not call Wire.usePullups() and that the pullups are connected to a voltage all devices find acceptable. Measure the voltage on those pins. Also, while a rather unlikely case, if the SMBus voltages are much lower than the device's voltages - and it's threshold for a high is higher, you may need to lower the baud rate or use stronger pullups, because the TWI will start counting the time of the HIGH portion of the clock when it hits the SMBus HIGH threshold (see the datasheet or Wire documentation).
* As you can see from above, the things that REALLY NEED the MVIO functionality are SPI and bidirecional UART, in addition to push-pull GPIO.

It is likely that the most common use cases will see the AVR running with VDD = 5V and VDDIO2 at a lower voltage like 3.3, 3.0, 2.5, 2.1. 2.0, or 1.8V (these are common voltages used in electronics) and AVRs are unusual among modern microcontrollers in that they can be run at 5V. Another common application would be a battery powered system running from a single cell LiPo battery (because you don't want to run at 3.3v from an LDO because the LDO will hog power, which is used directly to power the chip with no regulator (to lose the quiescent current of the regulator) which must be able to talk to devices standard "electronics voltages", though if the voltage is higher than Vdd. You still need to get the voltage to the VDDIO2 pin, though - maybe it has a pin that supplies it's Vdd but you still need to be able to operate it when it's not plugged in, or maybe you know what that voltage is, and can use the below-described OPAMP method to generate the IO voltage. For a concrete example of what sort of thing might do this, imagine something that you carry around and can set up via buttons and a screen, sleeps when not in use, and is made for the purpose of being plugged into other devices that run from mains power, like a standalone programmer to update firmware.

If running on batteries and you DON'T need MVIO, be sure to turn it off in the fuses, as it burns a small amount of power running what amounts to a less rigorous BOD mechanism (I say less rigorous because it's current is markedly lower; it's likely sampling less often).

### Running MVIO from the internal opamp
This is a feature of the DB's that Microchip marketing loves to talk about. And it is pretty F'ing cool, I'm not arguing with that. But it's only 20mA which is means there are a limited number of practical uses for it. Probably the most likely in Arduinoland is those damned fancypants new sensors developed for cell phones and the like, with unreal specs, but obnoxiously low maximum voltages (I've seen 2.5, 2.7 and 2.8, and I think once 2.1) - since their current consumption is often minuscule (that's one of their selling points) and they don't run at a high enough voltage for many hobbist applications, and sometimes don't even work with cheap level shifters (the classic bidirectional open drain level shifter often used for I2C will have a problem with that if they cheaped out on the FETs - while that would seem to be a shockling level of shoddiness, these are also 4-channel level shifters that can be had for under 50 cents each).

#### Limitations and Restrictions
* This requires an OPAMP peripheral. The DD-series doesn't have those.
* This uses the single DAC.
* This uses pins PD6 and PD2.
* The OPAMP is good to maybe 20mA, less near the edges of it's range.
  * This is a vast improvement over the DAC's sub-milliamp current capability.
* This uses an OPAMP, leaving 1 on 32 or 28 pin parts (hence precluding cascading two of them) and 2 on larger ones (hence precluding the instrumentation amplifier configuration described in the datasheet)
* VDD must be larger than the voltage you are trying to get out.

#### Required Connections
1. Run a jumper from the OPAMP output pin to VDDIO2.
2. Disconnect anything else from PD6, which is used for the DAC, and the DAC output buffer must be enabled to use it as an input to the OPAMP.

*note: Owners of an Azduino Nano DB or Nano DB+ do not need an external wire, as there are pads on the bottom that can be bridged with solder to select the OPAMP0 output. Refer to the documentation for more information*

#### Required code

```c
int16_t initMVIO_OPAMP(int16_t voltage, const int8_t opamp = 0,  const uint8_t options = 0, const int16_t dacref = -1)
```
voltage and dacref are in millivolts.

`voltage` is the volage that you want.

`dacref` is needed only when the DAC reference voltage is set to external. If omitted or -1, an internal reference is assumed. If DAC reference set to external or VDD, this is that reference voltage

`options` allows a small number of options to be passed. These can be OR'ed together if needed.
* `MVIO_SELECT_REF` - If this option is selected, `dacref` is ignored. The lowest reference voltage that is high enough to supply the requested voltage is configured. Remember that you need some headroom.

This can return any of the following:
* `MVIO_UNDERVOLTAGE` (0x01) - Voltage requested is under 1.65V, which is the minimum required by MVIO (the exact threshold varies between parts, and we can't determine that *a priori*.
* `MVIO_VOLTAGE_TOO_HIGH` (0x02) - if `voltage` or `dacref` exceeds 5.5V (5500), which is outside of recommended operating conditions.
* `MVIO_REF_TOO_LOW`  (0x04) - If the voltage requested is higher than the current DAC reference voltage. The core does NOT change the DAC reference voltage unless you tell it to. If the supplied dacref value for an external voltage is too low to ever generate a voltage high enough for MVIO that will return this and MVIO_UNDERVOLTAGE,
* `MVIO_NO_VALID_REF` (0x03) - If MVIO_SELECT_REF passed in options, but the requested voltage cannot be generated using an internal reference because it exceeds 4V
* `MVIO_DISABLED` (0x40)- If MVIO is either not supported on this part, or MVIO is disabled or the fuses are set to a nonsense value (call getMVIOStatus() for more diagnostics).
* `MVIO_UNSUPPORTED` (0x80)- If this part lacks either MVIO entirely, or the OPAMP peripheral.
* `MVIO_IMPOSSIBLE_CFG`(0x08) - If you passed dacref = -1 or omitted that argument, but the DAC reference is not set to an internal reference, and you didn't specify MVIO_SELECT_REF so we're not allowed to change it, or if you have passed a positive value as the dacref, but the dac reference is set to an internal reference, you will receive this error, indicating that you made an impossible request.
* `MVIO_OK` (0) - If no problems were encountered.


To be clear, this is what those last two options can be:
| options         | dacref | Meaning |
|-----------------|--------|---------|
| MVIO_SELECT_REF | -1     | Select and set the DAC reference to an internal ref to get the desired voltage. Our criteria is that the reference should be at least 100mV higher than the requested voltage. |
| MVIO_SELECT_REF | other  | Select and set the DAC reference to an internal ref as above, and ignore the inappropriate value passed as dacref. |
| 0               | -1     | I've already set the DAC reference to an internal reference (if not, returns MVIO_IMPOSSIBLE_CFG), turn on the DAC and opamp to buffer it and set it up. |
| 0               | >5500  | `MVIO_VOLTAGE_TOO_HIGH` - hopefully this was a mistake, and you aren't actually supplying a reference voltage that high, which would exceed the specifications of the chip. |
| 0               | 1750 > dacref > 0   | `MVIO_REF_TOO_LOW | MVIO_UNDERVOLTAGE` - that reference could never be used to derive voltages that could power MVIO, it's too low. |
| 0               | voltage > dacref > -1 | External reference selected, but lower than the requested voltage, `MVIO_REF_TOO_LOW`
| MVIO_SELECT_REF | -1, voltage > 4v | `MVIO_NO_VALID_REF` - there are no internal references higher than 4.096V, so there is no reference that will work for you.

Note that it is not possible to guarantee that if we didn't return MVIO_UNDERVOLTAGE, the voltage is *actually* high enough for MVIO to work, only that it *is expected to be* high enough. We cannot determine what the minimum voltage required for MVIO is except experimentally. If you did not get an error here, but getMVIOStatus() is giving MVIO_UNDERVOLTAGE, compare`analogRead(PIN_PD2)` and `analogRead(ADC_DAC)`. If PIN_PD2 is significantly lower than ADC_DAC, that indicates that you have too large of a load on VDDIO2, the OPAMP cannot supply that much current, and hence it's voltage is drooping enough that MVIO is unable to turn on. If both are nearly equal, and the voltage is not much higher than 1.65V, then your specimen has a particularly high threshold for MVIO turn-on (the specific turn-on threshold for MVIO is, according to the datasheet, not particularly tightly controlled). If an excessive load instead occurs only when load(s) on the VDDIO2 rail and an MVIO pin or pins which have been written HIGH, I would instead expect to see oscillatory behavior.

Notice that these errors do not mean the same things as they do when returned from getMVIOStatus(). You can call that for a more complete diagnosis of an MVIO issue.

```c++
uint8_t getMVIOStatus(bool debugmode = 0, HardwareSerial &dbgserial = Serial)
```

This returns one of the following constants:
```c++
MVIO_UNSUPPORTED      0x80 // 128 - You are calling this function on an AVR DA-series or EA-series. They do not support MVIO. We would like to generate a compiler warning in this case, however, that is not possible because I can't get the gcc warning attribute to work. Nobody I have asked has been able to explain what is wrong, nor has anyone reported success in using it.
MVIO_DISABLED         0x40 // 64  - MVIO is disabled via the fuses. If MVIO is set to disabled from tools submenu, we would like to generate a compiler warning, but we can't, per above.
MVIO_BAD_FUSE         0x20 // 32  - The MVIO configuration fuses are set to an invalid value (there are 2 bits, hence 4 possible values - but only 2 are legal)
// 0b00 and b11 are invalid; behavior is undefined. Some time I'll play with that and see what behavior you actually get. (or someone who has tried it can tell me).
MVIO_MENU_SET_WRONG   0x10 // 16  - the MVIO menu does not match the hardware, and you're either not using a bootloader (will also get IMPOSSIBLE_CFG) or you have selected the option that states that you're certain the fuse is set. That is considerably worse than MVIO_SETTING_MISMATCH.
MVIO_IMPOSSIBLE_CFG   0x08 // 8   - The MVIO menu does not match the hardware, yet we are not using a bootloader. This should not be possible unless you exported the hex file and manually uploaded it without correctly setting the fuses - it should be impossible to get here if you're using the Arduino IDE. If you can get this error with code uploaded by the IDE that is a bug which should be reported immediately.
MVIO_SETTING_MISMATCH 0x04 // 4   - MVIO is either enabled when menu says disabled, or the other way around. If just this is set, you're using optiboot and didn't tell the core that it was safe to assume the state of the MVIO fuse.
MVIO_UNDERVOLTAGE     0x01 // 1   - MVIO is enabled. The voltage on the VDDIO2 pin (if a voltage is applied at all) is too low for the port to function.
MVIO_OKAY             0x00 // 0   - MVIO is enabled, the voltage on the VDDIO2 pin is good, the port is ready to go!
```


By default (if no argument is passed to getMVIOStatus() it will not use "debugging mode", and will not print any output, even when severely broken conditions are found. Passing any non-zero value will enable debugging mode; this is written with the assumption that a human is reading the output of Serial, and, in the event of serious misconfigurations, will print out warning text. If it is being used with Serial connected to something else, and/or nobody is watching that serial port, no argument should be passed. As long as it its always called without non-zero argument, there will be no output to serial and the optimizer will be able to remove those print lines. In any event, even debugging mode will only generate any output in a small number of severely broken situations where core behavior in other areas will be impacted:
1. If you are using Optiboot, and have set the MVIO tools menu to the "Enabled, and "burn bootloader" has for sure done w/this selected." or "Disabled, and 'burn bootloader' was for sure done w/this selected", but the fuse is set to the opposite of what you specified. This causes breakage, because choosing that option tells the core to assume that it is configured that way (instead of checking the fuse - if you choose simply "Enabled" or "Disabled", we verify the value against the fuse - but we have to do so within a pin info macro, which isn't where you like to put it is not unexpected that an Optiboot configuration might not match the menu options, because they are only set on bootload (the bootloader cannot write it's own fuses). Telling the core that you're really sure you've set the fuse saves flash and improves performance, but if you're lying about that, the resulting behavior may be unexpected.
2. If you are not using Optiboot (and hence are uploading through UPDI), the SYSCFG1 fuse is set upon every upload. Thus, it should not be possible to end up with code on the chip that was compiled for MVIO being enabled when it wasn't or vice versa. In the event that this is detected, it will print a message and return `MVIO_MENU_SET_WRONG | MVIO_IMPOSSIBLE_CFG`  (numeric value 0x18).
3. If under any circumstances, the MVIO bits are set to a value that does not correspond to a valid MVIO setting, `MVIO_IMPOSSIBLE_CFG` will be returned and a warning message will be printed if `debugmode = 1`
4. If MVIO.STATUS is not 1, or MVIO.INTFLAGS is not 0, yet MVIO is disabled by the fuses. This should likely be raised with Microchip if observed.

Except for situation 1, which is most likely user error, or situation 4 (which would be a novel silicon bug), these situations can arise only when using a third party IDE that not configured to set the fuses, or is configured to set them incorrectly, or if that IDE does not pass the required define (`MVIO_ENABLED`) along to the compiler to tell it when MVIO is enabled. If cases 2 or 3 ever occur with the Arduino IDE, that is a serious bug that should be reported promptly along with any information on how you were able to make it happen.

Multiple errors can (and typically will be) be combined if they apply. `MVIO_MENU_SET_WRONG` only ever returned with at least MVIO_SETTING_MISMATCH, though the latter can appear without the former in Optiboot configurations, while in case 2, it is returned along with `MVIO_IMPOSIBLE_CFG` to indicate that it is not supposed to be possible to get the chip into this confiuration. It is also returned in weird situations that they datasheet says are impossible, for example if the system is set to single supply mode, but `MVIO.STATUS != 1` or `MVIO.INTFLAGS != 0`, directly contrary to the datasheet (MVIO chapter, functional description, under Initialization).

The error values should be tested on a bitwise basis (ie, bitwise and), not with equality (as they are almost always combined) under most use cases.  `if(getMVIOStatus()) {...}` will run the conditional statements if MVIO is **not** enabled and working. Similarly, `if(!getMVIOStatus()) {...}` or `if(!getMVIOStatus() == 0) {...}` will run the conditional if MVIO **is** supported and enabled, whether or not there is an appropriate VDDIO2 voltage supplied. The below example shows how one might print human readable description of the MVIO status:

```c++
void checkMVIO() {
  uint8_t status=getMVIOStatus(1);
  if(status & MVIO_DISABLED) {
    Serial.println("MVIO supported on chip but disabled in fuses.");
  } else if(status & MVIO_SETTING_MISMATCH) {
    Serial.println("MVIO fuse does not match menu selection.");
  } else if(status & MVIO_BAD_FUSE) {
    Serial.println("MVIO supported on chip but relevant fuses in invalid state.");
  } else if(status & MVIO_UNDERVOLTAGE) {
    Serial.println("MVIO enabled, VDDIO2 voltage is too low to support MVIO.");
  } else if(status & MVIO_UNSUPPORTED) {
    Serial.println("MVIO is not supported by this part.");
  } else if((status & MVIO_OKAY) && !(status & MVIO_DISABLED)) {
    Serial.println("MVIO is enabled and proper voltage is present!");
  }
}

```

```c++
int16_t getMVIOVoltage()
```
Very simply, this returns either an error code (these are all lower than 256) or the MVIO voltage, in millivolts, if there is no error (this is always larger than 1650). Negative numbers indicate an error from the ADC.

## Autotune
Autotune allows use of an external 32kHz crystal to automatically tune the internal high frequency oscillator. This is only accurate to within ~ 0.4% (the size of the tuning step on the internal oscillator). A very simple bit of code using this library is shown below as well.


```c
typedef enum X32K_OPT {
    X32K_LOWPWR_START31MS   = (CLKCTRL_CSUT_1K_gc   | CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START500MS  = (CLKCTRL_CSUT_16K_gc  | CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START1S     = (CLKCTRL_CSUT_32K_gc  | CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START2S     = (CLKCTRL_CSUT_64K_gc  | CLKCTRL_LPMODE_bm),
    X32K_HIGHPWR_START31MS  = (CLKCTRL_CSUT_1K_gc),
    X32K_HIGHPWR_START500MS = (CLKCTRL_CSUT_16K_gc),
    X32K_HIGHPWR_START1S    = (CLKCTRL_CSUT_32K_gc),
    X32K_HIGHPWR_START2S    = (CLKCTRL_CSUT_64K_gc),
    X32K_EXTCLK             = (CLKCTRL_SEL_bm)
} X32K_OPT_t;
/* X32K_HIGHPWR uses more current, but works with worse crystal configurations/wiring geometry.
 * Startup-time must be specified, either 1k, 16k, 32k, or 64k cycles, whenever a crystal is used, before we start trusting the crystal.
 * This library defaults to high power mode with maximum startup time,
 */

typedef enum X32K_ENABLE {
    X32K_DISABLED = (0x00),
    X32K_ENABLED  = (0x01),
    X32K_ALWAYSON = (0x81),
} X32K_ENABLE_t;
```

```c++
void configXOSC32K(X32K_OPT_t = X32K_HIGHPWR_START2S, X32K_ENABLE_t = X32K_ENABLED)
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. **This handles the enable-locking of these bits** and will stop the timer, wait for sync, write the new value, and again wait for sync.
// which means it may disable this clock source (CSUT is long enough that this may matter!)
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
  // Who would have thought, trying to use a clock that's not connected wouldn't work?
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
      Serial.println("Disabled autotune... ")
    }
  }
  Serial.println("Okay, we're done now! ");
  disableXOSC32K();
  Serial.println("Disabling the external oscillator - we give up!");

  // Done!
}
```





`*` See, if the product design process was a party, and the peripherals were guests, USART0 and SPI0 showed up early, and drained the PORTMUX punchbowl, snarfed the cake and left before most of the guests had even showed up, leaving most of the peripherals to suck on ice cubes and lick up crumbs. TCA skipped the event entirely, thinking its backroom deal with the EA would ensure it had a bright future. But as we've seen in the EB-product brief, WEX Luther and his crony, TCE, (who have have apparently escaped from xMegatraz), seem to have other ideas. He seems to think he's gonna change the world (I think he said change, might have been another word), and swears he's mended his ways since the xMega days, but I'm reserving judgement until we have a datasheet to look at. The fact that I can't tell from looking at the IO multiplexing chart which sets of TCE pin multiplexing option go together are not a particularly good sign... I'll be disappointed in them (not to mention Microchip, for letting them escape...) if they're still as unruly on the EB-series as they were on xMega. I can't say I'm terribly optimistic though. Why not? Well if they weren't up to something suspicious, they'd have just called it a TCE...
