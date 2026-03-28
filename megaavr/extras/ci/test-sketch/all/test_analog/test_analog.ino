volatile uint32_t GARBAGE = 0;
void discard(uint32_t g) {
  GARBAGE ^= g;
};
void discard(uint8_t g) {
  discard((uint32_t) g);
};
void discard(uint16_t g) {
  discard((uint32_t) g);
};
void discard(int8_t g) {
  discard((uint32_t) g);
};
void discard(int16_t g) {
  discard((uint32_t) g);
};
void discard(int32_t g) {
  discard((uint32_t) g);
};

volatile uint8_t NOT_A_CONST_BYTE = 0;
volatile uint16_t NOT_A_CONST_WORD = 0;
volatile uint32_t NOT_A_CONST_DWORD = 0;
volatile int8_t NOT_A_CONST_CHAR = 0;
volatile int16_t NOT_A_CONST_INT = 0;
volatile int32_t NOT_A_CONST_LONG = 0;
void loop() {
};

void setup() {
   // Test Everything!
  compile_test_analog();
};

void compile_test_analog() {
  /* Analog Functions */
  /* All parts have PA0, PA1 (these are crystal pins),
   * PC1, PC2, PC3, PD4, PD5, PD6, PD7, PF6 (reset)
   * Of those only PDn are analog inputs on all
   * also known as PIN_A4, PIN_A5, PIN_A6, PIN_A7*/
  /* Verify that references constant and non-constant compile */
  #if defined(DAC0)
    DACReference(INTERNAL1V024);
  #endif
  analogReference(INTERNAL1V024);
  #if defined(DAC0)
    DACReference(INTERNAL2V048);
  #endif
  analogReference(INTERNAL2V048);
  #if defined(DAC0)
    DACReference(INTERNAL4V096);
  #endif
  analogReference(INTERNAL4V096);
  #if defined(DAC0)
    DACReference(INTERNAL2V5);
  #endif
  analogReference(INTERNAL2V5);
  #if defined(DAC0)
    DACReference(DEFAULT);
  #endif
  analogReference(DEFAULT);
  #if defined(DAC0)
    DACReference(VDD);
  #endif
  analogReference(VDD);
  #if defined(DAC0)
    DACReference(EXTERNAL);
  #endif
  analogReference(EXTERNAL);
  #if defined(DAC0)
    DACReference(NOT_A_CONST_BYTE);
  #endif
  analogReference(NOT_A_CONST_BYTE);
  /* set ADC settings */
  uint8_t retval_analogReadResolution = analogReadResolution(10);
  discard(retval_analogReadResolution);
  retval_analogReadResolution = analogReadResolution(12);
  discard(retval_analogReadResolution);
  retval_analogReadResolution = analogReadResolution(NOT_A_CONST_BYTE);
  discard(retval_analogReadResolution);
  uint8_t retval_analogSampleDuration = analogSampleDuration(NOT_A_CONST_BYTE);
  discard(retval_analogSampleDuration);
  /* get + set ADC setting - I wish this was how the others worked! */
  int16_t retval_analogClockSpeed = analogClockSpeed(0); // default
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(-1);    // query
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(10,0);    // too low
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(1000,0);  // valid
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(10000,0); // too high
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(NOT_A_CONST_WORD,0);
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(10, 1);    // too low
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(1000, 1);  // valid
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(10000, 1); // too high
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(NOT_A_CONST_WORD,1);
  discard(retval_analogClockSpeed);
  retval_analogClockSpeed = analogClockSpeed(NOT_A_CONST_WORD, NOT_A_CONST_BYTE);
  discard(retval_analogClockSpeed);
  /* Get ADC settings */
  uint8_t retval_getAnalogReference = getAnalogReference();
  discard(retval_getAnalogReference);
  int8_t retval_getAnalogReadResolution = getAnalogReadResolution();
  discard(retval_getAnalogReadResolution);
  #if defined(DAC0)
    uint8_t retval_getDACReference = getDACReference();
    discard(retval_getDACReference);
  #endif
  uint8_t retval_getAnalogSampleDuration = getAnalogSampleDuration();
  discard(retval_getAnalogSampleDuration);
  // valid constant pins , native resolution
  uint16_t retval_analogRead = analogRead(PIN_A4);
  discard(retval_analogRead);
  int32_t retval_analogReadEnh = analogReadEnh(PIN_A4, ADC_NATIVE_RESOLUTION);
  discard(retval_analogReadEnh);
  #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL > 0
    int32_t retval_analogReadDiff = analogReadDiff(PIN_A4, PIN_A5, ADC_NATIVE_RESOLUTION);
    discard(retval_analogReadDiff);
  #endif
  // Valid constant channels, max oversampling
  retval_analogRead = analogRead(ADC_GROUND);
  discard(retval_analogRead);
  retval_analogReadEnh = analogReadEnh(ADC_GROUND, ADC_MAX_OVERSAMPLED_RESOLUTION);
  discard(retval_analogReadEnh);
  #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL > 0
    retval_analogReadDiff = analogReadDiff(ADC_GROUND, ADC_TEMPERATURE, ADC_MAX_OVERSAMPLED_RESOLUTION);
    discard(retval_analogReadDiff);
  #endif
  // Non-constants pin/channels, minimum resolution
  retval_analogRead = analogRead(NOT_A_CONST_BYTE);
  discard(retval_analogRead);
  retval_analogReadEnh = analogReadEnh(NOT_A_CONST_BYTE, ADC_NATIVE_RESOLUTION_LOW);
  discard(retval_analogReadEnh);
  #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL > 0
    retval_analogReadDiff = analogReadDiff(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, ADC_NATIVE_RESOLUTION_LOW);
    discard(retval_analogReadDiff);
  #endif
  // Non-constants pin/channels, minimum resolution
  retval_analogReadEnh = analogReadEnh(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  discard(retval_analogReadEnh);
  #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL > 0
    retval_analogReadDiff = analogReadDiff(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
    discard(retval_analogReadDiff);
  #endif
  /* End of analog */
};
