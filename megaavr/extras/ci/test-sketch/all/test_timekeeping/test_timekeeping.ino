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
  compile_test_timekeeping();
};

void compile_test_timekeeping() {
  #ifndef MILLIS_USE_TIMERNONE
    stop_millis();
    restart_millis();
    set_millis(12345);
    set_millis(NOT_A_CONST_DWORD);
    uint32_t retval_millis = millis();
    discard(retval_millis);
    uint32_t retval_micros = micros();
    discard(retval_micros);
    // pulsein with micros
    uint32_t retval_pulseInLong = pulseInLong(NOT_A_CONST_BYTE, HIGH, NOT_A_CONST_DWORD);
    discard(retval_pulseInLong);
    retval_pulseInLong = pulseInLong(NOT_A_CONST_BYTE, LOW, NOT_A_CONST_DWORD);
    discard(retval_pulseInLong);
    retval_pulseInLong = pulseInLong(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_DWORD);
    discard(retval_pulseInLong);
  #endif
  uint16_t retval_clockCyclesPerMicrosecond = clockCyclesPerMicrosecond();
  discard(retval_clockCyclesPerMicrosecond);
  uint32_t retval_clockCyclesToMicroseconds = clockCyclesToMicroseconds(NOT_A_CONST_DWORD);
  discard(retval_clockCyclesToMicroseconds);
  uint32_t retval_microsecondsToClockCycles = microsecondsToClockCycles(NOT_A_CONST_DWORD);
  discard(retval_microsecondsToClockCycles);
  // pulsein counting clocks

  uint32_t retval_pulseIn = pulseIn(PIN_PC1, HIGH, 100000);
  discard(retval_pulseIn);
  retval_pulseIn = pulseIn(NOT_A_CONST_BYTE, LOW, NOT_A_CONST_DWORD);
  discard(retval_pulseIn);
  retval_pulseIn = pulseIn(NOT_A_CONST_BYTE, HIGH, NOT_A_CONST_DWORD);
  discard(retval_pulseIn);
  retval_pulseIn = pulseIn(NOT_A_CONST_BYTE, NOT_A_CONST_BYTE, NOT_A_CONST_DWORD);
  discard(retval_pulseIn);

  delay(1234);
  delay(NOT_A_CONST_DWORD);
  delayMicroseconds(2345);
  delayMicroseconds(NOT_A_CONST_WORD);
};
