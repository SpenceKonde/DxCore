# Opamp
A library for interfacing with the opamp peripheral on the AVR-DB series MCUs. These are reasonably decent opamps integrated as part of the chip - thus requiring no extra board space, and often no external components whatsoever. You can use them as a programmable gain amplifier to amplify an analog signal or differential amplifier to amplify the difference between two signals - and you would then most likely measure the output pin with the ADC. They can be used as voltage followers, and the trio together can even serve as an instrumentation amplifier. They come with a few notable limitations - for one thing, there is only one pin mapping. Certain input mux options are available only on some opamps, not all three. That it has a built-in resistor ladder is very helpful for reducing part counts, and is vital for some of the most exciting applications, but it's limited to only 8 combination of resistances, corresponding to a 15:1 ratio between the resistances (though do note that you can use the pin inputs and external resistors instead - it's just more parts and more soldering). One of Microchip's most often mentioned use cases (and understandably so) is generating the supply for the MVIO pins using the on-chip opamp. All you need to do is configure the opamp as a voltage vollower, with the DAC as the positive input, and connect the output to VDDIO2. You don't get very much current (especially near VDD), so you can't *power* much from it. But usually you're instead trying to match the lower operating voltage of something you want to talk to. This is a perfect application for it; see the below example for usage of the output_pin property, which demonstrates one way to

Developed by [MCUdude](https://github.com/MCUdude/).

The AVR DB-series parts with 48 or 64-pins have three built-in opamps, while the 28 and 32-pin versions have only two. At present, the on-chip opamps are present on the AVR DB-series parts, and no other devices featuring them have been announced within their AVR product line. It is likely that they will remain exclusive to the the top-end AVR devices as a premium feature. More useful information about the opamp peripheral can be found in the [Microchip Application Note TB3286](https://ww1.microchip.com/downloads/en/Appnotes/GettingStarted-Analog-SignCondOPAMP-DS90003286A.pdf).

See also chapter 35: OPAMP - Analog Signal Processing of the datasheet. Section 35.3.7, "Applications Usage" is particularly valuable, showing the equivalent circuit and settings required for all registers to achieve a number of common opamp use cases - voltage followers, inverting and non-inverting programmable gain amplifiers, integrators (requires an external resistor and capacitor), differential amplifiers, and using all three opamps, even an instrumentation amplifier (a differential amplifier with each input buffered by a voltage follower)

| Opamp  | + Pin | - Pin | OUT  | Comments               |
|--------|-------|-------|------|------------------------|
| Opamp0 | PD1   | PD3   | PD2  | 48/64-pin parts only.  |
| Opamp1 | PD4   | PD7   | PD5  | PD6 is the DAC output. |
| Opamp2 | PE1   | PD3   | PE2  | 48/64-pin parts only.  |


## Opamp class
The Opamp class provides a wrapper around the on-chip opamps on the AVR DB-series parts. One instance of this class is created for each of the opamps present on the chip; the user should not be declaring new objects of class Opamp. The names are `Opamp0`, `Opamp1` and (on 48 and 64-pin parts) `Opamp2`. Parts with less than 48 pins only have 2 opamps (they also lack the pins that the third opamp would use).

Note that there is both the static  `start()` method (this enables or disables opamps in general) as well as each opamp's  `init()` method, which writes all the settings you've configured using the properties to the peripheral registers (this triggers the settling time, a delay after enabling or reconfiguring an opamp before it can be used)

### Input and Output-related properties

#### Summary



#### input_p
This property controls what the positive input of the opamp is connected to.

##### Accepted values
``` c++
in_p::pin;         // Connect the inpuy to the external pin
in_p::wiper;       // Connect the input to the wiper internally
in_p::dac;         // Connect the input to the DAC internally
in_p::gnd;         // Connect the input to ground internally
in_p::vdd_div2;    // Connect the input to Vdd/2 internally, which is half the supply voltage
in_p::link_output; // Connect the input to the previous opamp's output internally (only available for Opamp1 and Opamp2)
in_p::link_wiper;  // Connect the input to the Opamp0 wiper internally (only available for Opamp2)
```

As noted above, unlike (for example) the CCL, for which one might imagine the Opamp's to be the "analog version" of, `in_p::link_output` does not wrap around; it's just not available on Opamp0, and `in_p::link_wiper` is only an option on `Opamp2` (if present).

##### Usage
``` c++
Opamp0.input_p = in_p::vdd_div2; // Connect positive opamp input to Vdd/2
```

##### Default state
`OpampN.input_p` defaults to `in_p::pin` if not specified in the user program.

#### input_p_pin
`OpampN.input_p_pin` is the pin number of the positive input pin for the opamp (PD1, PD4, or PE1 respectively). Use it as a sanity check, to build highly portable code (by passing around opamp references (`&Opamp`), where you need to know what one of the pins is, such as to read the output with  `analogRead()`), and similar situations. You can even manipulate the pin directly with  `digitalWrite()`, though because this will be either Vcc or Gnd, it is not nearly as flexible as the DAC as an input. A user wishing these had more than one DAC might be tempted to go the RC filter route. This will work - of course, you can't use the input pin itself for this (even after moving PWM around, that wouldn't change the fact that the the signal at that point still looks like digital PWM. It's only analog on the other side of the resistor. The `input_p_pin` property itself is read-only.

##### Usage
```c++
Serial.print(analogRead(Opamp1.input_p_pin));
```
#### input_n
This property controls what the negative pin of the opamp is connected to. There are fewer options than the positive pin.

##### Accepted values
``` c++
in_n::pin;    // Connect the input to the external pin
in_n::wiper;  // Connect the input to the wiper internally
in_n::output; // Connect the input to the output of the opamp internally
in_n::dac;    // Connect the input to the DAC internally
```

##### Usage
``` c++
Opamp0.input_n = in_n::wiper; // Connect negative opamp input to the center of the resistor ladder
```
##### Default state
`OpampN.input_n` defaults to `in_n::pin` if not specified in the user program.


#### input_n_pin
`OpampN.input_n_pin` is the pin number of the negative input pin for the opamp (PD3, PD7, or PE3 respectively). See `input_p_pin` above for more information.

##### Usage
```c++
Serial.print(analogRead(Opamp1.input_n_pin));
```


#### inrange
Property for controlling the opamp input range (Microchip calls this bitfield `IRSEL`)- this is set globally, not per opamp. If it is not specified for a given opamp, `init` will not set it one way or the other. In rail-to-rail mode, the common mode voltage range (the range of voltages over which the inputs can vary and generate correct output) extends 0.3v beyond the power and ground rails, while the conventional option limits the common mode voltage rang only goes up to Vdd - 0.7v

##### Accepted values
``` c++
in::rail_to_rail; // Consumes more power. Common mode voltage range covers the entire range of voltages that can be safely applied to a pin. The default.
in::conventional; // Consumes less power but common mode voltage range ends  0.7v below the positive supply rail.
```

##### Usage
``` c++
Opamp0.inrange = in::rail_to_rail; // Set rail to rail mode
```

#### output
The output property controls whether the output of the opamp is enabled or not. Opamp output pins cannot be remapped. They are PD2, PD5, and PE2.

##### Accepted values
``` c++
out::disable;      // Do not enable output (can be overridden be DRIVE event)
out::enable;       // Enable output
out::drive_event;  // Enable output only with DRIVE event (syntactic sugar for disable)
```
There are only two actual options here - `out::drive_event` is synonymous with `out::disable`. However, if you are using the DRIVE event to control opamp output, `out::drive_event` is recommended to improve readability, as it more clearly communicates what you are doing: You're not "disabling the output", you're "using the drive event to control the output".

##### Usage
``` c++
Opamp0.output = out::enable; // enable output
```

##### Default state
`OpampN.output` defaults to `out::enable` if not specified in the user program - however, this setting (like all others) is only applied when `OpampN.init()` is called; all opamps begin completely disabled.

#### output_pin
`OpampN.output_pin` is the pin number of the output pin for the opamp (PD2, PD5, or PE2 respectively). It is read-only. This is to enable things like `analogRead(Opamp0.output_pin)`.

##### Usage
```c++
Serial.print(analogRead(Opamp1.output_pin));

```
An example of practical use might be when using an opamp + DAC to provide the MVIO supply - you might not want to just leave it trying and failing to supply some excessively power-hungry device on the VDDIO2 rail, and instead shut it off and warn the user:

``` c++
  analogWrite(DAC_PIN, 192);      // let analogWrite() initialize the DAC to Vcc * 075;
  Opamp0.input_p = in_p::dac;     // Connect positive input to external input pin (PD1);
  Opamp0.input_n = in_n::output;  // Connect negative input to the opamp output
  Opamp0.output = out::enable;    // Enable output - it is now a voltage follower on the DAC...
  Opamp0.init();                  // apply settings and
  Opamp0.start();                 // Turn on opamp
  delay(100);                     // Let it stabilize
  if (analogRead(Opamp0.output_pin) < 650) {
    // Uhoh, the opamp is configured to follow the DAC, the DAC is set to Vcc * 0.75, so we should get analogReading of appriox 768. Instead, we see under 650, which implies an excessive load is connected to the pin. Turn it off.
    Opamp0.stop();
    Serial.println("Excess load detected");
  }

```
Note that the above is not a particularly rigorous implementation, nor even the simplest. I think the best approach would be to use an analog comparator (see the [Comparator](../Comparator/README.md) library) - set it so that the event level goes low if the voltage falls low enough that you believe there to be an overcurrent event. `Comparator.init_val` wants to be high. That could then be used to disable the opamp directly, without CPU intervention or any specific code nor the need to wait through 1 to 2 ADC conversions to see the result by setting `OpampN.output = out::drive_event` and using the [Event System](../Event/README.md) to connect the two. Note that this configuration would, no matter how it was done, latch into the off state, as it's very unlikely that the voltage on the secondary supply will spontaneously increase when the power source has been disconnected; this easily achieved behavior is also typically the desired behavior

##### Default state
`OpampN.inrange` defaults to `in::rail_to_rail` if not specified in the user program.


### Resistor Ladder-related properties
A key feature of the OPAMP peripherals is the existence of the resistor ladder.

#### ladder_top
The ladder_top property sets what the top of the internal resistor ladder is connected to.

##### Accepted values
``` c++
top::off;    // Leave the resistor ladder top floating
top::output; // Connect the ladder top to the opamp output
top::vdd;    // Connect the ladder top to Vdd, the supply voltage
```

##### Usage
``` c++
Opamp0.ladder_top = top::vdd; // Connect the resistor ladder top to the supply voltage
```

##### Default state
`OpampN.ladder_top` defaults to `top::off` if not specified in the user program.


#### ladder_wiper
Property to set the resistor ladder values. R2 connects to the top and to the middle and R1 connects to the middle and to the bottom. Previous versions of this document claimed specific resistances of 64k total. I can find no reference to this number in the documentation, though that ballpark sounds appropriate. In any event, it is clear that the design is implemented as a 16-element resistor ladder (typical properties specifies 4k per element, though one's ability to interact directly with it in a manner where the value is relevant are somewhat limited)

Note the presence of a single "odd option out" in option 4, due to there being a must-have option that is symmetric on exchange of the two sides of the resistor network, allowing one of the options that would be needed for that to be repurposed for the 3:5 ratio. The most plausibly useful ratio *not* possible with the resistive elements we have available is the 5:3rds ratio, having gain of 1.6x (1 3/5th) and 0.60x (3/5th). The point between an even numbered element and the following odd numbered one is only offered on the two end-most nodes, likely due primarily to the strikingly ugly options that these result in for the gain of an opamp configured as an amplifier.

The gain values that would come out of the odd taps are: 3/13, 5/11, 7/9, 9/7 (1 2/7), 11/5 (2.2) and 13/3 (4 1/3) inverting or 1 3/13, 1 5/11, 1 7/9, 2 2/7, 3.2 and 5 1/3 in a non-inverting configuration. It's not terribly surprising that the designers chose to limit it to 8 options - the

As can be seen below, calculated gains are much easier to conceptualize as a fraction than the decimal values (the nominal gain, for options 0, 1, 2, and 4 all contain a repeating fraction),


##### Accepted values

Value           | R1 | R2 | R1:R2 | gain (non) | gain (inv) |
----------------|----|----|-------|------------|------------|
`wiper::wiper0` | 15 |  1 | 15: 1 | 1  1/15    |    1/15    |
`wiper::wiper1` |  7 |  1 | 14: 2 | 1  1/7     |    1/7     |
`wiper::wiper2` |  3 |  1 | 12: 4 | 1  1/3     |    1/3     |
`wiper::wiper3` |  1 |  1 |  8: 8 | 2          | 1          |
`wiper::wiper4` |  3 |  5 |  6:10 | 2  2/3     | 1  2/3     |
`wiper::wiper5` |  1 |  3 |  4:12 | 4          | 3          |
`wiper::wiper6` |  1 |  7 |  2:14 | 8          | 7          |
`wiper::wiper7` |  1 | 15 |  1:15 | 16         | 15         |

``` text

Bottom ---/\/\---/\/\---/\/\--/\/\---/\/\--/\/\--/\/\--/\/\---/\/\--/\/\---/\/\--/\/\---/\/\--/\/\---/\/\---/\/\-- Top
               |      |            |                        |            |            |            |      |            <-- Valid Wiper connections.

```


##### Usage
``` c++
Opamp0.ladder_wiper = in_p::wiper0; // R1:R2 = 15:1
```

##### Default state
`OpampN.ladder_wiper` defaults to `wiper::wiper3` if not specified in the user program.


#### ladder_bottom
Property for controlling what the bottom of the internal resistor ladder connects to.

##### Accepted values
``` c++
bottom::off;         // Leave the resistor ladder bottom floating
bottom::in_p;        // Connect the ladder bottom to the opamp positive input internally
bottom::in_n;        // Connect the ladder bottom to the opamp negative input internally
bottom::dac;         // Connect the ladder bottom to the DAC internally
bottom::link;        // Connect the ladder bottom to the previous opamps output internally. Available on all three OPAMPs, with OP0 getting the output of OP2.
bottom::gnd;         // Connect the ladder bottom to ground internally
```

##### Usage
``` c++
Opamp0.ladder_bottom = bottom::gnd; // Connect the ladder bottom to ground internally
```

##### Default state
`OpampN.ladder_bottom` defaults to `bottom::off` if not specified in the user program.


### Other properties

#### event
Property for enabling event reception and generation. The opamps each have 4 event users (enable, disable, drive, and dump) as well as one generator that is "high" when it is settled. See the datasheet for more information; it is possible to do most of the control via the event system, if your application requires that.

##### Accepted values
``` c++
event::disable; // Disable event reception and generation
event::enable;  // Enable event reception and generation
```

##### Usage
``` c++
Opamp0.event = event::disable;
```

##### Default state
`OpampN.event` defaults to `event::enable` if not specified in the user program.


#### standby
Property to specify if the opamp should run while the microcontroller is in standby or not.

##### Accepted values
``` c++
power::no_standby; // Opamp running while in standby. While in standby, this significantly increases power consumption.
power::standby;    // Opamp not running while in standby
```

##### Usage
``` c++
Opamp0.standby = power::standby;
```

##### Default state
`OpampN.standby` defaults to `power::no_standby` if not specified in the user program.


#### settle
Property to set the number of microseconds allowed for the opamp output to settle.

Ranges from 0 to 127 permitted, these result in settling time of 1 to 128 microseconds. This uses the timebase property, which is set by `Opamp.start()` automatically to the correct version for the configured F_CPU.

Per the datasheet:
> Since the settling time depends on a variety of factors, including the load on the op amp, it may not be known until the later stages of design and development. If the settling time is unknown, the maximum value of ‘0x7F’ (127 μs) should be written to the SETTLE bit field

In light of that, it may provide insight into the design philosophy that the manufacturer default is 0x00, not 0x7F. Consider the case where an insufficient settling time was used, causing the opamp to be treated as settled and set the interrupt/fire the event (the event can be piped to an oscilloscope, remember, which may be of use if repeatedly reconfiguring the opamp. No configuration change to the OPAMP is needed togenerat

##### Usage
``` c++
Opamp0.settle = 0x40;
```

##### Default state
`OpampN.settle` defaults to `0x7F / 127` if not specified in the user program.


#### wait_settle
Property to set whether the `Opamp::start()` or `Opamp::start(true)` will wait until this opamp's status is "settled" before returning.

##### Accepted values
Boolean values, `true` or `false`.

##### Usage
``` c++
Opamp0.wait_settle = false; // do not wait for this opamp to finish starting up,
```

##### Default state
`OpampN.input_n` defaults to `true` if not specified in the user program.


#### enable
Property to set whether the Opamp is turned on. Opamps which are not turned on, if set to use events, can still be turned on by the ENABLE event.

##### Usage
``` c++
enable::disable;     // Opamp not enabled (barring events)
enable::enable;      // Opamp enabled
enable::event;       // Opamp not enabled (barring events - syntactic sugar)
enable::always_on;   // Opamp enabled (syntactic sugar)
enable::unconfigured // this will leave revious event configuration
```
If using event control, `enable::event` is recommended for clarity, otherwise, `enable::disable`. `enable::always_on` mirrors terminology in datasheet.

##### Default state
`OpampN.enable` defaults to `enable::enable` if not specified in the user program - however, since it is only written to the opamp control registers when `OpampN.init()` is called, an opamp that the user has not touched will not be enabled.


### Methods

#### get_number()
The get_number() method simply returns the Opamp number - useful if it is being passed around as a reference, for example.

##### Usage
```c++
uint8_t comparator_number = Opamp1.get_number(); // get_number() will return 1
```


#### status()
The status() method returns `true` if the opamp has settled after a configuration change and `false` if it has not. The opamps become 'unsettled' when you change the configuration or first start the opamps.
##### Usage
```c++
bool opamp_status = Opamp0.status(); // Returns true if opamp has settled
```


#### calibrate()
Method to calibrate the opamp input DC offset. `0x00` provides the most negative value of offset adjustment, `0x80` provides no offset adjustment, and `0xFF` provides the most positive value of offset adjustment. These calibration values could be found either experimentally (a number of approaches using the on-chip analog comparator or ADC come to mind. You'll get better results with an external, high accuracy volt meter and some reference voltage that is ideally separate from Vdd; just set it to be a voltage follower, apply a voltage to the positive pin, and measure the difference between the voltage on the two pins as accurately as you can. Adjust the value passed to this to minimize it. This value will vary between specimens as well as between opamps on a single chip. Be sure to record it after measuring it; I would suggest storing it in the USERROW, so that it will survive a chip erase. It is unclear how often or under what conditions the calibration step must be repeated; one has to imagine that it is not valid forever, otherwise they could perform this calibration at the factory - but even if it is only valid over the short term, storing it somewhere that won't be erased with every upload will make life easier.

##### Usage
```c++
Opamp0.calibrate(0x90); // Small positive adjustment
```


#### init()
Once the desired parameters have been set, the `OpampN.init()` method must be called; this will write that configuration to the peripheral registers. The init method can be called while the opamp is running, though configuration changes will involve waiting through another settling period.

##### Usage
```c++
Opamp0.init(); // Initialize the opamp
```

```c++
// Opamp0 is enabled, Opamp::start() called a while ago.
// but we need to change wiper value now...
// and wait until it is settled before continuing.
Opamp0.ladder_wiper=wiper::wiper6;
Opamp0.init();              // Initialize w/new settings
while (!Opamp0.settled());  // wait until it settles.
```

#### start()
Static method to start start the on-chip Opamp(s) that have been enabled and initialized; this is the overall control for all opamps on the chip. Unlike the CCL blocks, Opamps can be reconfigured on the fly as shown in the previous usage example. `Start()` also waits for all enabled opamps to settle before returning unless `OpampN.wait_settled` has been set to `false`.
##### Usage
```c++
Opamp::start();
```


#### stop()
Static method to disable all opamps. There is no settling time when they are disabled.
##### Usage
```c++
Opamp::stop();
```

## Appendix: Useful Connections Reference
The OPAMP peripherals are often useful in combination with the ADC and the AC. The ADC, there's little to say about it. Yes, you can read the pin inputs with the ADC. You can also read the output pin with the ADC. Despite quotes implying internal routing to the ADC, I find no evidence in the register description to support this.

### Pins for OPAMPs
"Yes, we have no PORTMUX today!" I believe this is the only peripheral wrapper in this style where there is not a single PORTMUX option. Not surprising, really. But the pins were picked well, and are almost always convenient.

| Opamp  | + Pin | - Pin | OUT  | Comments               |
|--------|-------|-------|------|------------------------|
| Opamp0 | PD1   | PD3   | PD2  |                        |
| Opamp1 | PD4   | PD7   | PD5  | PD6 is the DAC output. |
| Opamp2 | PE1   | PE3   | PE2  | 48/64-pin parts only.  |

Mind the strange set of pins:
* Because 28 and 32 pin parts don't have PD0, and an opamp isn't much good lacking a positive pin,
* Because PD6, not PD5 is the DAC, OPAMP1 straddles PD6.
* OPAMP2 has the same layout as OPAMP0 for consistency.

Analog comparator pin mappings in contrast, lack rhytme and are scant on reason. OPAMP0 and OPAMP1 are far better connected than the third wheel.

### OPAMP0 pin and AC pin overlap

The ROWs contain the analog comparator.

The COLUMNS contain the pins OF THE OPAMP.

If a value is present, that value is the name of the AC input connected to the specified pin of the AC.


| AC  | + Pin | - Pin | OUT   |
|-----|-------|-------|-------|
| AC0 | -     | IN_N0 | IN_P0 |
| AC1 | -     | IN_N1 | IN_P0 |
| AC2 | -     | -     | IN_P0 |

PD1 is a ZCD pin, and the comparators shun it.

### OPAMP1 pin and AC pin overlap

| AC  | + Pin | - Pin | OUT   |
|-----|-------|-------|-------|
| AC0 | -     | IN_N2 | -     |
| AC1 | IN_P2 | IN_N2 | IN_N0 |
| AC2 | IN_P1 | IN_N2 | -     |

### OPAMP2 pin and AC pin overlap

| AC  | + Pin | - Pin | OUT   |
|-----|-------|-------|-------|
| AC0 | -     | -     | IN_P2 |
| AC1 | -     | -     | -     |
| AC2 | IN_P2 | -     | -     |

PE3 is a ZCD pin, and the comparators shun it.

## Nonidealities of this design
I'm talking about this library, not the hardware  I think I gave excess deference to the hardware's organization scheme. The four low bits of `CTRLA` could have been one field:

| Out[1] | Out[0] | Event | Alwayson | Result
|--------|--------|-------|----------|------------------------
|      0 |      0 |     0 |        0 | Peripheral disabled.
|      0 |      0 |     0 |        1 | Enabled? But output driver is not enabled without a `DRIVE` event, and event reception is not enabled. I believe this is pathological.
|      0 |      0 |     1 |        0 |
|      0 |      0 |     1 |        1 | I think this enables events, but not ENABLE/DISABLE, since always on is described as having precedence - however, it will not output anything without a `DRIVE`.
|      0 |      1 |     0 |        0 | Unclear. I believe
|      0 |      1 |     0 |        1 | Opamp enabled with output driver on, without any events.
|      0 |      1 |     1 |        0 |
|      0 |      1 |     1 |        1 |
|      1 |      x |     x |        x | Invalid Configuration,
