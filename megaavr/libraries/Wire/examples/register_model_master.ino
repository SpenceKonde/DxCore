/* Wire Register Model
 * by Spence Konde
 *
 * Demonstrates a master interacting with a wire slave implementing the register model
 * 0-4 are fully writable
 * the 5 low bits of 5 are writable
 * 6 and 7 are fully writable,
 * 8-11 only allow the 2 low bits in each nybble to be written
 * 12-15 are read-only
 * 16 and 17 allow only the low nybble to be written
 * 18 and 19 allow only the high nybble to be written.
 *
 * The address pointer autoincrements and wraps around.
 *
 * There are two special registers, 4 and 5.
 * (chosen mostly at random, just to demonstrate making things in loop depend on registers)
 * 4 and 5 are the low and high bytes of the time delay used for the blinking... So by default it's 0x0504 ms
 * or 1284 ms.
 */
#include <Wire.h>

#define MySerial Serial

void setup() {
  MySerial.swap(1);
  MySerial.begin(115200);
  MySerial.println("Hil, now to use a register model slave");
  Wire.begin();
}

void setAddressPointer(uint8_t address) {
  Wire.beginTransmission(0x69);    // prepare transmission to slave with address 0x69
  Wire.write(address);            // Write just the address
  Wire.endTransmission();
}

void loop() {
  MySerial.println("going to write to 0, then make a series of reads");
  setAddressPointer(0);
  if (0 != Wire.requestFrom(0x69,8)){
    while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
      MySerial.print(' ');
    }
  }
  delay(1000);
  MySerial.println("that was 8 bytes");
  Wire.requestFrom(0x69,12);
  while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
    MySerial.print(' ');
  }
  MySerial.println("that was 12 bytes more");
  delay(1000);
  MySerial.println("Now, let's request a whopping 32, the whole shebang");

  Wire.requestFrom(0x69,32);
  while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
    MySerial.print(' ');
  }
  MySerial.println("Now that was cool, no?");
  delay(1000);
  MySerial.println("Let's demo write protect in action");
  Wire.beginTransmission(0x69);    // prepare transmission to slave with address 0x69
  Wire.write(0x10);            // Write just the address
  Wire.write(0xEE);            // Write a value
  Wire.write(0xDD);            // Write a value
  Wire.write(0xCC);            // Write a value
  Wire.write(0xBB);            // Write a value
  Wire.write(0xFF);            // Write a value
  Wire.endTransmission();
  MySerial.println("Read-em-back:");
  setAddressPointer(16);
  Wire.requestFrom(0x69,5);
  while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
    MySerial.print(' ');
  }
  delay(1000);
  MySerial.println("Sick! Now finally.... That LED blinks mad slow, I want something more energetic!");
  Wire.beginTransmission(0x69);    // prepare transmission to slave with address 0x69
  Wire.write(0x4);            // Write just the address
  Wire.write(0x80);            //Write a value
  Wire.write(0x01);           // Write a value: ~3/8ths of a second!
  Wire.endTransmission();
  MySerial.println("Boooooyah!!");
  delay(30000);

}
