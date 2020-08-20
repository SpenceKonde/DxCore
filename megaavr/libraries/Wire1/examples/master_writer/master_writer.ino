// Wire1 Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire1 library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire1 Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire1.h>

void setup() {
  Wire1.begin(); // join i2c bus (address optional for master)
}

byte x = 0;

void loop() {
  Wire1.beginTransmission(8); // transmit to device #8
  Wire1.write("x is ");        // sends five bytes
  Wire1.write(x);              // sends one byte
  Wire1.endTransmission();    // stop transmitting

  x++;
  delay(500);
}
