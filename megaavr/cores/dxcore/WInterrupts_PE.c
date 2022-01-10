#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)

  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>

  #include "wiring_private.h"

  #ifdef PORTE

    extern voidFuncPtr * intFunc[7];
    voidFuncPtr intFunc_E[8];
    void attachPortEEnable(){
      intFunc[4]=intFunc_E;
    }
  /*
    ISR(PORTE_PORT_vect) {
      uint8_t flags = VPORTE.INTFLAGS;
      isrBody(flags,4);
      VPORTE.INTFLAGS = flags;
    }
  */
    ISR(PORTE_PORT_vect, ISR_NAKED){
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 8"      "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
      ::);
    }
  #endif
#endif
