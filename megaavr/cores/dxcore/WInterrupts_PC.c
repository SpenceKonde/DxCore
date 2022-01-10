#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>
  #include "wiring_private.h"
  #ifdef PORTC
    extern voidFuncPtr * intFunc[7];
    extern voidFuncPtr intFunc_C[8];
    void attachPortCEnable() {
      intFunc[2]=intFunc_C;
    }
    ISR(PORTC_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 4"      "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
      ::);
    }
  #endif
#endif
