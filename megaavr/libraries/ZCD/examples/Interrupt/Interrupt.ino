/***********************************************************************|
| AVR DA/DB Zero-cross detect library                                   |
|                                                                       |
| Interrupt.ino                                                         |
|                                                                       |
| A library for interfacing with the AVR DA/DB zero-cross detector.     |
| Developed in 2020 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we enable the ZCD hardware to toggle the output pin   |
| when a zero-cross occurs. An interrupt will also be triggered when    |
| the ZCD detects a rising input signal.                                |
|***********************************************************************/

#include <ZCD.h>

void setup() {
  // Configure serial port
  Serial1.begin(9600);

  // Enable the zero-cross detection output pin
  zcd.output = out::enable;

  // Initialize ZCD
  zcd.init();

  // Set interrupt (supports RISING, FALLING and CHANGE)
  zcd.attachInterrupt(interruptFunction, RISING);

  // Start ZCD
  zcd.start();
}

void loop() {

}

// This function runs when an interrupt occurs
void interruptFunction() {
  Serial1.println("Zero-cross output went high!");
}
