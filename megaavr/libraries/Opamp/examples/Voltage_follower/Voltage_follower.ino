
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
|
| That test required an external signal generator; I wanted a quick     |
| standard test for the OPAMP - since this way it generates it's own
| signal
|
|
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
  Opamp0.input_p = in_p::dac;    // Connect positive input to external input pin (PD1)
  Opamp0.input_n = in_n::output; // Connect negative input to the opamp output

  // Enable opamp output
  Opamp0.output = out::enable;

  // Initialize Opamp0
  Opamp0.init();

  Opamp::start();
}

void loop() {
  static uint8_t count = 0;
  count++;
  digitalWrite(LED_BUILTIN,CHANGE);
  delay(100);
  analogWrite(PIN_PD6,count);
}
