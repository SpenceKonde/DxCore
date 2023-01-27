#ifndef AZDUINO_MAX38903_H
#define AZDUINO_MAX38903_H
#include <Arduino.h>
#if !defined(__AVR_DB__) || !defined(_AVR_PINCOUNT) || _AVR_PINCOUNT < 64
  #error "This library is exclusively for boards sold by Azduino, and only the 64-pin AVR-DB boards we sell have this as an option. The library must be modified if it is to be used with any non-64-pin parts, or boards which use pins other than PE6, PG6, and PG7 to control the regulator."
#endif
int8_t setMAX38903Voltage(uint8_t setting);
// Encoding: 0xFF = OFF
// bits  0bBA00DC00;
// A: 1 = SELA is output, high or low. 0 = SELA high impedance.
// B: 1 = SELB is output, high or low, 0 = SELB high impedance.
// C: IFF A = 1, specifies high (1) or low (0) for SELA
// D: IFF B = 1, specifies high (1) or low (0) for SELB
// C must be 0 if A = 0, D must be 0 if B = 0 (unless setting is 0xFF (turn off regulator by leaving enable LOW))
#define REG_OFF      0xFF       // Write the active high enable pin LOW to turn off the reg.
#define REG_1V2      0b01000100 // Doesn't work as VDDIO2. SELA high, SELB floating.
#define REG_1V5      0b10001000 // Doesn't work as VDDIO2. SELA floating, SELB high.
#define REG_1V8      0b01000000 // SELA low, SELB floating.
#define REG_2V5      0b00000000 // SELA and SELB floating.
#define REG_3V0      0b11000000 // SELA and SELB low.
#define REG_3V1      0b11000100 // SELA high, SELB low.
#define REG_3V3      0b10000000 // SELA floating, SELB low.
#define REG_4V0      0b11001000 // SELA low, SELB high.
#define REG_5V0      0b11001100 // SELA and SELB high.
#endif
