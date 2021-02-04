# Opamp
A library for interfacing with the opamp peripheral on the AVR-DB series MCUs.  
Developed by [MCUdude](https://github.com/MCUdude/).  
The AVR-DB 48 and 64 pin chips have three built-in opamps, while the 32 and 28 pin ones only have two.
More useful information about the analog comparator can be found in the [Microchip Application Note TB3286](https://ww1.microchip.com/downloads/en/Appnotes/GettingStarted-Analog-SignCondOPAMP-DS90003286A.pdf).


## Opamp
Class for interfacing with the built-in opamps. Use the predefined objects `Opamp0`, `Opamp1` or `Opamp2`.


### input_p
Variable for controlling what the positive opamp input connects to.  
Accepted values:
``` c++
in_p::pin;         // Connect the pinout to the external pin
in_p::wiper;       // Connect the input to the wiper internally
in_p::dac;         // Connect the input to the DAC internally
in_p::gnd;         // Connect the input to ground internally
in_p::vdd_div2;    // Connect the input to Vdd/2 internally, which is half the supply voltage
in_p::link_output; // Connect the input to the previous opamps output internally (only avaiable for Opamp1 and Opamp2)
in_p::link_wiper;  // Connect the input to the previous opamps wiper internally (only avaiable for Opamp1 and Opamp2)
```

##### Usage
``` c++
Opamp0.input_p = in_p::vdd_div2; // Connect positive opamp input to Vdd/2
```

##### Default state
`OpampN.input_p` defaults to `in_p::pin` if not specified in the user program.


### input_n
Variable for controlling what the negative opamp input connects to.  
Accepted values:
``` c++
in_n::pin;    // Connect the pinout to the external pin
in_n::wiper;  // Connect the input to the wiper internally
in_n::output; // Connect the input to the output of the opamp internally
in_n::dac;    // Connect the input to the DAC internally
```

##### Usage
``` c++
Opamp0.input_n = in_p::wiper; // Connect positive opamp input to the center of the resistor ladder
```

##### Default state
`OpampN.input_n` defaults to `in_n::pin` if not specified in the user program.


### inrange
Variable for controlling the opamp input range.
Accepted values:
``` c++
in::conventional; // Consumes less power but can't handle signal close to the ground and power rails
in::rail_to_rail; // Consumes more power but can handle signal close to the ground and power rails
```

##### Usage
``` c++
Opamp0.inrange = in::rail_to_rail; // Set rail to rail mode

##### Default state
`OpampN.inrange` defaults to `in::rail_to_rail` if not specified in the user program.


### ladder_top
Variable for controlling what the top of the internal resistor ladder connects to
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
Variable to set the resistor ladder values. R2 connects to the top and to the middle and R1 connects to the middle and to the bottom.
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
Variable for controlling what the bottom of the internal resistor ladder connects to
Accepted values:
``` c++
bottom::off;         // Leave the resistor ladder bottom floating
bottom::in_p;        // Connect the ladder bottom to the opamp positive input internally
bottom::in_n;        // Connect the ladder bottom to the opamp negative input internally
bottom::dac;         // Connect the ladder bottom to the DAC internally
bottom::link_output; // Connect the ladder bottom to the previous opamps output internally (only avaiable for Opamp1 and Opamp2)
bottom::gnd;         // Connect the ladder bottom to ground internally
```

##### Usage
``` c++
Opamp0.ladder_bottom = bottom::gnd; // Connect the ladder bottom to ground internally
```

##### Default state
`OpampN.ladder_bottom` defaults to `bottom::2` if not specified in the user program.


### event
Variable for enabling event reception and generation.
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
Variable to specify if the opamp should run while the microcontroller is in standby or not
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
Variable to set the number of microseconds allowed for the opamp output to settle.
Ranges from 0 to 127 microseconds.

##### Usage
``` c++
Opamp0.settle = 0x40;
```

##### Default state
`OpampN.settle` defaults to `0x7F / 127` if not specified in the user program.


## get_number()
Function that returns the Opamp in use

##### Usage
```c++
uint8_t comparator_number = Opamp1.get_number(); // get_number() will return 1
```


## status()
Function that returns true if the opamp has settled after a configuration change and false otherwise.

##### Usage
```c++
bool opamp_status = Opamp0.status(); // Returns true if opamp has settled
```


## calibrate()
Function to calibrate the opamp input DC offset. `0x00` provides the most negative value of offset adjustment, `0x80` provides no offset adjustment, and `0xFF` provides the most positive value of offset adjustment.

##### Usage
```c++
Opamp0.calibrate(0x90);
```


## init()
Function that initialized the opamp and writes all the settings variables to their appropriate registers.

##### Usage
```c++
Opamp0.init(); // Initialize the opamp
```


## start()
Function that starts the opamp hardware.

##### Usage
```c++
Opamp::start();
```


## stop()
Function that disables the opamp hardware.

##### Usage
```c++
Opamp::stop();
```
