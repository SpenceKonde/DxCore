#ifndef AZDUINO_MAX38903_H
#define AZDUINO_MAX38903_H
#include <Arduino.h>
int8_t setMAX38903Voltage(uint8_t setting);
#define REG_OFF      0xFF       // Write the active high enable pin LOW to turn off the reg.
#define REG_1V2      0b01000100 // Doesn't work as VDDIO2
#define REG_1V5      0b10001000 // Doesn't work as VDDIO2
#define REG_1V8      0b01000000 //
#define REG_2V5      0b00000000 //
#define REG_3V0      0b11000000 //
#define REG_3V1      0b11000100 //
#define REG_3V3      0b10000000 //
#define REG_4V0      0b11001000 //
#define REG_5V0      0b11001100 //
#endif
