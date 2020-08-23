// Wire1 Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire1 library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire1 Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire1.h>

void setup() {
  Wire1.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire1.requestFrom(8, 6);    // request 6 bytes from slave device #8

  while (Wire1.available()) { // slave may send less than requested
    char c = Wire1.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  delay(500);
}
