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
        "push r1"       "\n\t"
        "push r16"      "\n\t"
        "ldi r16, 4"    "\n\t"
        ::);
        isrBody();
      __builtin_unreachable();
    }
  #endif
#endif
