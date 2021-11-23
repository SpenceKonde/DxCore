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
  compile_test_PWM();
};
void compile_test_PWM() {
  analogWrite(NOT_A_CONST_BYTE, 0);
  analogWrite(NOT_A_CONST_BYTE, 128);
  analogWrite(NOT_A_CONST_BYTE, 255);
  analogWrite(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  #if defined(TCA0) && !defined(MILLIS_USE_TIMERA0)
    takeOverTCA0();
    resumeTCA0();
  #endif
  uint8_t retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_TCA0_WO4_ALT3);  // should always be defined
  discard(retval_digitalPinToTimerNow);
  analogWrite(PIN_TCA0_WO4_ALT3, 0);
  analogWrite(PIN_TCA0_WO4_ALT3, 128);
  analogWrite(PIN_TCA0_WO4_ALT3, 255);
  analogWrite(PIN_TCA0_WO4_ALT3, NOT_A_CONST_BYTE);
  retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_TCA0_WO0_DEFAULT);  // should always be defined
  discard(retval_digitalPinToTimerNow);
  #if defined(TCA1) && !defined(MILLIS_USE_TIMERA1)
    takeOverTCA1();
    resumeTCA1();
    retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_TCA1_WO0_DEFAULT);
    discard(retval_digitalPinToTimerNow);
    analogWrite(PIN_TCA1_WO0_DEFAULT, 0);
    analogWrite(PIN_TCA1_WO0_DEFAULT, 128);
    analogWrite(PIN_TCA1_WO0_DEFAULT, 255);
    analogWrite(PIN_TCA1_WO0_DEFAULT, NOT_A_CONST_BYTE);
  #endif
  analogWrite(PIN_PC1, 0);
  analogWrite(PIN_PC1, 128);
  analogWrite(PIN_PC1, 255);
  analogWrite(PIN_PC1, NOT_A_CONST_BYTE);
  #ifdef TCD0
    #if !defined __AVR_DD__
        retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_TCD0_WOD_DEFAULT);
        discard(retval_digitalPinToTimerNow);
      analogWrite(PIN_TCD0_WOA_DEFAULT, 0);
      analogWrite(PIN_TCD0_WOB_DEFAULT, 128);
      analogWrite(PIN_TCD0_WOC_DEFAULT, 255);
      analogWrite(PIN_TCD0_WOD_DEFAULT, NOT_A_CONST_BYTE);
    #else
      retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_TCD0_WOD_ALT4);
      discard(retval_digitalPinToTimerNow);
      analogWrite(PIN_TCD0_WOC_ALT4, 0);
      analogWrite(PIN_TCD0_WOD_ALT4, 128);
      analogWrite(PIN_TCD0_WOC_ALT4, 255);
      analogWrite(PIN_TCD0_WOD_ALT4, NOT_A_CONST_BYTE);
    #endif
    takeOverTCD0();
  #endif
  bool retval_digitalPinHasPWM = digitalPinHasPWM(NOT_A_CONST_BYTE);
  discard(retval_digitalPinHasPWM);
  retval_digitalPinHasPWM = digitalPinHasPWM(PIN_TCA0_WO0_DEFAULT);
  discard(retval_digitalPinHasPWM);
  retval_digitalPinHasPWM = digitalPinHasPWM(PIN_TCA0_WO4_ALT3);
  discard(retval_digitalPinHasPWM);
  retval_digitalPinHasPWM = digitalPinHasPWM(PIN_PF6);
  discard(retval_digitalPinHasPWM);
  retval_digitalPinToTimerNow = digitalPinToTimerNow(NOT_A_PIN);
  discard(retval_digitalPinToTimerNow);
  retval_digitalPinToTimerNow = digitalPinToTimerNow(NOT_A_CONST_BYTE);
  discard(retval_digitalPinToTimerNow);
  retval_digitalPinToTimerNow = digitalPinToTimerNow(PIN_PF6);              // Never has PWM
  discard(retval_digitalPinToTimerNow);
  bool retval_digitalPinHasPWMNow = digitalPinHasPWMNow(NOT_A_CONST_BYTE);  // doesn't need more rigorous testing - all it does is call the above and check that it's non-zero.
  discard(retval_digitalPinHasPWMNow);
};
