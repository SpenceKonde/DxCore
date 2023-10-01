/***********************************************************************|
| Configurable Custom Logic library                                     |
| Developed in 2019 by MCUdude. https://github.com/MCUdude/             |
| Ported to DxC and maintained for mTC and DxC by Sprence Konde         |
| Example by Spence Konde 2021                                          |
| Modified to use event system and work on DD14 by Andrew J. Kroll      |
|                                                                       |
| LatchNoSeqDx.ino - Getting "RS Latch" like behavior with a single   |
| even numbered LUT. Should work on ALL parts.                          |
|                                                                       |
| See the original LatchNoSeq for additional discussion of the rationale|
| This example is specific to DxCore, and will work on all parts there  |
| It demonstrates use of event channels to deal with pin constraints    |
|                                                                       |
| In this example we use the configurable logic peripherals in AVR      |
| to act as a "latch" WITHOUT using both LUTs and the sequencer         |
| For the even-numbered logic block(s) we can simply use the feedback   |
| input. Otherwise we need to use the event system.                     |
|                                                                       |
|                                           3-input     truth table:    |
| We use CCL LUT event as our "feedback",   |PC1|PC3|CCL| Y |           |
| PC1 is RESET and PC3 is SET, both         |---|---|---|---|           |
| active low                                | 0 | 0 | 0 | 0 |           |
| Connect a button between those and Gnd    | 0 | 0 | 1 | 1 |           |
| Pressing button on PC3 will set output    | 0 | 1 | 0 | 1 |           |
| HIGH and pressing button on PC1 will set  | 0 | 1 | 1 | 1 |           |
| output LOW, and pressing neither will do  | 1 | 0 | 0 | 0 |           |
| nothing.                                  | 1 | 0 | 1 | 0 |           |
| We could even then fire an interrupt from | 1 | 1 | 0 | 0 |           |
| that pin                                  | 1 | 1 | 1 | 1 |           |
|                                                                       |
| The sky (well, and the number of LUTs) is the limit!!                 |
|                                                                       |
| This file is only part of DxCore; it is not distributed with mTC.     |
|***********************************************************************/


// PC1 RESET, PC3 SET -> PC2 RESULT
#include <Logic.h>
#include <Event.h>

void setup() {

  // low for reset
  pinMode(PIN_PC1, INPUT_PULLUP);
  Event2.set_generator(PIN_PC1);
  Event2.set_user(user::ccl0_event_a);
  Event2.start();

  // low for set
  pinMode(PIN_PC3, INPUT_PULLUP);
  Event3.set_generator(PIN_PC3);
  Event3.set_user(user::ccl0_event_b);
  Event3.start();

  Event4.set_generator(gen::ccl0_out);
  Event4.set_user(user::evoutc_pin_pc2);
  Event4.start();

  Logic0.enable = true;
  Logic0.input0 = in::feedback;
  Logic0.input1 = in::event_a;            // PC1 as input1 (RESET)
  Logic0.input2 = in::event_b;            // PC2 as input2 (SET)
  Logic0.filter = filter::disable;        // No output filter enabled
  Logic0.truth = 0x8E;                    // Set truth table
  Logic0.init();                          // Initialize logic block 0
  Logic::start();

}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything
}
