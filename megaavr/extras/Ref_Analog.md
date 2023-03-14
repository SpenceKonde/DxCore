# ADC/DAC Reference for DxCore
The Dx-series parts have a powerful ADC, with higher resolution, automatic accumulation, maximally convenient reference voltages and more. The one that the EA-series will have is even better.

References to the Ex-series were based on the assumption that it is the same ADC that the tinyAVR 2-series got. We find that this was a pretty good assumption): The ADC is almost identical, though it's specs have gotten a small across the board improvement, and

## Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:

 | AVR Dx/Ex-series (all)                  | Voltage | Minimum Vdd | Number |
 |-----------------------------------------|---------|-------------|--------|
 | `VDD` (default)                         | Vcc/Vdd |             |      5 |
 | `INTERNAL1V024`                         | 1.024 V |      2.5* V |      0 |
 | `INTERNAL2V048`                         | 2.048 V |      2.5  V |      1 |
 | `INTERNAL4V096`                         | 4.096 V |      4.55 V |      2 |
 | `INTERNAL2V500`                         | 2.500 V |      2.7  V |      3 |
 | `INTERNAL4V1` (alias of INTERNAL4V096)  | 4.096 V |      4.55 V |      2 |
 | `EXTERNAL`                              | VREF pin|        VREF |      6 |

 You can test like `if(getAnalogReference()==INTERNAL2V500)`, but if you try to say, print them, you just get a number. That's what is shown in the last columncolumn contains the numerical value of the constants representing these references. Don't use those, then nobody will understand your code - including yourself in two weeks. However, if you are printing the results of `getAnalogReference()` or `getDACReference()`, these are the numbers you will see.

 External reference voltage should be between 1.8v and Vdd. `INTERNAL` is not a supported reference, since there is no obvious reason that any of the four reference options should be used. These reference voltages are presented as they are in the datasheet, but do note that they are what the manufacturer targeted, not what the actual voltage is: The spec is +/- 4% over the temperature range of -40 to 85 C, when Vdd is at least the voltage specified above - though at typical conditions, every time I've checked, it was within 1%; The numbers they give for the voltages are specified to three decimal places (obviously that is convenient because then 1 mV = 1, 2, or 4 LSB), that should by no means be interpreted as a claim that they are *that* accurate. It is likely that the 1.024 V reference is usable below 2.5 V; the latest update to the datasheet for the DB-series suggests that the ADC clock speed should be kept below . Reference voltages exceeding the supply voltage will produce meaningless results and should not be used.

## Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin number:
| DA-series               | DB and DD-series          | EA-series (speculative) |
|-------------------------|---------------------------|-------------------------|
| `ADC_GROUND`            | `ADC_VDDDIV10`            | `ADC_VDDDIV10`          |
| `ADC_TEMPERATURE`       | `ADC_VDDIO2DIV10`         | `ADC_VDDIO2DIV10`       |
| `ADC_DAC0`              | `ADC_GROUND`              | `ADC_GROUND`            |
| `ADC_DACREF0`           | `ADC_TEMPERATURE`         | `ADC_TEMPERATURE`       |
| `ADC_DACREF1`           | `ADC_DAC0`                | `ADC_DAC0`              |
| `ADC_DACREF2`           | `ADC_DACREF0`             | `ADC_DACREF0`           |
|                         | `ADC_DACREF1`             | `ADC_DACREF1`           |
|                         | `ADC_DACREF2`             |                         |

The Ground internal sources are presumable meant to help correct for offset error. On Classic AVRs they made a point of talking about offset cal for differential channels, and often all the channels could be measured

DACREF0-2 are the the reference voltages for the analog comparators. On the DA-series, there is no way to measure the supply voltage other than using DAC or DACREF source: you can neither directly measure a reference voltage like some parts, nor is there any way to get a fraction of the supply voltage like the DB and DD-series support.  Note also that on the DB series, you can't measure the outputs of the OPAMPs directly - you must output to the pin and measure that, however much the high-level descriptions sound like there is a way to route the opamp signals internally.

## Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered using the accumulation feature allowing up to 15 bits of resolution (17 on future Ex-series); this is exposed through the `analogReadEnh()` function detailed below.

## A warning about the DAC (well, 2)
First, as described in the errata for the DA and DB, if the output buffer is not enabled, it will suffer accuracy drift over the life of the device. This is rarely an issue since almost all applications of the DAC require the buffer anyway. Just don't forget and leave it on doing nothing for a month or something.

Secondly, the DAC can source a minuscule amount of current. 1mA or less. Compared to what it can sink, however, that current looks genererous. It can sink a mere 1 *micro*amp or so. To get good performance with a falling DAC signal, if the load has much of any capacitance, you will need a small pulldown resistor (of course, you want this pulldown to be very weak so as not to throw off the output value, the ability to source current is also poor. This is not necessary if you are using a DB with one of the OPAMPs in voltage follower mode, which we highly recommend for all cases where you need to generate analog voltages for more that a near-zero-current reference.

## ADC Sampling Speed
DxCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs. The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 2 MHz (up to 3 MHz for 2-series w tiny or EA with internal ref, twice that on external or Vdd reference at full resolution. We use use 1.0 to 1.5 MHz on Dx, and will target 2-2.5 by default on EA unless someone provides information that suggests I shouldn't. I tried to get answers out of Microchip, but I think their employees have been instructed to never give quantitative advice on ADC settings, since one has to imagine that (especially for 2-series tinyAVRs, with so many dials to turn), "So I have a signal of such-and-such impedance, and I'm running at this speed, what settings should I run the ADC at to avoid inaccurate readings?" is not an uncommon question.

If anyone reading this has performed any testing of this, I would appreciate it if you could share any findings with me, to help inform the community.

 To compensate for the faster ADC clock, we extend the sampling period so it ends up with a similar sampling period to classic AVRs, while still being significantly faster by virtue of the much shorter conversion time.

## ADC Function Reference
This core includes the following ADC-related functions. Out of the box, analogRead() is intended to be directly compatible with the standard Arduino implementation. Additional functions are provided to use the advanced functionality of these parts and further tune the ADC to your application.

### uint8_t getAnalogReference() and getDACReference()
These return the numbers listes in the reference table at the top as a `uint8_t` - you can for example test `if (getAnalogReference() == INTERNAL1V1)`

### int16_t analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10 for compatibility. Negative return values indicate an error that we were not able to detect at compile time (as always, when we can detect a problem at compile time, we try to generate an error then. Return type is a 16-bit signed integer (`int` or `int16_t`).

### bool analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. *If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits*. Note that this can only happen when the value passed to it is determined at runtime - if you are passing a compile-time known constant which is invalid, we will issue a compile error. The only valid values are those that are supported natively by the hardware, plus 10 bit, even if not natively supported, for compatibility.
Hence, the only valid values are 10 and 12, plus 8 on the EA-series.

This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resources of an 8-bit AVR with this is a silly waste of resources, and padding a reading with zeros so it matches the format of a more precise measuring method is in questionable territory in terms of best practices. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution while keep the extra bits meaningful at the cost of slowing down the reading. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument, and restore the previous setting.

### bool analogSampleDuration(duration)
Sets sampling duration (SAMPLEN), the sampling time will be 2 + SAMPLEN ADC clock cycles. Sampling time is the time the sample and hold capacitor is connected to the source before beginning conversion. For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length.
speed. This returns a `bool` - it is `true` if value is valid.

This value is used for all analog measurement functions.

| Part Series | Sample time<br/>(default)  | Conversion time | Total analogRead() time | Default SAMPLEN | ADC clock sample time |
|-------------|--------------|-----------------|-------------------------|-----------------|-----------------------|
| Classic AVR |        12 us |           92 us |                  104 us | No such feature | 1.5                   |
| 0/1-series  |        12 us |       8.8-10 us |              20.8-22 us |     7, 9, or 13 | 2 + SAMPLEN           |
| Dx (10-bit) |   apx. 12 us |  approx.  10 us | Approximately  21-22 us |              14 | 2 + SAMPLEN           |
| Dx (12-bit) |   apx. 12 us |  approx.  12 us | Approximately  23-24 us |              14 | 2 + SAMPLEN           |
| 2-series    |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |
| EA-series   |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |

Note: The default samplen given above is the default that the core sets it to when the ADC is initialized. The hardware defaults to 0, giving a much shorter sampling time. Also, as shown above while it's called SAMPLEN on parts with that don't have the PGA, with the new ADC and accompanying register name reshuffle, that became SAMPDUR. SAMPLEN was the sole bitfienld in the SAMPCTRL on Dx, while SAMPDUR is the sole bitfield on the clearly named CTRLE register. I'm not sure how either of those differences made it through. If I were in the room at the time, I'd have let 'em have it, because you should never rename a register without good reason, and the renamed register should never be objectively worse than the previous one. In this case, there is no conceivable good reason, and the new register name (CTRLE I mean) is obviously worse, since it gives no indication of it's function, nor is it even located immediately after CTRLD.

The DxCore default is 14, which will result in a 16 ADC clock sampling time. For most signals this is more than needed, and the Microchip default is 0, giving 2, while documentation indicates that the ADC is optimized for signals with an impedance of around  ~10k~ 1k (This was are recent "datasheet clarification", which came at the same time as the "clarification" about the flash endurance - "'10,000', '1,000' - they're basically the same right?"). Reducing it to the minimum will approximately cut the time analogRead() takes in half. It will result in less accurate readings of high impedance signals, though it's not clear how high they can be before this becomes a concern.

On the 2-series, we are at least given some numbers that we can use to model it: 8pF for the sample and hold cap, and 10k input resistance so a 10k source impedance (total 20k impedance between the sample and hold cap and that which we sought to measur) would give 0.16us time constant, implying that even a 4 ADC clock sampling time is excessive, but at such clock speeds, impedance much above that would need a longer sampling period. We are obviously erring on the conservative side here - if ADC speed matters to you, you are encouraged to perform testing with the actual hardware you will be using, which may reveal that you can lower it significantly. Remember that it it also depends on ADC clock speed, and have that set the same during your testing as it will use in practice.

### int32_t analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 15 bits for Dx-series parts, and 17 for Ex-series. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res` native resolution) is selected. Note that maximum sample burst reads are not instantaneous, and in the most extreme cases can take milliseconds. Depending on the nature of the signal - or the realtime demands of your application - the time required for all those samples may limit the resolution that is acceptable. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See [Atmel app note AVR121](https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf) - the specific case of the new ADC on the Ex and tinyAVR 2-series is discussed in the newer DS40002200 from Microchip, but that is a rather vapid document). Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 128 (Dx-series), or up to 1024 (Ex-series). The Dx-series only makes available the 16 MSBs, so when accumulating more than 16 samples, the value will be truncated to 16 bits. Be aware that the lowest bits of a raw accumulated reading should not be trusted.; they're noise, not data (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On Ex-series parts *the PGA can be used for single ended measurements*. Valid options for gain on the Ex-series are 0 (PGA disabled, default), 1 (unity gain - may be appropriate under some circumstances, though I don't know what those are - possibly particularlty high impedance sources), or powers of 2 up to 16 (for 2x to 16x gain). On Dx-series parts, the gain argument should be omitted or 0, and passing any other value will generate an error message stating this: these do not have a PGA.

```c++
  int32_t adc_reading = analogReadEnh(PIN_PD2, 13); //be sure to choose a pin with ADC support. The Dx-series and tinyAVR have ADC on very different sets of pins.
  // Measure voltage on PD2, to 13 bits of resolution.
  // AVR Dx, Ex, and tinyAVR 2-series this will be sampled 4 times using the accumulation function, and then rightshifted once
  // tinyAVR 0/1-series, this will be sampled 64 times, as we need 3 more bits, hence we need to take 2^(3*2) = 64 samples then rightshift them 3 times.,
  int32_t adc_reading2 = analogReadEnh(PIN_PD2, ADC_ACC128);
  // Take 128 samples and accumulate them. This value, k, is 19 bits wide; on the Dx-series parts, this is truncated to 16 bits - the hardware does not expose the three LSBs.
```

Negative values from ADC_ENH always indicate a runtime error; these values are easily recognized, as they are huge negative numbers

### int32_t analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0)
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only pins on PORTD/PORTE (AVR Dx-series), or the constants `ADC_GROUND` or `ADC_DAC0` can be used as the negative input. Information on available negative pins for the Ex-series is not yet available, but is expected to be a subset of available analog pins. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure. Be careful when measuring very small values here, this is a "real" ADC not an "ideal" one, so there is a non-zero error, and through oversampling and/or gain, you can magnify that such that it looks like a signal.

On the Dx-series, the measured voltages must be less than VRef; this makes differential ADC measurements on these parts significantly less powerful than the Ex-series, where the input can extend as much as 100mV beyond the supply rails, permitting use to measure current through a high-side sense resistor against the 1.024 V reference.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

The 32-bit value returned should be between -65536 and 65535 at the extremes with the maximum 17-bit accumulation option, or, 32-times that if using raw accumulated values (-2.1 million to 2.1 million, approximately).

This, again, is easy to distinguish from an error code, as the error codes are close to -2.1 **b**illion.

**ERRATA ALERT** There is a mildly annoying silicon bug in early revisions of the AVR DA parts (as of a year post-release in 2021, these are still the only ones available) where whatever pin the ADC positive multiplexer is pointed at, digital reads are disabled. This core works around it by always setting the the ADC multiplexer to point at ADC_GROUND when it is not actively in use; however, be aware that you cannot, say, set an interrupt on a pin being subject to continuous analogReads and expect it to work correctly (not that this is particularly useful).

### uint16_t analogClockSpeed(int16_t frequency = 0, uint8_t options = 0)
The accepted options for frequency are -1 (reset ADC clock to core default, 1-1.35 MHz), 0 (make no changes - just report current frequency) or a frequency, in kHz, to set the ADC clock to. Values between 125 and 2000 are considered valid for Dx-series parts and Ex-series parts 300-3000 with internal reference, and 300-6000 with Vdd or external reference. The prescaler options are discrete, not continuous, so there are a limited number of possible settings (the fastest and slowest of which are often outside the rated operating range). The core will choose the highest frequency which is within spec, and which does not exceed the value you requested. If a 1 is passed as the third argument, the validity check will be bypassed; this allows you to operate the ADC out of spec if you really want to, which may have unpredictable results. Microchip documentation has provided little in the way of guidance on selecting this (or other ADC parameters) other than giving us the upper and lower bounds.

**Regardless of what you pass it, it will return the frequency in kHz** as a `uint16_t`. (ie, a value of 1000 indicates 1000 kHz/1 MHz/1,000,000 Hz)

The Dx-series has prescalers in every power of two from 2 to 256, and at the extreme ends, unless operating at unusual frequencies, the resulting ADC clock will be out of spec.

The Ex-series is expected to have prescalers similar to the tinyAVR 2-series: Every even number up to 16, then 20, 24, 28, 32, 40, 48, 56, and 64, giving considerably more precision in this adjustment.

In either case you don't need to know the range of prescalers - just ask for the desired frequency. We'll get as close as possible to that number without exceeding it or going outside the chip specifications, and tell you what that number we picked was. If options & 0x01 is non-zero, we will skip checking whether the frequency in question is within spec, just in case you for some reason want inaccurate results from the ADC, or the chip's clock speed has been changed (which of course breaks timekeeping too - but you may not care, or may also be compensating for that.

```c++
// ******* for both *******
Serial.println(analogClockSpeed());     // prints something between 1000 and 1500 depending on F_CPU
analogClockSpeed(300);                  // set to 300 kHz
Serial.println(analogClockSpeed());     // prints a number near 300 - the closest to 300 that was possible without going over.

// ****** For Dx-series *******
Serial.println(analogClockSpeed(3000)); // sets prescaler such that frequency of ADC clock is as
// close to but not more than 2000 (kHz) which is the maximum supported according to the datasheet.
// Any number of 2000 or higher will get same results.
Serial.println(analogClockSpeed(20));   // A ridiculously slow ADC clock request.
// Datasheet says minimum is 125, maximum prescale(256) would give 93.75 kHz - but that's not
// within spec, and no second argument was passed; we will instead set to 128 prescaler
// for clock speed of 187.5 kHz and will return 187,
Serial.println(analogClockSpeed(20, 1)); // As above, but with the override of spec-check
// enabled, so it will set it as close to 20 as it can (93.75) and return 93.

// ****** For Ex-series *******
Serial.println(analogClockSpeed(20000));   // Above manufacurer max spec, so seeks out a value that
// is no larger than that spec; 3000 if internal reference selected or 6000 otherwise.
Serial.println(analogClockSpeed(20000,1)); // Above manufacturer's spec. but we instruct
// it to ignore the spec and live dangerously. The requested speed, far in excess of the
// maximum obtainable (which is several times the maximum supported) speed, with limits bypassed,
// will lead it to choose the fastest possible ADC clock, which is only prescaled by a factor of 2,
// and return that value, likely 8000 or 10000 unless for 16 or 20 MHz. (higher if you're overclocking
// the chip in general too). One version that was released shortly after the 2-series contained a bug that would always set the
// ADC prescaler to the minimum. And I'd written about how disappointed I was in the new ADC. After fixing the bug, things improved significantly.
// I am favorably impressed by how well the poor overclocked ADC performed, despite being overclocked

// ******* For both *******
int returned_default = analogClockSpeed(-1); // reset to default value, around 1000 - 1500 kHz for Dx and around 2500 for Ex.

Serial.println(returned_default);  // will print the same as the first line, assuming you hadn't changed it prior to these lines.
```
If anyone undertakes a study to determine the impact of different ADC clock frequency on accuracy, take care to adjust the sampling time to hold that constant. I would love to hear of any results; I imagine that lower clock speeds should be more accurate, but within the supported frequency range, I don't know whether these differences are worth caring about.
I've been told that application notes with some guidance on how to best configure the ADC for different jobs is coming - though it seems like what they've actually done is to just remove some of the knobs from the EA-series. In any event, we're down to one mysterious switch (marked "Sign Chopping" - and I think I've worked out what it does, and the two least mysterious of the knobs on the EA (clock speed and sampling time).

### uint8_t getAnalogReadResolution()
Returns either 8, 10 or 12, the current resolution set for analogRead. no part has more than 2 options here - between the Dx (10, 12), Ex (unknown, likely 8 and 12), and tinyAVR parts (8 and 10 or 8 and 12), all of the combinations are possible.

### uint8_t getAnalogSampleDuration()
Returns the number of ADC clocks by which the minimum sample length has been extended.

### uint8_t ADCPowerOptions(options)
*Planned for when the AVR EA-series is added. For compatibility, a much more limited version will be provided for the Dx-series parts*
The PGA requires power when turned on. It is enabled by any call to `analogReadEnh()` or `analogReadDiff()` that specifies valid gain > 0; if it is not already on, this will slow down the reading. By default we turn it on so people can just off afterwards. There is also a "low latency" mode that, when enabled, keeps the ADC reference and related hardware running to prevent the delay (on order of tens of microseconds) before the next analog reading is taken. We use that by default, but it can be turned off with this function.
Generate the argument for this by using one of the following constants, or bitwise-or'ing together a low latency option and a PGA option. If only one option is supplied, the other configuration will not be changed.
This is currently implemented on megaTinyCore, as the 2-series has the same ADC that the EA-series will be getting (possibly with a few changes, but nothing major - nothing like the difference between the tinyAVR 1-series and the AVR Dx parts.
* `LOW_LAT_OFF`     Turn off low latency mode. *EA-series only*
* `LOW_LAT_ON`      Turn on low latency mode. *EA-series only*
* `SIGN_CHOP_ON`    Turn on sign chopping *EA-series in accumulated mode only*
* `SIGN_CHOP_OFF`   Turn off sign chopping *EA-series in accumulated mode only*
* `PGA_OFF_ONCE`    Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change. *EA-series only*
* `PGA_KEEP_ON`     Enable PGA. Disable the automatic shutoff of the PGA. *EA-series only*
* `PGA_AUTO_OFF`    Disable PGA now, and in future turn if off after use. *EA-series only*
* `ADC_ENABLE`      Enable the ADC if it is currently disabled.
* `ADC_DISABLE`     Disable the ADC to save power in sleep modes.
* `ADC_STANDBY_ON`  Turn on ADC run standby mode
* `ADC_STANDBY_OFF` Turn off ADC run standby mode

Example:
```c++
ADCPowerOptions(LOW_LAT_ON  | PGA_KEEP_ON );            //  low latency on. Turn the PGA on, and do not automatically shut it off. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(LOW_LAT_OFF | PGA_AUTO_OFF);            //  low latency off. Turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays. **ERRATA WARNING** turning off LOWLAT can cause problems on 2=series parts! See the errata for the specific part you are using.)
ADCPowerOptions(ADC_DISABLE);                           //  turn off the ADC.
ADCPowerOptions(ADC_ENABLE);                            //  Turn the ADC back on. If LOWLAT mode was on, when you turned off the ADC it will still be on,. Same with the other options.
```

### ADC Runtime errors
When taking an analog reading, you may receive a value near -2.1 billion, or a negative value on a part without a differential ADC or close to -32000 from analogRead() - these are runtime error codes.
The busy and disabled errors are the only ones that we never know at compile time (though the others can come up when the pin numbers are not known at compile time. analogCheckError is supplied by the DxCore.h library, as it does not require tight core integration to work. Note that the numeric values, though not the names, of some of these were changed to make the error checking more efficient. As long as you used the named constants like you're supposed to you'll be fine. The values returned by checkAnalogError will not change in future releases. We make no guarantees about the values of the error constants themselves, though no changes are expected.

| Error name                      |     Value   | analogCheckError val | Notes
|---------------------------------|-------------|---------------------------------------------------------------------
| ADC_ERROR_BAD_PIN_OR_CHANNEL    |      -32001 |                   -1 | The specified pin or ADC channel does not exist or does not support analog reads.
| ADC_ERROR_BUSY                  |      -32002 |                   -2 | The ADC is busy with another conversion.
| ADC_ERROR_DISABLED              |      -32007 |                   -7 | The ADC is disabled at this time. Did you disable it before going to sleep and not re-enable it?
| ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL| -2100000001 |                   -1 | The specified pin or ADC channel does not exist or does not support analog reads.
| ADC_ENH_ERROR_BUSY              | -2100000002 |                   -2 | The ADC is busy with another conversion.
| ADC_ENH_ERROR_RES_TOO_LOW       | -2100000003 |                   -3 | Minimum ADC resolution is 8 bits. If you really want less, you can always rightshift it.
| ADC_ENH_ERROR_RES_TOO_HIGH      | -2100000004 |                   -4 | Maximum resolution using automatic oversampling and decimation is less than the requested resolution.
| ADC_DIFF_ERROR_BAD_NEG_PIN      | -2100000005 |                   -5 | analogReadDiff() was called with a negative input that is not valid.
| ADC_ENH_ERROR_DISABLED          | -2100000007 |                   -7 | The ADC is currently disabled. You must enable it to take measurements. Did you disable it before going to sleep and not re-enable it?
| ADC_IMPOSSIBLE_VALUE            |   See Notes |                 -127 | 16-bit (analogRead()) value > 4095 (on classic AVRs this could also mean < 2048), or 32-bit value that's not an error<br/> code and is outside the range of -2,097,152 to 4,193,280 accumulation range. Should be impossible. <br/> If you sum N values each no larger than Y, and the result is larger than N * Y, something is seriously busted.
| Potentially valid reading       |      Result |                    0 | If there is some combinations of settings that could get this value without an error condition it returns 0.

The impossible values are checked for without testing all of the bytes for greater efficiency. If you see that result one of two things was the case: the value you passed in wasn't from analog read or had been cast to a different type before you passed it, or i, or was corrupted somehow (writing off end of adjacent array in memory? Overclocking too hard such that the chip was doing math wrong?)

### General specifications
These are based on the latest datasheet clarifications available, and often differ from the values in the initial datasheets All of the error values are based on the highest native ADC resolution.


| Part Family | Recommended input impedance |   Res | Cs+h  |   Rin | Offset error  | Gain Error      |     INL |     DNL | Ext. Ref |
|-------------|-----------------------------|-------|-------|-------|---------------|-----------------|---------|---------|----------|
| AVR DA      |                ??? ~10k~ 1k | 10/12 |  N/S  |   10k |  0-5, typ 2.5 |   +/-5. typ 1.5 | +/- 1.8 | +/- 1.0 |  1v8-VDD |
| AVR DB      |                ??? ~10k~ 1k | 10/12 |  N/S  |   10k |  0-5, typ 2.5 |   +/-5. typ 1.5 | +/- 1.8 | +/- 1.0 | 1V8-VDD* |
| AVR DD      |                ???       1k | 10/12 |  7 pf |   10k |  0-5, typ 2.5 |   +/-5. typ 1.5 |     TBD |     TBD |  1V8-VDD |
| Tiny 0/1 int|                 Unspecified |  8/10 | 7-8pf |   N/S |         +/-10 |  +/-25-60 typ 3 | +/- 1.5 | +/- 1.0 |      n/a |
| Tiny 1 ext  |                 Unspecified |  8/10 | 7-8pf |   N/S |         +/-10 |            +/-1 | +/- 1.5 | +/- 1.0 |  1V1-VDD |
| Tiny 2 ext  |                 Unspecified |  8/12 | 7-8pf |   10k |         +/-10 |            +/-1 | +/- 1.5 | +/- 1.0 |  1V0-VDD |
| AVR EA      |                         TBA |  8/12 |2.5/7pf|3/2.5k |       typ 1.5 |         typ 0.6 | typ 0.8 | typ 0.8 |  1V0-VDD |
| AVR EB      |                         TBA |   TBA |  TBA  |   TBA |           TBA |             TBA |     TBA |     TBA |      TBA |

* Latest DB-series daasheet has updated the ADC characteristics, and clarified that if the ADC clock is below 500 kHz, 1.024V ADC reference was acceptable. What's a little unclear is whether they are implying that the internal 1.024V reference should be used only when the ADC clock is that speed. However, while they indicated they had incorporated the changes from the datasheet clarifications, the recommended input impedance is still 10k, while the earlier version of the datasheet and the pre-datasheet-revision datasheet clarifications suggested 10k.

Notes: Yeah, those gain and offset error figures for the tiny 0/1-series with the internal ref. at the extremes are insanely huge. Likely the reason is that thermal or voltage effects are throwing the bandgap reference way off under the more extreme opperating conditions. Notice that the gain error plummets if an external reference is used - the problem is clearly the reference voltages, not the ADC itself. I have never seen results nearly that dreadful.

Where two values separated with a / are shown for Res and Cs+h, those are without and with the PGA enabled.

Watch out on parts with a PGA. The PGA will ampify that offset error, to such a degree that when I was swapping the positive and negative pin of two pins held at constant voltages, and repeatedly doing differential ratios with high gain, I thought my code was bad because like,

### Functions in DxCore.h
These functions are located in DxCore.h - they do not require tight core integration to work,.
#### printADCRuntimeError(uint32_t error, &HardwareSerial DebugSerial)
Pass one of the above runtime errors and the name of a serial port to get a human-readable error message. The serial port must have already been configured with the `begin()` method. This is a rather bloated function; once the analog stuff is working, you can probably lose this this function. Also, *you must not cast result to a different type before calling this. The logic is determined by whether it is passed a signed 32-bit value (from analogReadEnh or analogReadDiff) or a signed 16-bit one (from analogRead())*
```c++
int32_t adc_reading=AnalogReadDiff(PIN_PA1, PIN_PA2);
if (analogCheckError) { // an error occurred! Print a human readable value
  printADCRuntimeError(adc_reading, Serial);
}
```
#### analogCheckError(value)
Pass either the int16_t from analogRead or the int32_t from analogReadEnh to this to check if it's a valid value. If this returns a non-zero value, that means that you got an error, and should be printing debugging information, not trying to make use of it.
*you must not cast result to a different type before calling this - the logic is determined by whether it is passed a signed 32-bit value (from analogReadEnh or analogReadDiff) or a signed 16-bit one (from analogRead())*
```c++
// Right:
int32_t adcreading=analogReadEnh(PIN_PA1,12);
if (analogCheckError(adcreading)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading);
}
// Right:
int16_t adcreading2=analogRead(PIN_PA1);
if (analogCheckError(adcreading2)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading2);
}
// Wrong:
int32_t adcreading=analogRead(PIN_PA1); // analogRead() returned an int16_t, but if we assign it to a 32-bit variable it's going to think it came from analogReadEnh and will not correctly identify errors.
if (analogCheckError(adcreading)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading);
}
```

#### int16_t millivoltsToADCVals(int16_t mv, const int16_t ref, const uint8_t bits) *Not yet implemented*
Converts a voltage in millivolts into the approximate expected ADC counts (no accounting for error is made). See the [documentation for the DxCore Library](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore)

## Opamps
The DB-series may not have the true PGA that other parts like the the EA have - but they do have 2 or 3 on-chip opamops, which can be used to create various sorts of amplifiers to the benefit of the ADC, in addition to many other uses.

See the documentation for the [Opamp Library](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Opamp), written by MCUDude, and see also the Microchip Technical Briefs. The quality of the later ones is significantly improved from the initial ones.


## Analog *channel* identifiers
The ADC is configured in terms of channel numbers, not pin numbers. analogRead() hence converts the number of a pin with an analog channel associated with it to the number of that analog channel, so there is no need to deal with the analog channel numbers. The one exception to that is in the case of the non-pin inputs, the constants like ADC_DAC and ADC_VDDDIV10. I have a simple system to internally signal when a number isn';'t an digital pin number, but is instead an analog channel number: Simply set the high bit. I refer to these as analog channel identifiers. When the high bit is masked off, these are the value that you must set the MUX to in order to use this input source. No AVR has ever had more than 127 pins, much less that many analog channels, so this shouldn't be an issue. With 254 valid values, the current design provides room for 127 digital pins and 127 analog inputs, where the largest modern AVRs have only 56 I/O pins (it will be a technical challenge to surpass that, because they don't have any more registers for the VPORTs)

`ADC_CH()` will take an analog channel number (ex, "0") and turn it into the analog channel identifier. But you never need to do that unless you're getting very deep into a custom ADC library) The most common example when channels are used is when reading from things that are not pins - like the internal tap on the DAC output, or the VDDDIV10 value to find the supply voltage. These may overlap with pin numbers. Also internally, channel numbers are sometimes passed between functions. They are defined for pins that exist as analog channels, with names of the form `AINn` but **you should never use the AIN values** except in truly extraordinary conditions, and even then it's probably inappropriate. However I felt like mention of them here was needed. These macros were added at different times, and that can be seen from the inconsistent naming. This is unfortunate, but it  Some macros and helper functions involved are:

```text
digitalPinToAnalogInput(pin)    - converts an digital pin to an anbalog channel *without* the bit that says it's a channel (designed for the very last step of analogRead preparationm where we turn the pin number into the channel to set the MUX)
analogChannelToDigitalPin(p)    - converts an analog channel *number* to a digital pin.
analogInputToDigitalPin(p)      - converts an analog channel identifier to a digital pin number.
digitalOrAnalogPinToDigital(p)  - converts an analog channel identifier or digital pin to a digital pin number
ADC_CH(channel number)          - converts channel numbers to analog channel identifier

```

Try not to use these unless you're getting really deep into library development and direct interaction with the ADC; if you do end up needing them, try to just use digitalPinToAnalogInput() and analogChannelToDigitalPin(). At other times and on other cores, analog channel identifiers were more prominent.

## A word of warning to capacitive touch sensing applications
Libraries exist that use trickery and the ADC to measure capacitance, hence detect touch/proximity. Most of these libraries (since Atmel always locked up QTouch) relied on the leftover charge in the ADC S&H cap. The magnitude of this source of error is much larger on classic AVRs. It is considered undesirable (this is why when changing ADC channels in the past, you were advised to throw out the first couple of readings!) and with each generation, this has been reduced. There are still ways to do this effectively on the 2series, but require very different approaches.

## Microchip Documentation
Has been much expanded and I'm very happy to see this sort of document produced before the EA release. Wondering about the numbering? Well, TB stands for Technical Brief, which is different in some way from an Application Note (AN), and obviously a whitepaper or "Datasheet" (DS) is something else altogether. Even when they're about the same length and contain similar types of content. S
* [DS00003197A - Differential and Single-Ended ADC](https://ww1.microchip.com/downloads/en/DeviceDoc/Differential-and-Single-Ended-ADC-WhitePaper-DS00003197A.pdf)
* [TB3256 - How to Use the 12-Bit Differential ADC with PGA in Single Mode](https://www.microchip.com/content/dam/mchp/documents/MCU08/ApplicationNotes/ApplicationNotes/TB3256-How-to-Use-the-12-Bit-Differential-ADC-with-PGA-in-Single-Mode-DS90003256A.pdf)
* [AN4225 - Maximizing the Signal: Tips and Tricks to Properly Acquiring Analog Signals](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/Max-Signal-Properly-TipsTrick-Acq-Analog-Signals-DS00004225.pdf)
* [TB3254 - How to Use the 12-Bit Differential ADC with PGA in Burst Accumulation Mode
](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/TB3254-How-to-Use-the-12-Bit-Differential-ADC-Burst-Accumulation-Mode-DS90003254A.pdf)
* [TB3256 - How to Use the 12-Bit Differential ADC with PGA in Single Mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3256-How-to-Use-the-12-Bit-Differential-ADC-with-PGA-in-Single-Mode-DS90003256A.pdf)
* [TB3257 - How to Use the 12-Bit Differential ADC in Series Accumulation Mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3257-How-to-Use-the-12-Bit-Differential-ADC-in-Series-Accumulation-Mode-DS90003257A.pdf)
