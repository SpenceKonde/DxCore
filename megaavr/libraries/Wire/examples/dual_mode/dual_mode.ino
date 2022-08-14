/* Wire Dual Mode Example
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Writes data to an I2C/TWI slave and
 * Reads data from an I2C/TWI slave device
 *
 * With formatting and portability changes by Spence to support
 * disabled millis, different primary serial ports.
 *
 * The target of this example is to show how to use Dual Mode and to explain what it is.
 * The TWI module basically consists of two parts: Master and Slave. After a reset, both
 * parts are connected to the same pins. But the AVRs offer the Dual Mode, which
 * disconnects the Slave part from the Master pins and connects it to dedicated pins. This
 * is useful if you have different voltage levels, clocks, or multiple devices with a
 * hardcoded address that require different buses.
 * This code initializes the Slave part with the address 0x54 (7-bit format) with two
 * different behaviours depending weather the Master sends or requests data. If data is
 * received by the Slave it is put into the rxBuffer and printed out in the loop. If data
 * is requested, the Slave sends it's millis() value.
 * The Master, depending on the Serial input, either sends data to the address 0x54 or
 * requests from it. When the first char from Serial is an 'r' or 'R', a request is made,
 *  otherwise the Text is send directly to the Slave.
 */


#include "Wire.h"

void srmw (int);
void swmr (void);

#define MySerial Serial       // The serial port connected to the to the computer.

uint8_t txLen = 0;
uint8_t rxLen = 0;
uint8_t txBuffer[32] = {0};
uint8_t rxBuffer[32] = {0};

void setup() {
  Wire.onReceive(receiveDataWire);    // Set the slave receive ISR
  Wire.onRequest(transmitDataWire);   // Set the slave request ISR

  Wire.enableDualMode(true);    // set argument true if FastMode+ speeds are expected on slave
                                // This has to be called before begin(), so that  the library prepares the other pins
  Wire.usePullups();
  //Wire.swap(2);                 // Route TWI pins to an alternative pinout, if necessary.
  //Wire.pins(PIN_PC2, PIN_PC3);  // Note - the pins in the argument are for Host/Client only. There is no pin lookup for Dual mode
                                  // e.g. Wire.pins(PIN_PC6, PIN_PC7) does not work at this point right now! Please use swap() instead.

  Wire.begin();                   // initialize master
  Wire.begin(0x54, false);        // initialize slave

  MySerial.begin(115200);         // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  // put your main code here, to run repeatedly:
  if (MySerial.available() > 0) {   // as soon as the first byte is received on Serial
    readFromSerial();               // read the data from the Serial interface
    if (txLen > 0) {                // after the while-loop, if there was useful data,
      sendDataToWire();             // send the data over I2C
    }
    txLen = 0;                      // since the data was sent, the position is 0 again
  }
  if (rxLen > 0) {
    MySerial.print("Received on Slave pins: ");
    MySerial.write(rxBuffer, rxLen);
    MySerial.println();
    rxLen = 0;
  }
}



void readFromSerial() {
  #ifndef MILLIS_USE_TIMERNONE      // This example will compile and work without millis
  uint32_t timeout = millis();      // but if we have millis(), we grab the current time now
  #endif
  while (true) {
    char c = MySerial.read();       // read the next char
    while (c == -1) {               // when the buffer is empty, Serial.read() returns -1
      c = MySerial.read();          // this avoids filling the input buffer with gibberish
    }

    if (c == '\n' || c == '\r') {   // until a new line or carriage return is found
      break;                        // if so, break the endless while-loop
    }                               // otherwise
    txBuffer[txLen] = c;            // save the char
    txLen++;                        // increment the position
    
    if (txLen > 30) {               // if there is too much data
      break;                        // break the while-loop to avoid buffer overflow
    }

    #ifndef MILLIS_USE_TIMERNONE    // If we have millis and didn't get a newline
    if (millis() - timeout > 30000) {
      // we will give up after 30 seconds, and send what we have.
      MySerial.println("Serial timeout getting command");
      if (txLen > 0) {
        MySerial.println("Sending what we have");
      }
      break;
    }
    #endif
  }
}


void sendDataToWire() {
  uint8_t firstElement = txBuffer[0];
  if (firstElement == 'r' || firstElement == 'R') {   // check if the first element is an 'r' or 'R'
    if (4 == Wire.requestFrom(0x54, 4, 0x01)) {       // request from slave
      while (Wire.available()) {
        uint32_t ms;
        ms  = (uint32_t)Wire.read();                  // read out 32-bit wide data
        ms |= (uint32_t)Wire.read() <<  8;
        ms |= (uint32_t)Wire.read() << 16;
        ms |= (uint32_t)Wire.read() << 24;
        MySerial.println(ms);
      }
    } else {
      MySerial.println("Wire.requestFrom() timed out!");
    }
  } else {
    Wire.beginTransmission(0x54);       // prepare transmission to slave with address 0x54
    Wire.write(txBuffer, txLen);        // Write the received data to the bus buffer
    Wire.endTransmission();             // finish transmission
  }
}



void receiveDataWire(int16_t numBytes) {
  if (numBytes > 30) numBytes = 30; //avoid Bbuffer overflow
  uint8_t i;
  for (i = 0; i < numBytes; i++) {
    rxBuffer[i] = Wire.read();
  }
  rxLen = numBytes;
}

void transmitDataWire() {
  #ifdef MILLIS_USE_TIMERNONE
  uint32_t ms = 1234567890UL; /* We have no milliseconds value to send, so use a super-distinctive placeholder */
  #else
  uint32_t ms = millis();
  #endif
  Wire.write((uint8_t) ms);
  Wire.write((uint8_t)(ms >> 8));
  Wire.write((uint8_t)(ms >> 16));
  Wire.write((uint8_t)(ms >> 24));
}
