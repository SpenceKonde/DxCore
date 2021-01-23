#include <flashWrite.h>
#include <avr/pgmspace.h>
/* Okay - so there are two versions of this library. All of their functionality and implementation is identical!
 * The question is which makes for a better user experience!
 * This is the "Non-class-oriented" implementation - a bunch of straight up functions.
 * Note also that the calling conventions of these funtions are separate issues from whether
 * they are presents as members of Flash (class oriented) or independent functions
 * (this one, the non-class-oriented).
 *
 * So - please... Feedback Wanted! :-)
 */


// Base address to do our writing demos in. This way we will use the same area for all the demos.
// In all cases, this picks the highest 1/4th of the flash.
#if (PROGMEM_SIZE == 0x10000) // AVR64Dx
// Default mapped flash is second half (0x8000-0xFFFF)
// This starts right in the middle.
#define BASE_ADDRESS 0xC000
#elif (PROGMEM_SIZE == 0x20000) // AVR128Dx
// Default mapped flash is the last quarter of the flash - this starts at bottom of mapped flash
#define BASE_ADDRESS (0x18000)
#elif (PROGMEM_SIZE == 0x8000) // AVR32Dx
//Everything is mapped here, but as described we use last 1/4th of it
#define BASE_ADDRESS 0x6000
#else
// Well, there's only one possibility left... that the AVR16DD is now out
// and you're using one of those! Start at 12k in.
#define BASE_ADDRESS 0x3000
#endif

//Basic Flash.h demo

const uint16_t WordArray[] {0xDEAD, 0xBEEF, 0x0000, 0xCAFE, 0xBABE};
const uint8_t ByteArray[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

// Connect a button to ground or a dangling wire you can touch
// to ground to PA6 (pin 6) to re-run the demo (Pin PD6, Pin 7 on
// 14-pin DD-series)


#ifdef DD_14_PINS
#define BUTTON_PIN PIN_PD6
#else
#define BUTTON_PIN PIN_PA6
#endif

uint8_t pgm_read_byte(uint32_t address) {
  #if PROGMEM_SIZE >0x10000
    return pgm_read_byte_far(address);
  #else
    return pgm_read_byte_near(address);
  #endif
}
// How annoying! To read from the mapped flash by non-mapped means, with it in the normal place, we need to use pgm_read_byte_far().
// But the moment we switch to a smaller chip, they're not defined....

uint16_t pgm_read_word(uint32_t address) {
  #if PROGMEM_SIZE >0x10000
    //128k parts use the _far version, uses elpm instruction - but works fine for the near space too.
    return pgm_read_word_far(address);
  #else
    return pgm_read_word_near(address); //address gets t
  #endif
}


void setup() {

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  // Blink the LED for good measure too - pin PA7 (pin 7) except
  // on 14-pin DD-series, where it's PD7 (Pin 8)
  Serial.swap(1);
  Serial.begin(115200);
  // Give the user a chance to connect to the serial port
  for (byte i = 3; i; i--) {
    digitalWrite(LED_BUILTIN, CHANGE);
    Serial.print('.'); // Show something so it doesn't look hung
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println(F("Checking if flash is writable: "));
  byte flashcheck = flashCheckWritable();
  Serial.println(flashcheck);
  if (flashcheck) {
    Serial.println(F("It's not :-( Nothing we can do here!"));
    while (1);
  }
  Serial.println(F("At startup, the half-a-page of flash we will be working on looks like:"));
  readHalfPage(BASE_ADDRESS);
}

void loop() {

  // first, make sure we haven't run so we don't sit there eating flash endurance
  Serial.println(pgm_read_word(0x180FE), HEX);
  if (pgm_read_word(BASE_ADDRESS + 0xFE) == 0x0001) { //For this demo, first page after base
    Serial.println(F("Demo has already run"));
    digitalWrite(LED_BUILTIN, HIGH);
    byte buttonstate = digitalRead(BUTTON_PIN);
    if (buttonstate == LOW) {
      Serial.println(F("Un-"));
    }
    Serial.println(F("Ground Pin PA6 to run again."));
    while (buttonstate==digitalRead(BUTTON_PIN)); // loop unless/until pin state changes.
  }
  Serial.println(F("Flash.h demo - non-0 status codes are errors"));
  delay(500);
  Serial.print(F("Erasing: "));
  Serial.print(flashErasePage(BASE_ADDRESS));
  Serial.println(F("See, it is now empty"));
  readHalfPage(BASE_ADDRESS);
  Serial.println();
  Serial.println(F("Writing a word, a byte, an array of words, an array of bytes - status:"));
  Serial.println();
  Serial.println(F("Word to base + 1 fails - Flash.h requires words to be aligned"));
  Serial.printHex(flashWriteWord(BASE_ADDRESS + 1, 0x1234));
  Serial.println();
  Serial.println(F("Byte to base + 1 allowed by Flash.h ->"));
  Serial.printHex(flashWriteByte(BASE_ADDRESS + 1, 0x01));
  Serial.println();
  /*  Why did I decide to only allow bytes to be written unaligned? Well, in order to dodge the errata on some of the 128k parts
   *  we can only use SPM to write. SPM writes a word, not a byte. The only way to write a byte is to set on e half of a word to
   *  that byte, and the other byte of the word to 0xFF, and write that. Since writes can't turn 0's back into 1s, only 1s into
   *  0s (you need an erase of the page to turn 0s back into 1s), that will leave whatever is in the other half of the byte
   *  unchanged. with larger datatypes, you can just align them, but if you're writing independent bytes, well, I'm not going to
   *  ask you to leave half the space you're using empty...
   */


  Serial.println(F("Word to base + 2 status ->"));
  Serial.printHex(flashWriteWord(BASE_ADDRESS + 2, 0x4523));
  Serial.print(' ');
  Serial.printHex(flashWriteWords(BASE_ADDRESS + 0x06, WordArray, 5));
  Serial.print(' ');
  // If we have an array of something other than words to write - cast it to an word pointer.
  uint16_t * wordptr = (uint16_t *) ByteArray;
  // but be sure to adjust the length! we had 20 bytes, so we have 10 words.
  Serial.printHex(flashWriteWords(BASE_ADDRESS + 0x10, wordptr, 10));
  Serial.println();
  delay(500);
  readBackDemo();
  Serial.println(F("Notice how bytes within each word are swapped if you started with bytes but wrote as words or vice versa"));
  Serial.println(F("computers put low byte first, wikipedia 'endianness' - though it's not specific to flash. It's just"));
  Serial.println(F("an easy place to see it; see - the byte array we cast to words is like that too:"));
  Serial.println(wordptr[0],HEX);
  delay(500);
  Serial.println(F("Well, that's writing to flash from app - now, we'll write 1 to BASE_ADDRESS+0xFE"));
  Serial.println(F("so we don't run repeatedly until your flash wears out."));
  Serial.println(F("And we dump the whole half-page again, too"));
  flashWriteWord(BASE_ADDRESS + 0xFE, 0x0001);
  readHalfPage(BASE_ADDRESS);
  delay(60000);
}


void readHalfPage(uint32_t StartAddress) {
  StartAddress &= 0x0001FF00;
  for (unsigned long i = StartAddress; i < (StartAddress + 0x100); i++) {
    if ((i & 0xFF) == 0) {
      Serial.println(i, HEX);
    }
    Serial.printHex(pgm_read_byte(i));
    if ((i & 31) != 31) {
      //Print 31 bytes per line.
      Serial.print(' ');
    } else {
      Serial.println();
    }
  }
  Serial.println();
}

void readBackDemo() {
  Serial.println(F("Reading word, byte, word array back via pgm as bytes"));
  for (byte i = 0; i < 16; i++) {
    if (i) Serial.print(':'); //Trick to put the : between the values
    Serial.print(pgm_read_byte(BASE_ADDRESS + i), HEX);
  }
  Serial.println();
  delay(500);
  Serial.println(F("Reading word, byte, word array back via pgm as words"));
  for (byte i = 0; i < 16; i += 2) {
    if (i) Serial.print(':');
    Serial.print(pgm_read_word(BASE_ADDRESS + i), HEX);
  }
  Serial.println();
  Serial.println();
  Serial.println();
  delay(500);
  Serial.println(F("and our byte array - as bytes, base 10:"));
  for (byte i = 0; i < 20; i++) {
    if (i) Serial.print(':');
    Serial.print(pgm_read_byte(BASE_ADDRESS + 0x10 + i));
  }
  Serial.println();
  Serial.println(F("and our byte array - as words, base 10:"));
  delay(500);
  for (byte i = 0; i < 10; i++) { //here instead of incrementing i by 2
    if (i) Serial.print(':');
    Serial.print(pgm_read_word(BASE_ADDRESS + 0x10 + i * 2)); //multiply i by 2 down here
  }
  Serial.println();
  Serial.println(F("Whaaa? Okay how bout we try again as hex!"));
  for (byte i = 0; i < 20; i++) {
    if (i) Serial.print(':');
    Serial.print(pgm_read_byte(BASE_ADDRESS + 0x10 + i),HEX);
  }
  Serial.println();
  delay(500);
  for (byte i = 0; i < 10; i++) { //here instead of incrementing i by 2
    if (i) Serial.print(':');
    Serial.print(pgm_read_word(BASE_ADDRESS + 0x10 + i * 2),HEX); //multiply i by 2 down here
  }
  Serial.println();
  Serial.println(F("Phew - I mean kind of... at least I can see where it came from!!"));
  Serial.println();
  Serial.println();
  delay(500);
  Serial.println(F("Finally, read that whole 36-byte mess back via mapped flash..."));
  // Mapped flash starts at 0x8000 and goes to 0xFFFF.
  // If you know your address is in the mapped area of flash currently (which we know it is here
  // see top of sketch - after reset these always have the FLMAP bits pointing towards the
  // highest section), a bitwise or with 0x8000 will get you the mapped address, regardless of
  // the part or current FLMAP settings (if applicable for your part - neat trick :-)
  // Once you figure the number, cast it to an appropriate pointer or the compiler will warn you.
  // You want the warnings you DO get to indicate *problems*
  uint8_t *bytePtr=(uint8_t*)(BASE_ADDRESS | 0x8000);
  for (byte i = 0; i < 36; i++) {
    if (i) Serial.print(':');
    Serial.print(*bytePtr++,HEX);
  }
  Serial.println();
  delay(500);
}
