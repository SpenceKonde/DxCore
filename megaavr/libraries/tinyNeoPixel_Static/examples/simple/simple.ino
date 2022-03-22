// NeoPixel simple sketch (c) 2013 Shae Erisson, adapted to tinyNeoPixel library by Spence Konde 2019.
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <tinyNeoPixel_Static.h>


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN_PIXEL      PIN_PF5
#define PIN_REDEN      PIN_PF1
#define PIN_BLUEN      PIN_PF2
#define PIN_GRNEN      PIN_PF3
#define PIN_BRIGHT     PIN_PF4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      500

// Since this is for the static version of the library, we need to supply the pixel array
// This saves space by eliminating use of malloc() and free(), and makes the RAM used for
// the frame buffer show up when the sketch is compiled.

byte pixels[NUMPIXELS * 3];

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

tinyNeoPixel leds = tinyNeoPixel(NUMPIXELS, PIN, NEO_GRB, pixels);

int delayval = 500; // delay for half a second

void setup() {
  pinMode(PIN_PIXEL, OUTPUT);
  pinMode(PIN_REDEN, INPUT_PULLUP);
  pinMode(PIN_GRNEN, INPUT_PULLUP);
  pinMode(PIN_BLUEN, INPUT_PULLUP);
  pinMode(PIN_BRIGHT, INPUT_PULLUP);
  analogReadResolution(12);
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  uint8_t brightness = analogRead(PIN_BRIGHT) >> 4; // scale 12-bit value to 8 bits
  for (int i = 0; i < NUMPIXELS; i++) {
    
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    leds.setPixelColor(i, leds.Color(digitalReadFast(PIN_REDEN) ? 0 : brightness ,digitalReadFast(PIN_GRNEN) ? 0: brightness, digitalReadFast(PIN_BLUEN ? 0 : brightness))); // Moderately bright green color.

    leds.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  // with tinyNeoPixel_Static, since we have the pixel array, we can also directly manipulate it - this sacrifices the correction for the pixel order, and the clarity of setColor to save a tiny amount of flash and time.
  for (int i = 0; i < (NUMPIXELS * 3); i++) {
    pixels[i] = 150; // set byte i of array (this is channel (i%3) of led (i/3) (respectively, i%4 and i/4 for RGBW leds)
    leds.show(); // show
    delay(delayval); // delay for a period of time
    pixels[i] = 0; // turn off the above pixel
    // result is that each pixel will cycle through each of the primary colors (green, red, blue for most LEDs) in turn, and only one LED will be on at a time.
  }
}
