/***********************************************************************|
| AVR-DB Opamp library                                                  |
|                                                                       |
| Non_inverting_amplifier.ino                                           |
|                                                                       |
| A library for interfacing with the built-in AVR-DB Opamps             |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use opamp 0 as a non-inverting amplifier with a    |
| gain multiplier set by the built-in resistor ladder.                  |
|                                                                       |
|                 | Gain |  Wiper value  | R1  | R2  |                  |
|                 |------|---------------|-----|-----|                  |
|                 | 1.06 | wiper::wiper0 | 60k | 4k  |                  |
|                 | 1.14 | wiper::wiper1 | 56k | 8k  |                  |
|                 | 1.33 | wiper::wiper2 | 48k | 16k |                  |
|                 | 2.00 | wiper::wiper3 | 32k | 32k |                  |
|                 | 2.66 | wiper::wiper4 | 24k | 40k |                  |
|                 | 4.00 | wiper::wiper5 | 16k | 48k |                  |
|                 | 8.00 | wiper::wiper6 | 8k  | 56k |                  |
|                 | 15.0 | wiper::wiper7 | 4k  | 60k |                  |
|                                                                       |
|                          |\                                           |
|                          |  \                                         |
|                 PD1 -----| +  \                                       |
|                          |     |-----+----- PD2                       |
|                     +----| -  /      |                                |
|                     |    |  /        |                                |
|                     |    |/          |        Gain = 1 + (R2 / R1)    |
|          ______     |     ______     |                                |
|    +----|__R1__|----+----|__R2__|----+                                |
|    |                                                                  |
|   ___                                                                 |
|    -                                                                  |
|                                                                       |
| See Microchip's application note TB3286 for more information.         |
|***********************************************************************/

#include <Opamp.h>

void setup() {
  // Configure opamp input pins
  Opamp0.input_p = in_p::pin;   // Connect positive input to external input pin (PD1)
  Opamp0.input_n = in_n::wiper; // Connect negative input to the "middle" position of the resistor ladder

  // Configure resistor ladder
  Opamp0.ladder_top    = top::output;   // Connect the resistor ladder top to the opamp output
  Opamp0.ladder_bottom = bottom::gnd;   // Connect the resistor ladder bottom to ground
  Opamp0.ladder_wiper  = wiper::wiper3; // Set a gain of two

  // Enable opamp output
  Opamp0.output = out::enable;

  // Initialize Opamp0
  Opamp0.init();

  // Start the Opamp hardware
  Opamp::start();
}

void loop() {

}
