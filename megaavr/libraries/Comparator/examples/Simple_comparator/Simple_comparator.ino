/***********************************************************************|
| AVR DA/DB analog comparator library                                   |
|                                                                       |
| Simple_comparator.ino                                                 |
|                                                                       |
| A library for interfacing with the AVR DA/DB analog comparator.       |
| Developed in 2019 by MCUdude  https://github.com/MCUdude/             |
|                                                                       |
| Ported 2021-2022 to DxCore, megaTinyCore by Spence Konde.             |                                                                      |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise.                                |
| This is much like tranplanting an 'lm339" only with better specs into |
| your AVR. Pin used are:                                               |
|       | Dx/Ex/Mega0 | TinyAVR  |  DD14      |                         |
| In_p  | PD2         |      PA7 | AINP3 PB                     |
| in_N  | PD3         |      PA6 |            |                         |
| Out   | PA7         |      PA5 | Todo: evsys|           |             |
|                                                                       |
|Note that the evetn library can by used to redirect the outputt        |
|
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;  // Use positive input 0 (PD2)
  Comparator.input_n = comparator::in_n::in0;  // Use negative input 0 (PD3)
  Comparator.output = comparator::out::enable; // Enable output on digital pin 7 (PA7)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
