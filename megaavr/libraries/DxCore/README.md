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

**Not yet implemented**
```c
int16_t millivoltsToADCValue(int16_t mv, int16_t ref = -1, const uint8_t bits = /* current resolution setting*/)
```
`ref` is in millivolts, or one of the following constants (these constants are the theoretical values. If you have more accurately measured them, store it in EEPROM or USERROW and use those instead of these constants. Constants that are references for peripherals the device doesn have will be caught at compile time if known most of the time, and if not will return the appropriate error code (shown below)
* `REF_MV_ADC`  - Whatever the currently selected ADC reference is    (numeric value:    -1)
* `REF_MV_ADC0` - synonym for above.
* `REF_MV_ADC1` - Whatever the currently selected ADC1 reference is   (numeric value:    -2) - tinyAVR 1+Series only
* `REF_MV_AC0`  - Whatever the currently select AC0 reference is      (numeric value:    -8) - Syn. for REF_MV_DAC0 on tinyAVR 1-/1+Series.
* `REF_MV_AC0`  - Whatever the currently select AC0 reference is      (numeric value:    -9) - If the device has one. Syn. for REF_MV_DAC1 on tinyAVR 1+Series
* `REF_MV_AC0`  - Whatever the currently select AC0 reference is      (numeric value:   -10) - If the device has one. Syn. for REF_MV_DAC2 on tinyAVR 1+Series
* `REF_MV_DAC`  - Whatever the currently selected DAC reference is    (numeric value:   -16) - if the device has one.
* `REF_MV_DAC0` - Synonym for above.
* `REF_MV_DAC1` - Whatever the currently selected DAC reference is    (numeric value:   -17) - if the device has one.
* `REF_MV_DAC2` - Whatever the currently selected DAC reference is    (numeric value:   -18) - if the device has one.
* For specific internal references, or for external ones, pass the number of millivolts (1024, 2048, 2500 or 4096 except on tinyAVR0/1).

`mv` - voltage in millivolts. If the device has a differential ADC and you've investigating a differential measurement, you can pass negative numbers. Remember than anything without the differential ADC *with the PGA* (EA and tiny2) must measure voltages that are not higher than VREF, while those with the PGA can go slightly above Vdd, though of course you can only get meaningful values if the difference, after the gain is applied, is less than VREF.

`bits` - should be the a number of bits that is either 8, 10, or (for parts with 12-bit ADCs) 12. Leftshift the answer as appropriate if you are oversampling and decimating with analogReadEnh if needed. Be aware that this function assumes a perfect ADC, and such a thing does not exist. Claiming to tell you what value it would give you for 3400mv if measured on the 4096mv reference to 15bits through oversampling is dishonest - see the ADC specs in the latest version of the datasheet - in the same way that I don't like the official Arduino analogReadResolution will permits values the hardware is not accurate enough to provide and pads the result with 0's

By using millivolts, we avoid having to use ~bloating~ floating point numbers which hog flash and bloat application size.

A negative value on a device that does not have a differential ADC will return 0 for voltages that are not outside the absolute maximum values. Similarly with positive voltages/

This may return the following additional error codes:
* `PERIPHERAL_NOT_PRESENT` - If one of the above REF_MV_... constants is used, but this device doesn't have that reference. (numeric value -32100)
* `VOLTAGE_TOO_LOW` - If the voltage is lower than the minimum voltage permitted by the datasheet to apply to a pin, but was not known at compile time (if it is, we generate an error then) (numeric value -32101)
* `VOLTAGE_TOO_HIGH` - If the voltage is higher than the maximum voltage permitted by the datasheet to apply to a pin at it's maximum operating voltage, but was not known at compile time (if it is, we generate an error then) (numeric value -32102)
* `INVALID_PRECISION` - If the bits is not 8, 10, or 12 (12 only on devices with a 12-bit ADC), but was not known at compile time (if it is, we generate an error then) (numeric value -32003)
* `VREF_CONFIGURATION_INVALID` - if a REF_MV_... constant is used, and we find that the VREF is set to a value that is listed as reserved (if on a DD, this would mean it was set to 0x04 or 0x07). Only possible if set manually.  (numeric value -32104)

**not yet implemented**
```c
int16_t adcValueToMillivolts(int16_t value, int16_t ref = -1, const uint8_t bits = -1/* current resolution setting*/)
int32_t adcValueToMillivolts(int32_t value, int16_t ref = -1, const uint8_t bits = -1/* default 12. Can take any resolution setting valid for these parts*/, const uint8_t gain = 0)
float adcValueToMillivolts(int32_t value, int16_t ref, const uint8_t bits,/* default 12. Can take any resolution setting valid for these parts*/, const float gain = 0.0)
```
Does the opposite of the above they take a value from the ADC or a value that the DACREF or DAC could be set to, and return the number of millivolts it corresponds to. The second argument can use any of the values listed above.
1) there are three versions.
  a) if passed the value as an int16_t, it treats it as having come from analog read (thus bits must be 8, 10 or 12), and default will use the ADC reference and ADC resolution. if using an external reference, you must pass the voltage of that referennce in millivolts. If REF_MV_DAC or REF_MV_AC is specified as the second argument, that referennce will be used. 8 is the only valid `bits` value to AC ref options. For DAC options, 8 is the default for the DAC, but 10 is also valid.
  Pass -1 to either of those arguments to use the default.
  b) If passed the value as a 32-bit integer, any bits may be given any value between 8 and the highest value supported for the resolution of analogReadEnh on that part when ref >= -1. (otherwise the same constraints apply), because this function is explicitly to help you convert between ADC or DAC values, and the voltage they represent. So specifying the DAC reference voltage at 12 bits for example implies that you think the DAC is 12 bits, which it of course is not, so we give an error (and if you're using the ADC to measure one of those values, the ADC reference is the one that is relevant and should be indicated by the second argument)
  c) If a fourth argument is given, it must be 0 or 1 for all parts without a PGA, otherwise 0, 1, 2, 4, 8 or 16 (ie, valid gain values). 0 and 1 are treated identically. This calculates the voltage assuming the ADC value was measured with the specified gain. If instead of an integer, a floating point value is passed as the fourth argument, the return value will be a float instead of an int32.

The 16-bit version can return the following error codes:
* `PERIPHERAL_NOT_PRESENT` - If one of the above REF_MV_... constants is used, but this device doesn't have that reference. (numeric value -32100)
* `VALUE_TOO_LOW` - If the negative number is passed to the 16-bit version as the `value`,this error code is returned, analog read does not return negative values. This will only be shown if the value is not known to be invalid at compile time (if it is, we generate an error then) (numeric value -32101, ie, it is the same code as VOLTAGE_TOO_LOW)
* `VALUE_TOO_HIGH` - If the value passed is larger than 2<sup>bits-1</sup>, and hence could not have come from the ADC with those settings, this error is returned. If we know at compile time that this will happen, we generate an error then) (numeric value -32102, same code as VOLTAGE_TOO_HIGH)
* `INVALID_PRECISION` - If the value of `bits` is not one of the allowed values described above, this is the error code given) (numeric value -32103)
* `VREF_CONFIGURATION_INVALID` - if a REF_MV_... constant is used, and we find that the VREF is set to a value that is listed as reserved (if on a DD, this would mean it was set to 0x04 or 0x07). Only possible if set manually.  (numeric value -32104)

The 32-bit version can return the following error codes:
* `PERIPHERAL_NOT_PRESENT` - If one of the above REF_MV_... constants is used, but this device doesn't have that reference. (numeric value -2,100,000,100)
* `VALUE_TOO_LOW` - If a negative number larger than the maximum negative value the differential ADC can output with the stated number bits (ie, -1 x 2<sup>bits-1) is passed as a 32-bit signed integer `value` this error code is returned. This will only be shown if the value is not known to be invalid at compile time (if it is, we generate an error then) (numeric value -2,100,000,101)
* `VALUE_TOO_HIGH` - If the value passed is larger than 2<sup>bits-1</sup>, and hence could not have come from the ADC with those settings, this error is returned. If we know at compile time that this will happen, we generate an error then) (numeric value -2,100,000,102)
* `INVALID_PRECISION` - If the value of `bits` is not one of the allowed values described above, this is the error code given) (numeric value -2,100,000,103)
* `VREF_CONFIGURATION_INVALID` - if a REF_MV_... constant is used, and we find that the VREF is set to a value that is listed as reserved (if on a DD, this would mean it was set to 0x04 or 0x07). Only possible if set manually.  (numeric value -2,100,000,104)


## Multi-Voltage IO

The AVR DB-series and DD-series supports a new feature, Multi-Voltage IO (MVIO). When enabled by the appropriate fuse settings, the I/O voltage used for PORTC will be the voltage applied to the VDDIO2 pin (actually in my tests, this was always the case - regardless of whether it was enabled by the fuse - the fuse didn't internally connect them or anything, and there was no increased current that would suggest damage was occurring. However, with the fuse set to single supply mode, PORTC would behave erratically when VDDIO2 was too low, instead of handling that situation gracefully). A new function is provided, `getMVIOStatus()`, which returns a value depending on whether MVIO is supported, whether it is enabled, and if so, whether VDDIO2 voltage is high enough that the MVIO pins (PORTC) are available; MVIO requires VDDIO2 to be at least 1.65V - **Warning: Read that again. *IF MVIO IS TURNED OFF IN FUSES, VDDIO2 MUST BE CONNECTED TO VDD*.** Otherwise, you are entering the realm of **UNDEFINED BEHAVIOR** - quick tests showed nothing surprising happening as long as the VDDIO2 voltage was in range, but this is **NOT** recommended as a way to save power, as there is no guarantee that it does not risk damaging the chip. **When VDDIO2 was below the minimum voltage range, unconnected to VDD, and MVIO was disabled, the behavior of the port is not only undefined, but known to be in bizarre and very wrong** - think random results for reads, and likely output as well; **We cannot exclude the possibility that damage to the chip could be caused opperating in this incorrectly configured regime**; the datasheet uses the keyword "must" to describe the the need to connect VDD and VDDIO when MVIO is not enabled, not "should". That's a much more forceful word.

So don't do that. If MVIO is disabled, FFS, connect VDDIO2 to VDD like the datasheet says you must.
If MVIO is intended to be enabled, MAKE SURE THAT IT IS.

(unfortunately, there *isn't* a way to measure the voltage being supplied to the VDDIO2 pin if MVIO is disabled in fuses. VDDIO2DIV10 just reads as 0 - why is this unfotunate? Because, particularly with TQFP packages, it is possible for a cold solder joint that looks good to be invisible to visual inspection, and when you probe it with a multimeter, that will press down the pin and bring it into contact, and you will think it's connected (unless the stuff on PORTC just came to life all of a sudden which would be a dead giveaway. Avoiding such cold solder joints is one of the reasons that you need to use flux while drag soldering).

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
* Keep in mind that you may have as few as THREE PINS that operate at the MVIO voltage, and at most 8. Note that these always contain a serial port, an SPI port, and an I2C port. On parts with 28 or 32 pins, both serial and I2C can be used at the same time, and on parts with more pins, all three can. An example of wise pin layount that is to be commended, especially considering that the decisions were cast in silicon back in 2016 (for the larger parts - the parts with only 3 pins had to be give an extra serial mux option on USART0 to use the port at all since they otherwise lost their TX pin - but like, with only 14 pins, 3 of them used by power, and 2 more by reset and updi... something had to give (up pins) and in this case. At 20 pins they've amputated all the non-funky pins of PF, needed 4 more so off with the low half of PD (they needed the high half cause that's where the DAC goes. Based on the precedent of ZCD3, they'd have had to call it DAC1 if they wanted the sole dac to come out any other pin), and all the pins of portA that aren't needed for the crystal were already on the chopping block. Something was gonna come off of PORTC, or the part would have had to grow a 21st pin. And so PC0 got the axe and USART0 took another glass of the PORTMUX-punch `*`, as did SPI0 to get, respectively a C1, C2, and a C1, C2, C3, F6 (reset) option.
* You may find that you don't have enough MVIO pins. AVR provides many solutions here. You only *need* to use MVIO or a level shifter if you need to drive a pin connected to a lower voltage part to that part's HIGH Otherwise:
  * If you only need to read the pin, on a DD, DB, or EA, set the INLVL voltage to TTL (using either pinConfigure per the digital reference, or manually writing the PORTx.PINnCTRL register bit),
  * If you need to read the pin and the part is at a higher voltage, you can usually use a resistor divider.
    * This does no favors for power consumption if a pin is normally HIGH though, but in that case, you can turn on the internal pullup and put a small signal diode with the band towards the high voltage device, so the pin will be pulled up to this device's VDD unless the external pin is driven low.
  * If you need to only drive a pin LOW, and a pullup to it's lower Vdd will suffice or it has an internal pullup, see the openDrain() and openDrainFast() API extensions in the digital reference for s friendlier way to do this.
    * Even serial can do this on the TX line: Use ODME, and pull the lines up with external pullups to the power rail of the device you're talking to.
  * For I2C, on some parts (see the datasheet and/or wire documentation) there is an SMBus voltage option. This the I2C analog of the above mentioned INLVL option, and will make the thresholds vastly lower. Be sure in this case that you do not call Wire.usePullups() and that the pullups are connected to a voltage all devices find acceptable. Power up the system before making this measurement anddouble-check that the place you're planning to connect them to really is at the voltage you think. Also, while a rather unlikely case, if the SMBus voltages are much lower than the device's voltages - and it's threshold for a high is higher, you may need to lower the baud rate or use stronger pullups, because the TWI will start counting the time of the HIGH portion of the clock when it hits the SMBus HIGH threshold (see the datasheet or Wire documentation).
* As you can see from above, the things that REALLY NEED the MVIO functionality are SPI or bidirecional UART, in addition to push-pull GPIO.


It is likely that the most common use cases will see the AVR running with VDD = 5V and VDDIO2 at a lower voltage like 3.3, 3.0, 2.5, 2.1. 2.0, or 1.8V (these are common voltages used in electronics) and AVRs are unusual among modern microcontrollers in that they can be run at 5V. Another common application would be a battery powered system running from a single cell LiPo battery (because you don't want to run at 3.3v from an LDO because the LDO will hog power, which is used directly to power the chip with no regulator (to lose the quiescent current of the regulator) which must be able to talk to devices standard "electronics voltages", though if the voltage is higher than Vdd. You still need to get the voltage to the VDDIO2 pin, though - maybe it has a pin that supplies it's Vdd but you still need to be able to operate it when it's not plugged in, or maybe you know what that voltage is, and can use the below-described OPAMP method to generate the IO voltage. For a concrete example of what sort of thing might do this, imagine something that you carry around and can set up via buttons and a screen, sleeps when not in use, and is made for the purpose of being plugged into other devices that run from mains power, like a standalone programmer to update firmware.

If running on batteries and you DON'T need MVIO, be sure to turn it off in the fuses, as it burns a small amount of power running what amounts to a less rigorous BOD mechanism (I say less rigorous because it's current is markedly lower; it's likely sampling less often).

###  Running off the internal opamp
This is a feature of the DB's that Microchip Marketing loves to talk about. And it is pretty F'ing cool, I'm not arguing with that. But it's only 20mA which is means there are a limited number of practical uses for it. Probably the most likely in Arduinoland is those damned fancypants new sensors developed for cell phones and the like, with unreal specs, but obnoxiously low maximum voltages (I've seen 2.5, 2.7 and 2.8, and I think once 2.1) - since their current consumption is often minuscule (that's one of their selling points) and they don't run at a high enough voltage for many hobbist applications, and sometimes don't even work with cheap level shifters (the classic bidirectional open drain level shifter often used for I2C will have a problem with that if they cheaped out on the FETs - It would be pretty shoddy ogf tyhm).

#### Limitations and Restrictions
* This requires an OPAMP peripheral. The DD-series doesn't have those.
* This uses the single DAC.
* This uses pins PD6 and PD2.
* The OPAMP is good to maybe 20mA, less near the edges of it's range.
  * This is a vast improvement over the DAC's sub-milliamp current capability.
* This uses an OPAMP, leaving 1 on 32 or 28 pin parts (hence precluding cascading two of them) and 2 on larger ones (hence precluding the instrumentation amplifier configuration described in the datasheet)

#### Required Connections:
1. Run a jumper from the OPAMP output pin to VDDIO2.
2. Disconnect anything else from PD6, which is used for the DAC, and the DAC output buffer must be enabled to use it as an input to the OPAMP.

*note: Owners of an Azduino Nano DB or nano DB+ do not need an external wire, as there are pads on the bottom that can be bridged with solder to select the OPAMP0 output. Refer to the documentation for more information*

#### Required code

```c
int16_t initMVIO_OPAMP(uint16_t voltage, const int8_t opamp = 0,  const uint8_t options = 0, const int16_t dacref = -1)
```
voltage and dacref are in millivolts.

`voltage` is the volage that you want.

`dacref` is needed only when the DAC reference voltage is set to external. If omitted or -1, an internal reference is assumed. If DAC reference set to external, this is it's voltage, in millivolts.

`options` allows a small number of options to be passed. These can be OR'ed together if needed.
* `MVIO_SELECT_REF` - If this option is selected, `dacref` is ignored. The lowest reference voltage that is high enough to supply the requested voltage is returned. Remember that you need some headroom.

This can return any of the following:
* `MVIO_VOLTAGE_TOO_LOW` - Voltage requested is under 1.65V, which is the minimum required by MVIO (the exact threshold varies between parts, and we can't determine that *a priori*.
* `MVIO_VOLTAGE_TOO_HIGH` - if `voltage` or `dacref` exceeds 5.5V.
* `MVIO_REF_TOO_LOW` - if the voltage requested is higher than the current DAC reference voltage. The core does NOT change the DAC reference voltage unless you tell it to.


```c++
uint8_t getMVIOStatus(bool debugmode = 0, HardwareSerial &dbgserial = Serial)
```

This returns one of the following constants:
```c++
MVIO_UNSUPPORTED      0x80 // 128 - You are calling this function on an AVR DA-series or EA-series. They do not support MVIO.
MVIO_DISABLED         0x40 // 64  - MVIO is disabled via the fuses.
MVIO_BAD_FUSE         0x20 // 32  - The MVIO configuration fuses are set to an invalid value (there are 2 bits, but only 2 legal values.
// 0b00 and b11 are invalid; behavior is undefined. Some time I'll play with that and see what behavior you actually get. I'd love it if one bit
// controlled the ADC VDDIO2 channel and the other controlled the MVIO PORT BOD,
// so one xould get a safeguard that power was actually being delivered to VDDIO2 and hence that you had soldered it properly when running in single supply mode, without having to burn the extra power that MVIO does (which is spent running what is effectively a second BOD for VDDIO2.
MVIO_MENU_SET_WRONG   0x10 // 16  - the MVIO menu does not match the hardware, and you're either not using a bootloader (will also get IMPOSSIBLE_CFG) or you have the option that states that you're certain the fuse is set. That is considerably worse than having the other incorrect option.
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

Very simply, this returns either an error code (as a negative value) or the MVIO voltage, in millivolts, if there is no error.

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
      Serial.println("Disabled autotube... ")
    }
  }
  Serial.println("Okay, we're done now! ");
  disableXOSC32K();
  Serial.println("Disabling the external oscillator - we give up!");

  // Done!
}
```

## Enabling closely related things





`*` See, if the product design process was a party, and the peripherals were guests, USART0 and SPI0 showed up early, and drained the PORTMUX punchbowl, snarfed the cake and left before most of the guests had even showed up, leaving most of the peripherals to suck on ice cubes and lick up crumbs. TCA skipped the event entirely, thinking it's backroom deal with the EA would ensure it had a bright future. But as we've seen in the EB-product brief, WEX Luther and his crony, TCE, (who have have apparently escaped from xMegatraz), seem to have other ideas. He seems to think he's gonna change the world (I think he said change, might have been another word), and swears he's mended his ways since the xMega days, but I'm reserving judgement until we have a datasheet to look at. The fact that I can't tell from looking at the IO multiplexing chart which sets of TCE pin multiplexing option go together are not a particularly good sign... I'll be disappointed in them (not to mention Microchip, for letting them escape...) if they're still as unruly on the EB-series as they were on xMega. I can't say I'm terribly optimistic though. Why not? Well if they weren't up to something suspicious, they'd have just called it a TCE...
