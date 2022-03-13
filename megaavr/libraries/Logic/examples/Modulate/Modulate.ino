/***********************************************************************|
| Configurable Custom Logic library                                     |
| Developed in 2019 by MCUdude.      https://github.com/MCUdude/        |
| Example by Spence Konde.
|                                                                       |
| Modulate.ino - replicating the "timer modulation" feature             |
|                                                                       |
| In this example we use the configurable logic peripherals of the of a |
| modern AVR to achieve the "modulate one timer's PWN with another's"   |
| behavior like some of the more full-featured classic megaAVR parts    |
| We set one input to TCA0 WO1, the other to TCB1 WO, and mask the third|
| and set the truth table so that the output is only HIGH when both are |
| HIGH. You can add some third condition as well, if that was somehow   |
| useful too!                                                           |
| As can be immediately seen, this is vastly more powerful than what the|
| classic AVRs were capable of, where this could be done only with two  |
| timers on a single pin. Now any CCL output pin can be used be made to |
| output any timer channel, modulated by any other. We could even use   |
| the CCL as an event generator to move the output to one of the EVOUT  |
| pins!                                                                 |
| On the Dx-series and 1-series parts, one of the TCD channels might be |
| used, which may be the most useful thing to modulate with another     |
| timer, with the higher maximum speed and dithering features.          |
************************************************************************/

#include <Logic.h>

void setup() {


  Logic0.enable = true;                 // Enable logic block 0
  Logic0.input0 = in::tca0;             // Use TCA WO0 as input0
  Logic0.input1 = in::tcb;              // Use TCB WO as input 1 (input 1 = TCB1)
  Logic0.input2 = in::masked;           // mask input 2
  // Logic0.output_swap = out::pin_swap; // Uncomment this line if you want or
  //                                       need to use the alternate output pin
  Logic0.output = out::enable;          // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = filter::disable;      // No output filter enabled
  Logic0.truth = 0x08;                  // Set truth table - HIGH only if both high
  Logic0.init();                        // Initialize logic block 0
  Logic::start();                       // Start the AVR logic hardware
  analogWrite(PIN_PB0, 128);            // start TCA0 WO0 running
  TCB1.CTRLA = 0x01;                    // enabled with CLKPER as clock source
  TCB1.CTRLB = 0x07;                    // PWM8 mode, but output pin not enabled
  TCB1.CCMPL = 255;                     // 255 counts
  TCB1.CCMPH = 128;                     // 50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
