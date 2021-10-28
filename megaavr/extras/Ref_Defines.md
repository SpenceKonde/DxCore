# Identifying menu options and part identity within sketch or library
It is often useful to identify what options are selected on the menus from within the sketch - one of the most critical is the millis timer the millis timer selection. You may know that you require a certain configuration and want to prevent accidentally compiling with the wrong option and wasting time chasing that instead of a more meaningful issue. *Submenu options are not saved on a sketch or board basis, but an IDE-wide basis* so you can easily get them mixed up.

You cannot check for options that depend on the fuses at compile-time, because we don't know what they will be - sketch can be uploaded separately from setting them.

## Millis timer
The option used for the millis/micros timekeeping is given by a define of the form `USE_MILLIS_TIMERxx`. Possible options are:
* `MILLIS_USE_TIMERA0`
* `MILLIS_USE_TIMERA1` (only on 48/64 pin DA/DB, and all EA)
* `MILLIS_USE_TIMERB0`
* `MILLIS_USE_TIMERB1` (default on 14/20-pin DD for now)
* `MILLIS_USE_TIMERB2` (Default except on 14/20 pin DD that don't have it)
* `MILLIS_USE_TIMERB3` (48/64-pin DA/DBV, all EA only)
* `MILLIS_USE_TIMERB4` (64-pin parts only)
* `MILLIS_USE_TIMERD0` (not supported on DA/DB series - there's no shortage of TCBs here). May be supported for DD due to their only having 2 type B timers on the 14/20 pin parts.
* `DISABLE_MILLIS`

### Using to check that correct menu option is selected
If your sketch requires that the B0 is used as the millis timer, for example:

```
#ifndef MILLIS_USE_TIMERB2
  #error "This sketch is written for use with TCB2 as the millis timing source"
#endif
```

## Identifying part family within sketch
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on. Defines of the form `__AVR_AVRxDAy__` are provided, where `x` is the size of the flash (in kb) and `y` is the number of pins, for example `__AVR_AVR128DA64__` for the part with 128K flash and 64 pins.

This core provides an additional define depending on the number of pins on the part and it's family (which identifies it's peripheral selection) and the DxCore version:
* `DA_28_PINS`
* `DA_32_PINS`
* `DA_48_PINS`
* `DA_64_PINS`
* `DB_28_PINS`
* `DB_32_PINS`
* `DB_48_PINS`
* `DB_64_PINS`
* `DD_14_PINS` (unreleased, support not enabled)
* `DD_20_PINS` (unreleased, support not enabled)
* `DD_28_PINS` (unreleased, support not enabled)
* `DD_32_PINS` (unreleased, support not enabled)
* `DX_14_PINS`
* `DX_20_PINS`
* `DX_28_PINS`
* `DX_32_PINS`
* `DX_48_PINS`
* `DX_64_PINS`
* `__AVR_DA__`
* `__AVR_DB__`
* `__AVR_DD__` (Unreleased. Last I heard it's release date is Q1 2021. Obviously, things seem to be a bit behind schedule)
* `__AVR_DU__` (unreleased and unofficial - a product brief was available for apparently a few hours and then pulled)
Whether Ex-series will be grouped with Dx is TBD depending on what the peripherals are like, but if, as expected, the only differences are the NVMCTRL and the ADC, they will be.
* `EA_28_PINS`
* `EA_32_PINS`
* `EA_48_PINS`
* `__AVR_EA__`
These define the version of the core:
* `DXCORE` - DxCore version number, as string. If it's "Unknown 1.3.7+" or similar, see below.
* `DXCORE_MAJOR` - DxCore major version
* `DXCORE_MINOR` - DxCore minor version
* `DXCORE_PATCH` - DxCore patch version
* `DXCORE_RELEASED` - 1 if a released version, 0 if unreleased (ie, installed from github between releases).
* `DXCORE_NUM` - DxCore version, as unsigned long.

```
Serial.println(DXCORE);
Serial.print(DXCORE_MAJOR);
Serial.print(' '); // using ' ' instead of " " saves a tiny amount of flash!
Serial.print(DXCORE_MINOR);
Serial.print(' ');
Serial.print(DXCORE_PATCH);
Serial.print(' ');
Serial.println(DXCORE_RELEASED);
Serial.println(DXCORE_NUM,HEX);

```
Will produce output like:
```
1.3.7
1 3 7 1
01030401
```
or - if your non-Arduino IDE is not able to handle the escaping and you happened to be using a 1.3.8 github build:
```
Unknown 1.3.7+
1 3 8 0
01030800
```
The various numeric representations of version are of interest to those writing libraries or code meant to be run by others. They are meant to ensure that there is always an easy way to test against specific versions (for when a release contains a regression), as well as an "X or better" type test. Warning: In some third party development environments, the `DXCORE` define is lost. Since that's how we tell people to detect if DxCore is in use (`#if defined(DXCORE)`) we now detect that and put define it as "Unknown 1.3.7+" (where the number changes whenever I happen to notice it while editing the file for a new version). `DXCORE_NUM` is by far the easiest way to detect if a certain fix or patch is present.


## __AVR_ARCH__
This can be set to 102, 103, or 104 depending on flash size - the compiler sets this, and it works foer ALL AVR parts. Anything with it set to 103 has fully mapped flash. Anything with 102 has 64k, etc.
* `__AVR_ARCH__ == 103` - All parts where all of the flash is mapped in the data space. This means Dx-series parts with 32k or less of flash, tinyAVR 0/1/2-series, and megaAVR 0-series.
* `__AVR_ARCH__ == 104` - Parts with 128Kb of flash, mapped flash is split into 4 sections (AVR128DA, AVR128DB).
* `__AVR_ARCH__ == 102` - Parts with 64Kb of flash, mapped flash is split into 2 sections (AVR64DA, AVR64DB).

## Core feature detection
* `DEVICE_PORTMUX_TCA = 2` 1 = each wave output cannnel can be moved individually, like tinyAVRs. 2 = Whole thing moved at once
There are a number of macros for determining what (if any) features the core supports (these are shared with megaTinyCore and ATTinyCore (2.0.0+)
* `CORE_HAS_FASTIO = 1` - If defined as 1 or higher. indicates that digitalWriteFast() and digitalReadFast() is available. If undefined or defined as 0, these are not available. If other "fast" versions are implemented, they would be signified by this being defined as a higher number. If defined as -1, there are no digital____Fast() functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction).
* `CORE_HAS_OPENDRAIN = 1` - If defined as 1, indicates that openDrain() and (assuming `CORE_HAS_FASTIO` is >= 1) openDrainFast() are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG = 1` - If defined as Indicates that pinConfigure() is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER = 1` - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
* `CORE_HAS_TIMER_RESUME = 1`- if defined as 1, the corresponding resumeTCxn() functions, which reinitialize them and return them to their normal core-integrated functions, are available.
* `ADC_NATIVE_RESOLUTION = 1`- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW = 10` - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `ADC_DIFFERENTIAL = 1` - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrainted to between Gnd and V<sub>Ref</sub>), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core. If it's defined as -1, it has differential capability in the way that classic AVRs do. See also `CORE_HAS_ANALOG_DIFF`
* `SUPPORT_LONG_TONES = 1`  - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, and some cores may make the choice to not address it (megaTinyCore only supports long tones on parts with more than 8k of flash).  If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH = 1` - If defined as 1, analogReadEnh() (enhanced analogRead) is available. Otherwise, it is not.
* `CORE_HAS_ANALOG_DIFF = 1` - If defined as 1, analogReadDiff() (differential enhanced analogRead) is available. Otherwise, it is not.  It has same features as enhanced, except that it takes a differential measurement. If this is -128, (128 unsigned), it is a classic AVR, not a modern one with a differential ADC, and the core's analogRead implementation accepts the constants listed in the core documentation to make an analogRead
* `ADC_MAX_OVERSAMPLED_RESOLUTION = 15` - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN = 0 (DA, DD, DU) or -1 (DB)` - Some parts' ADC is blessed with a Programmable Gain Amplifier (PGA) amplifier, often used for differential readings (though if the 2-series is any guide, the EA-series will support it for all readings) The Dx-series are not among them, though the EA will have it. If this is defined as a positive number, it is the maximum gain available (16 on the EA). If this is defined as 0 that means there is no way to amplify an analog signal we might want to measure (if this is a problem you need external opamps). If this is defined as -1 (or 255 if stuffed into a uint8), there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation and part specific work would be needed; they may be more flexible in some ways, but they are very rigid in others (for example, in that there is only one pin option for them. If it is defined as -128 (128 in a uint8) there is a gain stage on the differential ADC, but it is specified along with the pair of pins, not with separate configuration options. This also means that it's *NOT* a modern AVR! This is generally what is done on all classic AVRs with a differential ADC) so the available gain options depend on which pins are being measured, and there is a different procedure to use it, as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is undefined, there is definitley no support exposed through the core, and it may not be a feature of the hardware at all.
* `PORT_ID_INLVL = 1 on DD/DB only` - If 1, the input levels can be switched between TTL and Schmitt trigger. If undefined or 0, they cannot be.

Next up is information about the part number:
* `_AVR_FAMILY` - String - "DA", "DB", "DD, "DU, or "EA" depending on what kind of part it  is.
* `_AVR_PINCOUNT` - The number of physical pins
* `_AVR_FLASH` - Flash size, in KB - these three can be used to print the human readable part number easily.

Finally, really important nasty errata are listed. (minor ones aren't). If the erratum is present, they will be 1, and if it never impacted this part, it not be defined. If/when these are fixed, they get replaced with a macro to check REVID and see if the bug is there or not (eg, `(SYSCFG.REVID >= 0xA5 ? 0 : 1)` for a bug fixed in A5 revision, like the ZCD PORTMUX bug. See the [errata summary](./Errata.md) for details on them, and the example below for the best way to test them

* `ERRATA_ADC_PIN_DISABLE`  - The bug on DA with digital input being disabled on the pin the ADC is pointed at. Transparently worked around by the core.
* `ERRATA_TCA1_PORTMUX`     - The bug on DA 128 where the last 2 settings don't work for TCA1.
* `ERRATA_NVM_ST_BUG`       - The bug on DA 128 where memory protection is applied to more areas than it should be when writing using ST (our bootloader and the flash module use SPM instead. It's faster too.)
* `ERRATA_TCB_CCMP`         - Impacts all parts with TCBs. CCMPL and CCMPH cannot be written individually. You must write both together, in that order, because they're treated like a 16-bit register, even in PWM mode.
* `ERRATA_CCL_PROTECTION`   - Impacts all parts with a CCL. CCL settings cannot be changed without disabling the whole CCL - not just the LUT, which was apparently the intent.
* `ERRATA_TCD_PORTMUX`      - Impacts all DA/DB parts. Only the default TCD0 port mapping works.

```c++
int8_t funWithTCA1() {
  #if defined(ERRATA_TCA1_PORTMNUX) // Test if it's defined with preprocessor - so you can avoid any runtime cost if it never impacted that part.
    if (__builtin_constant_p(ERRATA_TCA1_PORTMUX)) // test if it's constant and throw an error at compile time if the part has the bug and no silicon without the bug has been released.
      badCall("This chip does not support directing output to PG0:5 or PE4:7. This impacts all currently available die revisions"); //prevents compilation.
    if (ERRATA_TCA1_PORTMUX)       // Whether a potentially impacted part has the working die rev can ONLY be tested at runtime!
      return -1; // Return error code.
    doSomethingWithTCA1PinMapping();
    return 0; // Uses the unix-like 0 = success convention
  #endif
}
void setup() {
  if funWithTCA1() {
    Serial.println("ERROR: This chip cannot direct output from TCA1 to PG0:5 or PE4:7, use a newer chip to ");
  } else {
    partyDown();  // defined elsewhere.
  }
}
```
