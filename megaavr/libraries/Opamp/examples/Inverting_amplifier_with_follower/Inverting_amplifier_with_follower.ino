/***********************************************************************|
| AVR-DB Opamp library                                                  |
|                                                                       |
| Inverting_amplifier_with_follower.ino                                 |
|                                                                       |
| A library for interfacing with the built-in AVR-DB Opamps             |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use opamp 0 as voltage follower to drive the       |
| inverting opamp 1. Since the output can't go below 0V, we center the  |
| output voltage around Vdd/2, which is 5V if your supply voltage is    |
| 5V. This means that if 1.5V is applied to PD1, PD5 will output 3.5V.  |
|                                                                       |
|                 | Gain   |  Wiper value  | R1  | R2  |                |
|                 |--------|---------------|-----|-----|                |
|                 | -0.067 | wiper::wiper0 | 60k | 4k  |                |
|                 | -0.143 | wiper::wiper1 | 56k | 8k  |                |
|                 | -0.334 | wiper::wiper2 | 48k | 16k |                |
|                 | -1.00  | wiper::wiper3 | 32k | 32k |                |
|                 | -1.667 | wiper::wiper4 | 24k | 40k |                |
|                 | -3.00  | wiper::wiper5 | 16k | 48k |                |
|                 | -7.00  | wiper::wiper6 | 8k  | 56k |                |
|                 | -15.0  | wiper::wiper7 | 4k  | 60k |                |
|          |\                                                           |
|          |  \                                                         |
|  PD1 ----| +  \           ______           ______                     |
|          | OP0 |----+----|__R1__|----+----|__R2__|----+               |
|     +----| -  /     |                |                |               |
|     |    |  /       |                |    |\          |               |
|     |    |/         |                |    |  \        |               |
|     |               |                +----| -  \      |               |
|     +---------------+                     | OP1 |-----+----- PD5      |
|                                 Vdd/2 ----| +  /                      |
|                                           |  /                        |
|                                           |/      Gain = - (R2 / R1)  |
|                                                                       |
|                                                                       |
| See Microchip's application note TB3286 for more information.         |
|***********************************************************************/

#include <Opamp.h>

void setup() {
  // Configure Opamp0 as a voltage follower tied to the Opamp1 resistor ladder
  Opamp0.input_p = in_p::pin;    // Connect positive input to external input pin (PD1)
  Opamp0.input_n = in_n::output; // Connect negative input to the opamp output
  Opamp0.output  = out::enable;  // Enable output

  // Configure Opamp1 pins
  Opamp1.input_n = in_n::wiper;    // Connect negative input to the "middle" position of the resistor ladder
  Opamp1.input_p = in_p::vdd_div2; // Connect positive input to Vdd/2 internally
  Opamp1.output  = out::enable;    // Enable output

  // Configure Opamp1 resistor ladder
  Opamp1.ladder_top    = top::output;   // Connect the resistor ladder top to the opamp output
  Opamp1.ladder_bottom = bottom::link;  // Connect the resistor ladder bottom to the output of the previous opamp
  Opamp1.ladder_wiper  = wiper::wiper3; // Set a gain of -1

  // Initialize Opamps
  Opamp0.init();
  Opamp1.init();

  // Start the Opamp hardware
  Opamp::start();
}

void loop() {

}
