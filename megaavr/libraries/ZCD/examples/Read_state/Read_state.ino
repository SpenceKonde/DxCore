/***********************************************************************|
| AVR DA/DB Zero-cross detect library                                   |
|                                                                       |
| Read_state.ino                                                        |
|                                                                       |
| A library for interfacing with the AVR DA/DB zero-cross detector.     |
| Developed in 2020 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we enable the ZCD hardware so we can read the ZCD     |
| state internally.                                                     |
|***********************************************************************/

#include <ZCD.h>

void setup() {
  // Configure serial port
  Serial1.begin(9600);

  // Disable the zero-cross detection output pin
  // We'll read the signal internally instead
  zcd.output = out::disable;

  // Initialize ZCD
  zcd.init();

  // Start ZCD
  zcd.start();
}

void loop() {
  Serial1.printf("Zero cross state: %d\n", zcd.read());
  delay(250);
}
