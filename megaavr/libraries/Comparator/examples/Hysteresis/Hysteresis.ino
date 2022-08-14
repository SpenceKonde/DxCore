/***********************************************************************|
| Modern AVR Comparator library for tinyAVR 0/1/2, megaAVR0, Dx, and  Ex|
|                                                                       |
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| https://github.com/SpenceKonde/megaTinyCore                           |
| Ported to tinyAVR 2022 by Spence Konde for DxCore                     |
| https://github.com/SpenceKonde/DxCore                                 |                   |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise. We'll also use the built-in    |
| hysteresis functionality to prevent spurious signals while the        |
| voltage on the two pins is very close                                 |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;      // Use positive input 0 - these are boring options, but they will compile everywhere
  Comparator.input_n = comparator::in_n::in0;      // Use negative input 0 - which is critical as these are used for CI testing too.
  Comparator.hysteresis = comparator::hyst::large; // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;     // Enable output on digital pin 7 (PA7)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
