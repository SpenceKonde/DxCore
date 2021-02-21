/***********************************************************************|
| AVR DA/DB Zero-cross detect library                                   |
|                                                                       |
| Frequency_counter.ino                                                 |
|                                                                       |
| A library for interfacing with the AVR DA/DB zero-cross detector.     |
| Developed in 2020 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we enable the ZCD hardware to toggle the output pin   |
| when a zero-cross occurs. An interrupt will also be triggered when    |
| the ZCD detects a rising input signal. The number of interrupts       |
| occurring every second is used to calculate the frequency of the      |
| input signal. Note that this is a very crude frequency counter        |
| implementation, and the microcontroller main clock plays a major role |
| when it comes to accuracy. Done expect this to be very accurate!      |
|***********************************************************************/

#include <ZCD.h>

uint32_t counter = 0;

void setup() {
  // Configure serial port
  Serial1.begin(9600);

  // Disable the zero-cross detection output pin
  zcd.output = out::disable;

  // Initialize ZCD
  zcd.init();

  // Set interrupt (supports RISING, FALLING and CHANGE)
  zcd.attachInterrupt(interruptFunction, RISING);

  // Start ZCD
  zcd.start();
}

void loop() {
  if (millis() % 1000 == 0) {
    cli();       // Disable interrupts
    Serial1.printf("Input signal frequency = %ld Hz\n", counter);
    counter = 0; // Clear frequency counter
    sei();       // Enable interrupts
    delay(1);    // Prevent this from being executed twice
  }
}

// This function runs when an interrupt occurs
void interruptFunction() {
  counter++;
}
