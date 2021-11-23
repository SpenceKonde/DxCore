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
  compile_test_tone();
};

void compile_test_tone() {
  /* Tone */
  noTone(PIN_PC1);
  tone(PIN_PC1, 25000);
  tone(PIN_PC2, 500, 12345);
  tone(PIN_PC3, 23232, NOT_A_CONST_DWORD);
  tone(PIN_PD4, NOT_A_CONST_WORD);
  tone(PIN_PD5, NOT_A_CONST_WORD, 12345);
  tone(PIN_PD6, NOT_A_CONST_WORD, NOT_A_CONST_DWORD);
  // tone non-constant pins
  noTone(NOT_A_CONST_BYTE);
  tone(NOT_A_CONST_BYTE, 25000);
  tone(NOT_A_CONST_BYTE, 500, 12345);
  tone(NOT_A_CONST_BYTE, 23232, NOT_A_CONST_DWORD);
  tone(NOT_A_CONST_BYTE, NOT_A_CONST_WORD);
  tone(NOT_A_CONST_BYTE, NOT_A_CONST_WORD, 12345);
  tone(NOT_A_CONST_BYTE, NOT_A_CONST_WORD, NOT_A_CONST_DWORD);
};
