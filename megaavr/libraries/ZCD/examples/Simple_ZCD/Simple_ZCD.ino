/***********************************************************************|
| AVR DA/DB Zero-cross detect library                                   |
|                                                                       |
| Simple_ZCD.ino                                                        |
|                                                                       |
| A library for interfacing with the AVR DA/DB zero-cross detector.     |
| Developed in 2020 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we enable the ZCD hardware to toggle the output pin   |
| when a zero-cross occurs.                                             |
|***********************************************************************/

#include <ZCD.h>

void setup() {
  // Enable the zero-cross detection output pin
  zcd.output = out::enable;

  // Initialize zcd
  zcd.init();

  // Start ZCD
  zcd.start();
}

void loop() {

}
