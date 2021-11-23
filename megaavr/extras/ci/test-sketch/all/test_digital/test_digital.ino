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
  compile_test_digital();
  compile_test_digital_advanced();
};

void compile_test_digital() {
    /* Digital pins */
    /* All parts have PA0, PA1 (these are crystal pins),
     * PC1, PC2, PC3, PD4, PD5, PD6, PD7, PF6 (reset)
     * Of those only PDn are analog inputs on all*/
  // fast digital I/O - constant pins
  digitalWriteFast(PIN_PC1, HIGH);
  digitalWriteFast(PIN_PC2, LOW);
  digitalWriteFast(PIN_PC3, CHANGE);
  digitalWriteFast(PIN_PF6, NOT_A_CONST_BYTE);
  openDrainFast(PIN_PD4, HIGH);
  openDrainFast(PIN_PD5, LOW);
  openDrainFast(PIN_PD6, CHANGE);
  openDrainFast(PIN_PD7, NOT_A_CONST_BYTE);
  int8_t retval_digitalReadFast = digitalReadFast(PIN_PC2);
  discard(retval_digitalReadFast);
  // slow digital I
  int8_t retval_digitalRead = digitalReadFast(PIN_PC2);
  discard(retval_digitalRead);
  retval_digitalRead = digitalRead(NOT_A_CONST_BYTE);
  discard(retval_digitalRead);
  // slow digital O - constant pins
  digitalWrite(PIN_PC1, HIGH);
  digitalWrite(PIN_PC2, LOW);
  digitalWrite(PIN_PC3, CHANGE);
  digitalWrite(PIN_PF6, NOT_A_CONST_BYTE);
  openDrain(PIN_PD4, FLOATING);
  openDrain(PIN_PD5, LOW);
  openDrain(PIN_PD6, CHANGE);
  openDrain(PIN_PD7, NOT_A_CONST_BYTE);
  pinMode(PIN_PD4, INPUT);
  pinMode(PIN_PC2, OUTPUT);
  pinMode(PIN_PD6, INPUT_PULLUP);
  pinMode(PIN_PF6, NOT_A_CONST_BYTE);
  // slow digital O non-constant pins
  digitalWrite(NOT_A_CONST_BYTE, HIGH);
  digitalWrite(NOT_A_CONST_BYTE, LOW);
  digitalWrite(NOT_A_CONST_BYTE, CHANGE);
  digitalWrite(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  openDrain(NOT_A_CONST_BYTE, FLOATING);
  openDrain(NOT_A_CONST_BYTE, LOW);
  openDrain(NOT_A_CONST_BYTE, CHANGE);
  openDrain(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  pinMode(NOT_A_CONST_BYTE, INPUT);
  pinMode(NOT_A_CONST_BYTE, OUTPUT);
  pinMode(NOT_A_CONST_BYTE, INPUT_PULLUP);
  pinMode(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  // turnOffPWM already called by digitalWrite()
};
void compile_test_digital_advanced() {
  // pinConfigure constant pin
  pinConfigure(PIN_PC2,          PIN_ISC_ENABLE);
  pinConfigure(PIN_PC3,          NOT_A_CONST_WORD);
  pinConfigure(PIN_PD4,          (PIN_ISC_LEVEL    | PIN_PULLUP_TGL  | PIN_OUT_CLR));
  pinConfigure(PIN_PD5,          (PIN_ISC_CHANGE   | PIN_DIR_SET     | PIN_PULLUP_CLR  | PIN_INLVL_CLR));
  pinConfigure(PIN_PD6,          (PIN_ISC_RISE     | PIN_INVERT_SET  | PIN_INVERT_TGL  | PIN_DIR_CLR));
  pinConfigure(PIN_PD7,          (PIN_ISC_FALL     | PIN_PULLUP_SET  | PIN_OUT_TGL     | PIN_DIR_TGL));
  pinConfigure(PIN_PF6,          (PIN_ISC_DISABLE  | PIN_INVERT_CLR  | PIN_INLVL_SET   | PIN_OUT_SET));
  // pinConfigure non constant pin
  pinConfigure(NOT_A_CONST_BYTE, PIN_ISC_ENABLE);
  pinConfigure(NOT_A_CONST_BYTE, NOT_A_CONST_WORD);
  pinConfigure(NOT_A_CONST_BYTE, (PIN_ISC_LEVEL    | PIN_PULLUP_TGL  | PIN_OUT_CLR));
  pinConfigure(NOT_A_CONST_BYTE, (PIN_ISC_CHANGE   | PIN_DIR_SET     | PIN_PULLUP_CLR  | PIN_INLVL_CLR));
  pinConfigure(NOT_A_CONST_BYTE, (PIN_ISC_RISE     | PIN_INVERT_SET  | PIN_INVERT_TGL  | PIN_DIR_CLR));
  pinConfigure(NOT_A_CONST_BYTE, (PIN_ISC_FALL     | PIN_PULLUP_SET  | PIN_OUT_TGL     | PIN_DIR_TGL));
  pinConfigure(NOT_A_CONST_BYTE, (PIN_ISC_DISABLE  | PIN_INVERT_CLR  | PIN_INLVL_SET   | PIN_OUT_SET));
  // Shifty I/O
  shiftOut(PIN_PD4, PIN_PD5, MSBFIRST, 0x12);
  shiftOut(NOT_A_CONST_BYTE, PIN_PD6, LSBFIRST, 0x34);
  shiftOut(PIN_PD7, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, 0x56);
  shiftOut(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, 0x78);
  shiftOut(NOT_A_CONST_BYTE, PIN_PD4, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  shiftOut(PIN_PD6, NOT_A_CONST_BYTE, LSBFIRST, NOT_A_CONST_BYTE);
  shiftOut(NOT_A_CONST_BYTE, PIN_PD7, MSBFIRST, NOT_A_CONST_BYTE);
  shiftOut(NOT_A_CONST_BYTE, PIN_PD7, MSBFIRST, NOT_A_CONST_BYTE);
  shiftOut(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  uint8_t retval_shiftIn = shiftIn(PIN_PD4, PIN_PD5, MSBFIRST);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(PIN_PD4, PIN_PD6, LSBFIRST);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(PIN_PD7, NOT_A_CONST_BYTE, MSBFIRST);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, LSBFIRST);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(PIN_PD5, PIN_PD4, NOT_A_CONST_BYTE);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(NOT_A_CONST_BYTE, PIN_PD6, NOT_A_CONST_BYTE);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(PIN_PD7, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  discard(retval_shiftIn);
  retval_shiftIn = shiftIn(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_BYTE);
  discard(retval_shiftIn);

};
