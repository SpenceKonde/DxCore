#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>
  #include "wiring_private.h"
  #ifdef PORTG
    extern voidFuncPtr * intFunc[7];
    extern voidFuncPtr intFunc_G[8];
    void attachPortGEnable() {
      intFunc[6]=intFunc_G;
    }
    ISR(PORTG_PORT_vect, ISR_NAKED){
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 12"     "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
      ::);
    }
  #endif
#endif
