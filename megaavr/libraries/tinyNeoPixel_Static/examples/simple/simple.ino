// NeoPixel simple sketch (c) 2013 Shae Erisson, adapted to tinyNeoPixel library by Spence Konde 2019.
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <tinyNeoPixel_Static.h>


// Which pin on the Arduino is connected to the NeoPixels?
  #define PIN    PIN_PA6    // need to use a different pin on DD14

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      400

// Since this is for the static version of the library, we need to supply the pixel array
// This saves space by eliminating use of malloc() and free(), and makes the RAM used for
// the frame buffer show up when the sketch is compiled.

byte pixels[NUMPIXELS * 3];

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

tinyNeoPixel leds = tinyNeoPixel(NUMPIXELS, PIN, NEO_RGB, pixels);

int delayval = 500; // delay for half a second

void setup() {
  pinModeFast(PIN_PC4, INPUT_PULLUP);
  Serial.swap(1);
  Serial.begin(115200);
  delay(100);
  Serial.println(analogReadEnh(PIN_PC4,15)); 
  Serial.println("testing");
  pinModeFast(PIN, OUTPUT);
  // with tinyNeoPixel_Static, you need to set pinMode yourself. This means you can eliminate pinMode()
  // and replace with direct port writes to save a couple hundred bytes in sketch size (note that this
  // savings is only present when you eliminate *all* references to pinMode).
  // leds.begin() not needed on tinyNeoPixel
  leds.show();
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for (int i = 0; i < 100; i++) {
    if (i % 50 != 49) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      leds.setPixelColor(i, leds.Color(255, 255, 255)); // Moderately bright green color.

      leds.show(100); // This sends the updated pixel color to the hardware.

      delay(30); // Delay for a period of time (in milliseconds).
    } else {
      leds.setPixelColor(i, leds.Color(5,0,0));
      leds.show(100);
      delay(30);
    }
    
  Serial.println(analogReadEnh(PIN_PC1,15)); 
  }
  delay(2000);
  
  Serial.println(analogReadEnh(PIN_PC1,15)); 
  for (int i = 0; i < 100; i++) {
    if (i % 50 != 49) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      leds.setPixelColor(i, leds.Color(i < 100? 5:0 , i>100? 5:0, i>200? 5:0)); // Moderately bright green color.

      leds.show(100); // This sends the updated pixel color to the hardware.

      delay(30); // Delay for a period of time (in milliseconds).
    } else {
      leds.setPixelColor(i, leds.Color((i > 100)?0:255,i>100?255:0,i>200?255:0));
      leds.show(100);
      delay(30);
      
  Serial.println(analogReadEnh(PIN_PC5,15)); 
    }
  }
  // with tinyNeoPixel_Static, since we have the pixel array, we can also directly manipulate it - this sacrifices the correction for the pixel order, and the clarity of setColor to save a tiny amount of flash and time.
  for (int i = 0; i < (300); i++) {
    pixels[i] = 150; // set byte i of array (this is channel (i%3) of led (i/3) (respectively, i%4 and i/4 for RGBW leds)
    leds.show(100); // show
    delay(30); // delay for a period of time
    pixels[i] = 0; // turn off the above pixel
    // result is that each pixel will cycle through each of the primary colors (green, red, blue for most LEDs) in turn, and only one LED will be on at a time.
  }
}
