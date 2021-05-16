# Opamp
A library for interfacing with the opamp peripheral on the AVR-DB series MCUs.

Developed by [MCUdude](https://github.com/MCUdude/).

The AVR-DB 48 and 64 pin chips have three built-in opamps, while the 32 and 28 pin ones have only two. More useful information about the opamp peripheral can be found in the [Microchip Application Note TB3286](https://ww1.microchip.com/downloads/en/Appnotes/GettingStarted-Analog-SignCondOPAMP-DS90003286A.pdf).


## Opamp
Class for interfacing with the built-in opamps. Use the predefined objects `Opamp0`, `Opamp1` or `Opamp2`.


### input_p
Property for controlling what the positive opamp input connects to.
Accepted values:
``` c++
in_p::pin;         // Connect the pinout to the external pin
in_p::wiper;       // Connect the input to the wiper internally
in_p::dac;         // Connect the input to the DAC internally
in_p::gnd;         // Connect the input to ground internally
in_p::vdd_div2;    // Connect the input to Vdd/2 internally, which is half the supply voltage
in_p::link_output; // Connect the input to the previous opamps output internally (only available for Opamp1 and Opamp2)
in_p::link_wiper;  // Connect the input to the Opamp0 wiper internally (only available for Opamp2)
```

##### Usage
``` c++
Opamp0.input_p = in_p::vdd_div2; // Connect positive opamp input to Vdd/2
```

##### Default state
`OpampN.input_p` defaults to `in_p::pin` if not specified in the user program.



### input_p_pin
`OpampN.input_p_pin` is the pin number of the negative input pin for the opamp (PD1, PD4, or PE1 respectively). Use it to check your opamp's math and configuration (if you lack faith in one or more of: the laws of physics, Microchip's analog design talent, or your understanding of the opamp peripheral) or with digitalWrite() as yet another way to influence the behavior of the opamp, with subtly different specifics.

##### Usage
```c++
Serial.print(analogRead(Opamp1.input_p_pin));
```


### input_n
Property for controlling what the negative opamp input connects to.
Accepted values:
``` c++
in_n::pin;    // Connect the pinout to the external pin
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


### input_n_pin
`OpampN.input_n_pin` is the pin number of the negative input pin for the opamp (PD3, PD7, or PE3 respectively). Use it to check your opamp's math and configuration (if you lack faith in one or more of: the laws of physics, Microchip's analog design talent, or your understanding of the opamp peripheral) or with digitalWrite() as yet another way to influence the behavior of the opamp, with subtly different specifics.

##### Usage
```c++
Serial.print(analogRead(Opamp1.input_n_pin));
```


### output
Property for controlling the opamp output. Opamp output pins cannot be remapped. They are PD2, PD5, and PE2.
Accepted values:
``` c++
out::disable;      // Do not enable output (can be overridden be DRIVE event)
out::enable;       // Enable output
out::drive_event;  // Enable output only with DRIVE event (syntactic sugar for disable)
```
If using the drive event to control output, `out::drive_event` is recommended to improve readability, even though it is the same as `out::disable;`

##### Usage
``` c++
Opamp0.output = out::enable; // enable output
```

##### Default state
`OpampN.output` defaults to `out::enable` for any initialized opamp if not specified in the user program (if an OpampN.init() is never called, it will not be turned on).

### output_pin
`OpampN.output_pin` is the pin number of the output pin for the opamp (PD2, PD5, or PE2 respectively). Hence you can do things like `analogRead(Opamp0.output_pin)`.

##### Usage
```c++
Serial.print(analogRead(Opamp1.output_pin));

```
An example of practical use might be when using an opamp + DAC to provide the MVIO supply - you might not want to just leave it trying and failing to supply some excessively power-hungry device on the VDDIO2 rail, and instead shut it off and warn the user:

``` c++
  analogWrite(DAC_PIN,192);       // let analogWrite() initialize the DAC to Vcc * 075;
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

### inrange
Property for controlling the opamp input range - this is set globally, not per opamp. If it is not specified for a given opamp, `init` will not set it one way or the other.
Accepted values:k,23q8i
``` c++
in::rail_to_rail; // Consumes more power but can handle signal close to the ground and power rails. The default.
in::conventional; // Consumes less power but can't handle signal close to the power rail.
```

##### Usage
``` c++
Opamp0.inrange = in::rail_to_rail; // Set rail to rail mode
```

##### Default state
`OpampN.inrange` defaults to `in::rail_to_rail` if not specified in the user program.


### ladder_top
Property for controlling what the top of the internal resistor ladder connects to.
Accepted values:
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


### ladder_wiper
Property to set the resistor ladder values. R2 connects to the top and to the middle and R1 connects to the middle and to the bottom.
Accepted values:
``` c++
wiper::wiper0; // Set R1 = 60k, R2 = 4k
wiper::wiper1; // Set R1 = 56k, R2 = 8k
wiper::wiper2; // Set R1 = 48k, R2 = 16k
wiper::wiper3; // Set R1 = 32k, R2 = 32k
wiper::wiper4; // Set R1 = 24k, R2 = 40k
wiper::wiper5; // Set R1 = 16k, R2 = 48k
wiper::wiper6; // Set R1 = 8k,  R2 = 56k
wiper::wiper7; // Set R1 = 4k,  R2 = 60k
```

##### Usage
``` c++
Opamp0.ladder_wiper = in_p::wiper0; // Set R1 = 60k, R2 = 4k
```

##### Default state
`OpampN.ladder_wiper` defaults to `wiper::wiper3` if not specified in the user program.


### ladder_bottom
Property for controlling what the bottom of the internal resistor ladder connects to.
Accepted values:
``` c++
bottom::off;         // Leave the resistor ladder bottom floating
bottom::in_p;        // Connect the ladder bottom to the opamp positive input internally
bottom::in_n;        // Connect the ladder bottom to the opamp negative input internally
bottom::dac;         // Connect the ladder bottom to the DAC internally
bottom::link;        // Connect the ladder bottom to the previous opamps output internally (only available for Opamp1 and Opamp2)
bottom::gnd;         // Connect the ladder bottom to ground internally
```

##### Usage
``` c++
Opamp0.ladder_bottom = bottom::gnd; // Connect the ladder bottom to ground internally
```

##### Default state
`OpampN.ladder_bottom` defaults to `bottom::2` if not specified in the user program.


### event
Property for enabling event reception and generation. The opamps each have 4 event users (enable, disable, drive, and dump) as well as one generator that is "high" when it is settled. See the datasheet for more information; it is possible to do most of the control via the event system, if your application requires that.
Accepted values:
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


### standby
Property to specify if the opamp should run while the microcontroller is in standby or not.
Accepted values:
``` c++
power::no_standby; // Opamp running while in standby
power::standby;    // Opamp not running while in standby
```

##### Usage
``` c++
Opamp0.standby = power::standby;
```

##### Default state
`OpampN.standby` defaults to `power::no_standby` if not specified in the user program.


### settle
Property to set the number of microseconds allowed for the opamp output to settle.
Ranges from 0 to 127 microseconds.

##### Usage
``` c++
Opamp0.settle = 0x40;
```

##### Default state
`OpampN.settle` defaults to `0x7F / 127` if not specified in the user program.


### wait_settle
Property to set whether the Opamp::start() will wait until this opamp's status is "settled" before returning. Boolean values, `true` or `false`.

##### Usage
``` c++
Opamp0.wait_settle = false; // do not wait for this opamp to finish starting up,
```

##### Default state
`OpampN.input_n` defaults to `true` if not specified in the user program.


### enable
Property to set whether the Opamp is turned on. Opamps which are not turned on, if set to use events, can still be turned on by the ENABLE event.

##### Usage
``` c++
enable::disable;    // Opamp not enabled (barring events)
enable::enable;     // Opamp enabled
enable::event;      // Opamp not enabled (barring events - syntactic sugar)
enable::always_on;  // Opamp enabled (syntactic sugar)
```
If using event control, `enable::event` is recommended for clarity, otherwise, `enable::disable`. `enable::always_on` mirrors terminology in datasheet.

##### Default state
`OpampN.enable` defaults to `enable::enable` if not specified in the user program - however, since it is only written to the opamp control registers when `OpampN.init()` is called, an opamp that the user has not touched will not be enabled.


## get_number()
Method that returns the Opamp number - useful if it is being passed around as a reference, for example.

##### Usage
```c++
uint8_t comparator_number = Opamp1.get_number(); // get_number() will return 1
```


## status()
Method that returns `true` if the opamp has settled after a configuration change and `false` otherwise.

##### Usage
```c++
bool opamp_status = Opamp0.status(); // Returns true if opamp has settled
```


## calibrate()
Method to calibrate the opamp input DC offset. `0x00` provides the most negative value of offset adjustment, `0x80` provides no offset adjustment, and `0xFF` provides the most positive value of offset adjustment.

##### Usage
```c++
Opamp0.calibrate(0x90);
```


## init()
Method that initialized the opamp and writes all the settings properties to their appropriate registers. The init method can be called while the opamp is running, though configuration changes will involve waiting through another settling period.

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

## start()
Static method that starts the opamp hardware.

##### Usage
```c++
Opamp::start();
```


## stop()
Static method that disables the opamp hardware.

##### Usage
```c++
Opamp::stop();
```
