/***********************************************************************|
| megaAVR Configurable Custom Logic library                             |
|                                                                       |
| Two_input_OR.ino                                                      |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude. Example fixed Spence Konde 2021         |
| This version is part of DxCore for AVR DA, DB, etc. Example works for |
| megaAVR 0-series and tinyAVR 0/1/2-series as well.                    |
|                                                                       |
| In this example we use the configurable logic peripherals the the     |
| megaAVR to create a 2-input OR gate using logic block 0 on PORT A.    |
| The example is pretty straight forward, but the truth table value may |
| be a little difficult to understand at first glance.                  |
| We will only use PA1 and PA2 as inputs. when the first input is       |
| disabled it will always be read as 0.                                 |
| Here's how 0xFE turns out to be the correct value to create a 2-input |
| OR gate:                                                              |
|                                     2-input OR truth table:           |
|                                     |IN2|IN1|IN0| Y |                 |
| If we look at the truth table       |PA2|PA1|---|OUT|                 |
| to the right, we can see that       |---|---|---|---|                 |
| all binary values for Y can         | 0 | 0 | 0 | 0 |                 |
| be represented as x1x0x0x0.         | 0 | 0 | 1 | x | IN0 is always 0 |
| Using a 0 for all don't care bits   | 0 | 1 | 0 | 1 |                 |
| gives us 0b01010100. But that isn't | 0 | 1 | 1 | x | IN0 is always 0 |
| a great choice in terms of code     | 1 | 0 | 0 | 1 |                 |
| readability. Either 0b11111100 or   | 1 | 0 | 1 | x | IN0 is always 0 |
| 0b11111110 do a more unambiguous    | 1 | 1 | 0 | 1 |                 |
| job of communicating to the reader  | 1 | 1 | 1 | x | IN0 is always 0 |
| that anything except a 0 on all inputs                                |
| that aren't masked will result in a 1 output.                         |
|***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // It has one output, PA3, but can be swapped to PA6 if needed
  Logic0.enable = true;                 // Enable logic block 0
  Logic0.input1 = in::input_pullup;     // Set PA1 as input with pullup
  Logic0.input2 = in::input_pullup;     // Set PA2 as input with pullup
  // Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to to alternate location, if available.
  Logic0.output = out::enable;          // Enable logic block 0 output pin (PA3)
  Logic0.filter = filter::disable;      // No output filter enabled
  Logic0.truth = 0xFE;                  // Set truth table

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
