// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <tinyNeoPixel_Static.h>


// You can use any I/O pin that is not being overridden by some peripheral for either purpose.
// No pin is inherently better or worse than any other for either of these purposes; it's all about what
// other things you need pins for, and whether any of them are picky about which pins are used.

#if _AVR_PINCOUNT == 14 // need to use a different pins on DD14
  #define BUTTON_PIN    PIN_PD5    // Digital IO pin connected to the button.  This will be
  // driven with a pull-up resistor so the switch should
  // pull the pin to ground momentarily.  On a high -> low
  // transition the button press logic will execute.
  #define PIXEL_PIN    PIN_PD4
#else
  #define BUTTON_PIN    PIN_PA2
  #define PIXEL_PIN     PIN_PA3    // Digital IO pin connected to the NeoPixels
#endif

#define PIXEL_COUNT 16


// Since this is for the static version of the library, we need to supply the pixel array
// This saves space by eliminating use of malloc() and free(), and makes the RAM used for
// the frame buffer show up when the sketch is compiled.

byte pixels[PIXEL_COUNT * 3];
// It is strongly recommended to have one variable or #define for the number of pixels, and then
// multiply that by 3 (for RGB) or 4 (for RGBW) when declaring the pixel array, as is done here
// to avoid the need for double-entry bookkeeping.


// Parameter 1 = number of pixels in strip,
// Parameter 2 = pin number (most are valid)
// Parameter 3 = color order (NEO_RGB, NEO_GRB, etc).
// Parameter 4 = pixel buffer declared above
// Unlike the Adafruit library there's no 400 kHz option. I don't think I have ever seen 400 kHz pixels, and they may no longer be made.
// Because the optimizer's hands are tied when working with classes it would impose undue burdens on all users to support it, plus it would
// need the asm routines adapted to every supported clock speed... all for the sake of a part that seems extinct in the wild.

tinyNeoPixel strip = tinyNeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB, pixels);

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIXEL_PIN, OUTPUT);
  // strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 9) {
        showType = 0;
      }
      startShow(showType);
    }
  }

  // Set the last button state to the old state.
  oldState = newState;
}

void startShow(int i) {
  switch (i) {
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
      break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
      break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
      break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
      break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
      break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
      break;
    case 6: theaterChase(strip.Color(0,   0, 127), 50);   // Blue
      break;
    case 7: rainbow(20);
      break;
    case 8: rainbowCycle(20);
      break;
    case 9: theaterChaseRainbow(50);
      break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  // turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      // turn every third pixel off
      }
    }
  }
}

// Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel((i + j) % 255)); // turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      // turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
