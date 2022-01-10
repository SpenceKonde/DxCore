#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)

  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>

  #include "wiring_private.h"

  #ifdef PORTF

    extern voidFuncPtr * intFunc[7];
    voidFuncPtr intFunc_F[8];
    void attachPortFEnable(){
      intFunc[5]=intFunc_F;
    }
  /*
    ISR(PORTF_PORT_vect) {
      uint8_t flags = VPORTF.INTFLAGS;
      isrBody(flags,5);
      VPORTF.INTFLAGS = flags;
    }
  */
    ISR(PORTF_PORT_vect, ISR_NAKED){
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 10"     "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
      ::);
    }
  #endif
#endif
