/***********************************************************************|
| AVR-DB Opamp library                                                  |
|                                                                       |
| Voltage_follower.ino                                                  |
|                                                                       |
| A library for interfacing with the built-in AVR-DB Opamps             |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use opamp 0 as a voltage follower.                 |
|                                                                       |
|                          |\                                           |
|                          |  \                                         |
|                 PD1 -----| +  \                                       |
|                          |     |-----+----- PD2                       |
|                     +----| -  /      |                                |
|                     |    |  /        |                                |
|                     |    |/          |      Gain = 1                  |
|                     |                |                                |
|                     +----------------+                                |
|                                                                       |
|                                                                       |
| See Microchip's application note TB3286 for more information.         |
|***********************************************************************/

#include <Opamp.h>

void setup() {
  // Configure opamp input pins
  Opamp0.input_p = in_p::pin;    // Connect positive input to external input pin (PD1)
  Opamp0.input_n = in_n::output; // Connect negative input to the opamp output

  // Enable opamp output
  Opamp0.output = out::enable;

  // Initialize Opamp0
  Opamp0.init();

  // Start the Opamp hardware
  Opamp::start();
}

void loop() {

}
