/***********************************************************************|
| AVR-Dx Configurable Custom Logic library                             |
|                                                                       |
| Five_input_NOR.ino                                                    |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use two logic blocks to get five inputs.           |
| The output of block 1 is connected to one of the inputs of block 0.   |
| With the correct truth tables values we can make the output of        |
| block 0 go high when all inputs are low.                              |
|                                                                       |
| See Microchip's application note TB3218 for more information.         |
|***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 1
  // Logic block 1 has three inputs, PC0, PC1 and PC2 on ATmega parts

  Logic1.enable = true;               // Enable logic block 1
  Logic1.input0 = logic::in::input_pullup;   // Set PC0 as input with pullup
  Logic1.input1 = logic::in::input_pullup;   // Set PC1 as input with pullup
  Logic1.input2 = logic::in::input_pullup;   // Set PC2 as input with pullup
  Logic1.output = logic::out::disable;       // Enable output pin
  Logic1.filter = logic::filter::disable;    // No output filter enabled
  Logic1.truth = 0x01;                // Set truth table

  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // Block 0 output on PA3 or PC6

  Logic0.enable = true;               // Enable logic block 0
  Logic0.input0 = logic::in::link;           // Route output from block 1 to this input internally
  Logic0.input1 = logic::in::input_pullup;   // Set PA1 as input with pullup
  Logic0.input2 = logic::in::input_pullup;   // Set PA2 as input with pullup
  Logic0.output = logic::out::enable;        // Enable logic block 0 output pin
  Logic0.filter = logic::filter::disable;    // No output filter enabled
  Logic0.truth = 0xFE;                // Set truth table

  // Initialize logic block 0 and 1
  Logic0.init();
  Logic1.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
