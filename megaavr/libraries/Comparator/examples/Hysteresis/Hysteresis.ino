/***********************************************************************|
| AVR DA/DB/DD analog comparator library                                |
|                                                                       |
| Hysteresis.ino                                                        |
|                                                                       |
| A library for interfacing with the AVR DA/DB analog comparator.       |
| Developed in 2019 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the negative input 2 and positive input 3 of   |
| the comparator. Those inputs were chosen so that any comparator could |
| be used in place of Comparator (Comparator0, AC0) and the comment     |
| would still be accurate.                                              |
| The output goes high if the positive input is higher than             |
| the negative input, and low otherwise. We'll also use the built-in    |
| hysteresis functionality to prevent false spikes.                     |
************************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = in_p::in0;      // Use positive input 0 - these are boring options, but they will compile everywhere
  Comparator.input_n = in_n::in0;      // Use negative input 0 - which is critical as these are used for CI testing too.
  Comparator.hysteresis = hyst::large; // Use a 50mV hysteresis
  Comparator.output = out::enable;     // Enable output on digital pin 7 (PA7)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
