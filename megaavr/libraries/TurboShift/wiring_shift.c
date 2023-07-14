#include <Arduino.h>

inline uint8_t turboShiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  if (!(__builtin_constant_p(dataPin) && __builtin_constant_p(clockPin))) {
    badArg("Like Fast I/O, the pins used must known at compile time");
    return 0;
  } else {
    if (digitalPinToPort(dataPin) != digitalPinToPort(clockPin)) {
      badArg("The two pins used need to be on the same port for efficient implementation");
      return 0;
    }
    uint8_t p = digitalPinToPort(dataPin);
    p << 2;
    if (bitOrder == MSBFIRST) {
      return _turboShiftIn_M(vportbase, digitalPinToBitMask(dataPin), digitalPinToBitMask(clockPin))
    } else {
      return _turboShiftIn_L(vportbase, digitalPinToBitMask(dataPin), digitalPinToBitMask(clockPin))
    }
  }
}
inline uint8_t _turboShiftIn_M(uint8_t vp, uint8_t dp, uint8_t cp) {
  uint8_t result = 0;
  uint8_t bits = 8;
  __asm__ __volatile__(
    "sbi %5, %3"
    "rjmp .+0"
    "sbic %5, %2"
     "sec"
    "cbi %5, %3"
    "rol, %0"
    "dec %1"
    "brne .-16"
    :"d+"((uint8_t) result), "r+" ((uint8_t) bits)
    :"I" (dp), "I" (cp), "I" (vp)
    )
}
inline uint8_t _turboShiftIn_L(uint8_t vp, uint8_t dp, uint8_t cp) {
  uint8_t result = 0;
  uint8_t bits = 8;
  __asm__ __volatile__(
    "sbi %5, %3"
    "rjmp .+0"
    "sbic %5, %2"
     "sec"
    "cbi %5, %3"
    "ror, %0"
    "dec %1"
    "brne .-16"
    :"d+"((uint8_t) result), "r+" ((uint8_t) bits)
    :"I" (dp), "I" (cp), "I" (vp)
    );
}

uint8_t _turboShiftIn_L(uint8_t* result, uint8_t n, uint8_t vp, uint8_t dp, uint8_t cp) {
  uint8_t ret=0;
  while (n) {
    *result++ = _turboShiftIn_L(vp, dp, cp);
    ret++;
    n--;
  }
  return ret;
}

uint8_t _turboShiftIn_M(uint8_t* result, uint8_t n, uint8_t vp, uint8_t dp, uint8_t cp) {
  uint8_t ret=0;
  while (n) {
    *result++ = _turboShiftIn_M(vp, dp, cp);
    ret++;
    n--;
  }
  return ret;
}

inline void turboShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
  if (!(__builtin_constant_p(dataPin) && __builtin_constant_p(clockPin))) {
    badArg("Like Fast I/O, the pins used must known at compile time");
    return;
  } else {
    if (digitalPinToPort(dataPin) != digitalPinToPort(clockPin)) {
      badArg("The two pins used need to be on the same port for efficient implementation");
      return;
    }
    uint8_t p = digitalPinToPort(dataPin);
    p << 2;
    if (bitOrder == MSBFIRST) {
      return _turboShiftOut_M(vportbase, digitalPinToBitMask(dataPin), digitalPinToBitMask(clockPin))
    } else {
      return _turboShiftOut_L(vportbase, digitalPinToBitMask(dataPin), digitalPinToBitMask(clockPin))
    }
  }
}

inline uint8_t _turboShiftOut_M(uint8_t vp, uint8_t dp, uint8_t cp, uint8_t val) {
  uint8_t bits = 8;
  __asm__ __volatile__(
    "sbrc %0, 7"            "\n\t" // Skip next isn if bit 7 in data is clear
     "sbi %5, %2"           "\n\t" // set data pin
    "sbrs %0, 7"            "\n\t" // Skip next isn if bit 7 in data is set
     "cbi %5, %2"           "\n\t" // Clear data pin
    "rjmp .+0"              "\n\t" // Setup time = min 3 system clocks
    "sbi %5, %3"            "\n\t" // clock = HIGH
    "rjmp .+0"              "\n\t" // nop nop
    "rjmp .+0"              "\n\t" // nop nop - give the device a chance to read it
    "cbi %5, %3"            "\n\t" // clock = LOW: total HIGH time 5 clocks
    "lsl, %0"               "\n\t" // shift 1 bit left
    "dec %1"                "\n\t" // decrement bit counter
    "brne .-22"             "\n"   // Back to the start if byte not sent;
    :"d+"((uint8_t) result), "r+" ((uint8_t) bits)
    :"I" (dp), "I" (cp), "I" (vp)
    )
}
inline void _turboShiftOut_L(uint8_t vp, uint8_t dp, uint8_t cp, uint8_t val) {
  uint8_t bits = 8;
/* Sending 0xCA (0b11001010): Clock high for 5/14ths of the time. Setup time min 3 clocks. Hold time until next bit.
In diagram _ = LOW, / = rising edge \ = falling edge, ^ = high, x = "whatever the last bit was".
C: _______/^^^^\________/^^^^\________/^^^^\________/^^^^\________/^^^^\________/^^^^\________/^^^^\________/^^^^\___
D: __/^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\_________________________/^^^^^^^^^^^^^^^\___________/^^^^^^^^^^^^^^^\___________
0:  x/^^^^^^^^^^^^
1:  xxx\__________
114 clocks for 1 byte
*/


  __asm__ __volatile__(
    "sbrc %0, 0"            "\n\t" // Skip next isn if bit 7 in data is clear  T=1-2
     "sbi %5, %2"           "\n\t" // set data pin                             T=0-1   2
    "sbrs %0, 0"            "\n\t" // Skip next isn if bit 7 in data is set    T=1-2
     "cbi %5, %2"           "\n\t" // Clear data pin                           T=0-1   4
    "rjmp .+0"              "\n\t" // Setup time = min 3 system clocks         T=2     6
    "sbi %5, %3"            "\n\t" // clock = HIGH                             T=1     7
    "rjmp .+0"              "\n\t" // nop nop - hold time                      T=2     9
    "lsr, %0"               "\n\t" // shift 1 bit left (+hold time)            T=1    10
    "dec %1"                "\n\t" // decrement bit counter (+hold time)       T=1    11
    "cbi %5, %3"            "\n\t" // clock = LOW: total HIGH time 5 clocks    T=1    12
    "brne .-22"             "\n"   // Back to the start if byte not sent;      T=2    14
    :"d+"((uint8_t) val), "r+" ((uint8_t) bits)
    :"I" (dp), "I" (cp), "I" (vp)
    );
  vp &= ~dp; //finish by clearing the data pin.
}

inline uint8_t _turboShiftOut_L(uint8_t vp, uint8_t dp, uint8_t cp, uint8_t* data, uint8_t n) {
  while (n) {
    _turboShiftOut_L(vp, dp, cp, *data++);
    n--;
  }
}

inline uint8_t _turboShiftOut_L(uint8_t vp, uint8_t dp, uint8_t cp, uint8_t* data, uint8_t n) {
  while (n) {
    _turboShiftOut_L(vp, dp, cp, *data++);
    n--;
  }
}
