# ADC/DAC Reference for DxCore
The Dx-series parts have a powerful ADC, with higher resolution, automatic accumulation, maximally convenient reference voltages and more. The one that the EA-series will have is even better.

References to the Ex-series are based on the assumption that it is the same ADC that the tinyAVR 2-series got; no released material has suggested otherwise and where details have been released, they match exactly.

## Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:

 | AVR Dx/Ex-series (all)                  | Voltage | Minimum Vdd | Number |
 |-----------------------------------------|---------|-------------|--------|
 | `VDD` (default)                         | Vcc/Vdd |             |      5 |
 | `INTERNAL1V024`                         | 1.024 V |      2.5* V |      0 |
 | `INTERNAL2V048`                         | 2.048 V |      2.5  V |      1 |
 | `INTERNAL4V096`                         | 4.096 V |      4.55 V |      2 |
 | `INTERNAL2V500`                         | 2.500 V |      2.7  V |      3 |
 | `INTERNAL4V1` (alias of INTERNAL4V096)  | 4.006 V |      4.55 V |      2 |
 | `EXTERNAL`                              | VREF pin|        VREF |      6 |

 You can test like `if(getAnalogReference()==INTERNAL2V500)`, but if you try to say, print them, you just get a number. That's what is shown in the last columncolumn contains the numerical value of the constants representing these references. Don't use those, then nobody will understand your code - including yourself in two weeks. However, if you are printing the results of `getAnalogReference()` or `getDACReference()`, these are the numbers you will see.

 External reference voltage should be between 1.8v and Vdd. `INTERNAL` is not a supported reference, since there is no obvious reason why one of the four reference options should be used. These reference voltages are presented as they are in the datasheet, but do note that they are what the manufacturer targeted, not what the actual voltage is: The spec is +/- 4% over the temperature range of -40 to 85 C, when Vdd is at least the voltage specified above - though at typical conditions, every time I've checked, it was within 1%; The numbers they give for the voltages are specified to three decimal places (obviously that is convenient because then 1 mV = 1, 2, or 4 LSB), that should by no means be interpreted as a claim that they are *that* accurate. It is likely that the 1.024 V reference is usable below 2.5 V, but they do not guarantee it it's properties, and I do not know whether it remains useful all the way down to the minimum operating voltage of 1.8 V - I suspect it does, but verify first (please let me know if you have further information). Reference voltages exceeding the supply voltage will produce meaningless results and should not be used.

## Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin number:
| DA-series               | DB and DD-series          | EA-series (speculative) |
|-------------------------|---------------------------|-------------------------|
| `ADC_GROUND`            | `ADC_VDDDIV10`            | `ADC_VDDDIV10`          |
| `ADC_TEMPERATURE`       | `ADC_VDDIO2DIV10`         | `ADC_VDDIO2DIV10`       |
| `ADC_DAC0`              | `ADC_GROUND`              | `ADC_GROUND`            |
| `ADC_DACREF0`           | `ADC_TEMPERATURE `        | `ADC_TEMPERATURE `      |
| `ADC_DACREF1`           | `ADC_DAC0`                | `ADC_DAC0`              |
| `ADC_DACREF2`           | `ADC_DACREF0`             | `ADC_DACREF0`           |
|                         | `ADC_DACREF1`             | `ADC_DACREF1`           |
|                         | `ADC_DACREF2`             |                         |

The Ground internal sources are presumable meant to help correct for offset error. On Classic AVRs they made a point of talking about offset cal for differential channels, and often all the channels could be measured

DACREF0-2 are the the reference voltages for the analog comparators. On the DA-series, there is no way to measure the supply voltage other than using DAC or DACREF source: you can neither directly measure a reference voltage like some parts, nor is there any way to get a fraction of the supply voltage like the DB and DD-series support.  Note also that on the DB series, you can't measure the outputs of the OPAMPs directly - you must output to the pin and measure that, however much the high-level descriptions sound like there is a way to route the opamp signals internally.

## Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered using the accumulation feature allowing up to 15 bits of resolution (17 on future Ex-series); this is exposed through the `analogReadEnh()` function detailed below.

## ADC Sampling Speed
DxCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs. The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 2 MHz (up to 3 MHz with internal ref, twice that on external of Vdd reference at full resolution. We use use 1.0 to 1.5 MHz on Dx, and will target 2-2.5 by default on EA unless someone provides information that shuggests I shouldn't.  To compensate for the faster ADC clock, we extend the sampling period so it ends up with a similar sampling period to classic AVRs, while still being sgnoificant;y faster. On the 2-series, we'll  aim a bit lower with the sample duration as wecan actually calculate some numbers

## ADC Function Reference
This core includes the following ADC-related functions. Out of the box, analogRead() is intended to be directly compatible with the standard Arduino implementation. Additional functions are provided to use the advanced functionality of these parts and further tune the ADC to your application.

### getAnalogReference() and getDACReference()
These return the numbers listes in the reference table at the top as a `uint8_t`

### analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10 for compatibility. Negative return values indicate an error that we were not able to detect at compile time. Return type is a 16-bit signed integer (`int` or `int16_t`).

### analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. *If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits*. Note that this can only happen when the value passed to it is determined at runtime - if you are passing a compile-time known constant which is invalid, we will issue a compile error. The only valid values are those that are supported natively by the hardware, plus 10 bit, even if not natively supported, for compatibility.
Hence, the only valid values are 10 and 12. The EA-series will likely launch with the same 8bit resolution option as tinyAVR 2-series which would add 8 to that list.

This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resources of an 8-bit AVR with this is a silly waste of resources, and padding a reading with zeros so it matches the format of a more precise measuring method is in questionable territory in terms of best practices. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution while keep the extra bits meaningful at the cost of slowing down the reading. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument, and restore the previous setting.

### analogSampleDuration(duration)
Sets sampling duration (SAMPLEN), the sampling time will be 2 + SAMPLEN ADC clock cycles. Sampling time is the time the sample and hold capacitor is connected to the source before beginning conversion. For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length.
speed. This returns a `bool` - it is `true` if value is valid.

This value is used for all analog measurement functions.

The DxCore default is 14, which will result in a 16 ADC clock sampling time. For most signals this is more than needed, and the Microchip default is 0, giving 2, while documentation indicates that the ADC is optimized for signals with an impedance of around 10k. Reducing it to the minimum will approximately cut the time analogRead() takes in half. It will result in less accurate readings of high impedance signals, though it's not clear how high they can be before this becomes a concern.

On the 2-series, we are at least iven some numbers: 8pF for the sample and hold cap, and 10k input resistance so a 10k source impedance would give 0.16us time constant, implying that even a 4 ADC clock sampling time is excessive, but at such clock speeds, impedance much above that would need a longer sampling period. You probab

### analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 15 bits for Dx-series parts, and 17 for Ex-series. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res` native resolution) is selected. Note that maximum sample burst reads are not instantaneous, and in the most extreme cases can take milliseconds. Depending on the nature of the signal - or the realtime demands of your application - the time required for all those samples may limit the resolution that is acceptable. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See Atmel app note AVR121 (https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf - the specific case of the new ADC on the Ex and tinyAVR 2-series is discussed in DS40002200 from Microchip, but it is a far less informative document). Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 128 (Dx-series), or up to 1024 (Ex-series). The Dx-series only makes available the 16 MSBs, so when accumulating more than 16 samples, the value will be truncated to 16 bits. Be aware that the lowest bits of a raw accumulated reading should not be trusted.; they're noise, not data (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On Ex-series parts *the PGA can be used for single ended measurements*. Valid options for gain on the Ex-series are 0 (PGA disabled, default), 1 (unity gain - may be appropriate under some circumstances, though I don't know what those are), or powers of 2 up to 16 (for 2x to 16x gain). On Dx-series parts, the gain argument should be omitted or 0; these do not have a PGA.

```c++
  int32_t adc_reading = analogReadEnh(PIN_PD2,13);
  // Measure voltage on PD2, to 13 bits of resolution. This will be sampled 4 times using the accumulation function, and then rightshifted once.
  int32_t adc_reading2 = analogReadEnh(PIN_PD2, ADC_ACC128);
  // Take 128 samples and accumulate them. This value, k, is 19 bits wide; on the Dx-series parts, this is truncated to 16 bits - the hardware does not expose the three LSBs.
```

Negative values always indicate a runtime error.

### analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0)
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only pins on PORTD/PORTE (AVR Dx-series), or the constants `ADC_GROUND` or `ADC_DAC0` can be used as the negative input. Information on available negative pins for the Ex-series is not yet available, but is expected to be a subset of available analog pins. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure. Be careful when measuring very small values here, this is a "real" ADC not an "ideal" one, so there is a non-zero error, and through oversampling and/or gain, you can magnify that such that it looks like a signal.

On the Dx-series, the measured voltages must be less than VRef; this makes differential ADC measurements on these parts significantly less powerful than the Ex-series, where the input can extend as much as 100mV beyond the supply rails, permitting use to measure current through a high-side sense resistor against the 1.024 V reference.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

The 32-bit value returned should be between -32768 and 32767 (inclusive). Large negative indicate a runtime error.

**ERRATA ALERT** There is a mildly annoying silicon bug in early revisions of the AVR DA parts (as of a year post-release in 2021, these are still the only ones available) where whatever pin the ADC positive multiplexer is pointed at, digital reads are disabled. This core works around it by always setting the the ADC multiplexer to point at ADC_GROUND when it is not actively in use; however, be aware that you cannot, say, set an interrupt on a pin being subject to analogReads (not that this is particularly useful).

### analogClockSpeed(int16_t frequency = 0, uint8_t options = 0)
The accepted options for frequency are -1 (reset ADC clock to core default, 1-1.35 MHz), 0 (make no changes - just report current frequency) or a frequency, in kHz, to set the ADC clock to. Values between 125 and 2000 are considered valid for Dx-series parts and Ex-series parts 300-3000 with internal reference, and 300-6000 with Vdd or external reference. The prescaler options are discrete, not continuous, so there are a limited number of possible settings (the fastest and slowest of which are often outside the rated operating range). The core will choose the highest frequency which is within spec, and which does not exceed the value you requested. If a 1 is passed as the third argument, the validity check will be bypassed; this allows you to operate the ADC out of spec if you really want to, which may have unpredictable results. Microchiop documentation has provided little in the way of guidance on selecting this (or other ADC parameters) other than giving us the upper and lower bounds.

**Regardless of what you pass it, it will return the frequency in kHz**

The Dx-series has prescalers in every power of two from 2 to 256, and at the extreme ends, typical operating frequencies will result in an ADC clock that is not in spec.

The Ex-series is expected to have prescalers similar to the tinyAVR 2-series: Every even number up to 16, then 20, 24, 28, 32, 40, 48, 56, and 64, giving considerably more precision in this adjustment.

In either case you don't need to know the range of prescalers, just ask for a frequency and we'll get as close as possible without exceeding it, and tell you what was picked

```c++
Serial.println(analogClockSpeed()); // prints something between 1000 and 1500 depending on F_CPU
analogClockSpeed(300); // set to 300 kHz
Serial.println(analogClockSpeed()); // prints a number near 300 - the closest to 300 that was possible without going over.
// For Dx-series
Serial.println(analogClockSpeed(3000)); // sets prescaler such that frequency of ADC clock is as close to but not more than  2000 (kHz)
// as possible which is the maximum supported according to the datasheet. Any number of 2000 or higher will get same results.
Serial.println(analogClockSpeed(20)); // A ridiculously slow ADC clock request. Datasheet says minimum is 125. Maximum prescaler is 256, so this would
                                      // set the ADC clock to that for the lowest ADC clock possible with this clock speed and return that (93 kHz (93.75 truncated to int)).

// For Ex-series
Serial.println(analogClockSpeed(20000)); // Above manufacurer max spec, so seeks out a value that is no larger than that spec 3000 if internal reference selected or 6000 otherwise.
Serial.println(analogClockSpeed(20000,1)); // Above manufacturer's spec. but we instruct it to ignore the spec and live dangerously.
// The requested speed, far in excess of the maximum obtainable (which is several times the maximum supported) speed, with limits bypassed,
// will lead it to choose the fastest possible ADC clock, which is only prescaled by a factor of 2, and return that value, likely 8000 or 10000
// unless you also like exceeding manufacturer system clock limiits and are just overclocking everything.
// Either way
int returned_default = analogClockSpeed(-1); // reset to default value, around 1000 - 1500 kHz.
Serial.println(returned_default);  // will print the same as the first line, assuming you hadn't changed it prior to these lines.
```
If anyone undertakes a study to determine the impact of different ADC clock frequency on accuracy, take care to adjust the sampling time to hold that constant. I would love to hear of any results; I imagine that lower clock speeds should be more accurate, but within the supported frequency range, I don't know whether these differences are worth caring about.

### getAnalogReadResolution()
Returns either 10 or 12, the current resolution set for analogRead.

### getAnalogSampleDuration()
Returns the number of ADC clocks by which the minimum 2 clock sample length has been extended.

## ADC Runtime errors
When taking an analog reading, you may receive a value near -2.1 billion - these are runtime error codes.
The busy and disabled errors are the only ones that we never know at compile time.
| Error name                     |     Value   | Notes
|--------------------------------|-------------|---------------------------------------------------------------------
|ADC_ERROR_INVALID_CLOCK         |      -32764 | Returned by analogSetClock() wt
|ADC_ERROR_BAD_PIN_OR_CHANNEL    |      -32765 | The specified pin or ADC channel does not exist or does support analog reads.
|ADC_ERROR_BUSY                  |      -32766 | The ADC is busy with another conversion
|ADC_ERROR_DISABLED              |      -32767 | The ADC is disabled at this time.
|ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL| -2100000000 | ADC input (or positive input) is neither a valid pin nor channel
|ADC_ENH_ERROR_BUSY              | -2100000001 | The ADC is busy with another conversion.
|ADC_ENH_ERROR_RES_TOO_LOW       | -2100000003 | Minimum ADC resolution is 8 bits. If you really want less, you can always rightshit it.
|ADC_ENH_ERROR_RES_TOO_HIGH      | -2100000004 | Maximum resolution, using automatic oversampling and decimation is 15, and will be 17 on Ex-series
|ADC_DIFF_ERROR_BAD_NEG_PIN      | -2100000005 | analogReadDiff() was called with a negative input that is not valid.
|ADC_ENH_ERROR_DISABLED          | -2100000007 | The ADC is currently disabled. You must enable it to take measurements.


## DAC Support
The Dx-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference or control voltage, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DACReference() function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). Call `analogWrite()` on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call `digitalWrite()` or `turnOffPWM()` on that pin. Regardless, will want to set a DAC reference voltage with `DACReference()` - same options and syntax as `analogReference()`. The default is the 1.024V reference, which isn't what you probably want!

To use it in 10-bit mode
```
//assumes dacvalue is an unsigned 16-bit integer containing a number between 0 and 1023.

// enable DAC
DAC0.CTRLA |= (DAC_OUTEN_bm | DAC_ENABLE_bm);

// write value to DAC
DAC0.DATA=(dacvalue<<6);

// disable DAC
DAC0.CTRLA &= ~(DAC_OUTEN_bm | DAC_ENABLE_bm);
```

### DAC Error
For rather silly reasons, I wound up taking a bunch of measurements with a millivolt meter hooked up to the DAC. At least on the chip I tested, at room temperature and Vcc = approx. 5v, both the 4.096 and 1.024V references were within 1%, and the voltages coming out of the DAC were a few mV low below around the half-way point. It looked to me like, feeding it 8-bit values, if your "dac writing" function secretly set the low 2-bits of DATA a value dependent on the 8 high-bits and the current reference voltage, you could get a significant improvement in accuracy of the output. Something like this would do well - at least on the chip I was looking at. I have too many other tasks to pursue further - but someone who likes analog stuff could have a lot of fun with this, for certain values of fun...
```c++
/* Note - this is not part of the core, but an example of how you might uswe the core. */
void correctedDACWrite(uint8_t value) {
  // assumes DAC already enabled and outputting, just changing output value
  // elsewhere, pretend that it's not doing any of this.
  byte datal=0;
  byte dacref = VREFA.DAC0REF &0x07;
  if (dacref == 2) { // 4.096
    if (value < 128) {
      datal=1;
    }
  } else if (dacref == 2)  {// 1.024
    if (value < 112) {
      datal = min(value,3);
    } else if (value < 128) {
      datal = 2;
    } else if (value < 144) {
      datal = 1;
    }
  } else if (dacref = 1) { // 2.048
    /* I didn't  play with this reference voltage... */
  } else if (dacref = 3) { // 2.500
    /* or this one */
  }
  DAC0.DATAL=(datal << 6); // needs to be left-adjusted,
  DAC0.DATAH=value;
}

```

## Opamps
The DB-series may not have the true PGA that other the EA will have - but they do have 2 or 3 on-chip opamops, which can be used to create various sorts of amplifiers to the benefit of the ADC, in addition to many other uses.

See the documentation for the [Opamp Library](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Opamp) by MCUDude.
