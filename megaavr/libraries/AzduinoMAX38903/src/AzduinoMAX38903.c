// *INDENT-OFF*
int8_t setMAX38903Voltage(uint8_t setting) {
  if (setting & 0x33) {                //mov andi breq
    return -1; /* error - invalid setting */
  } else if (setting < _SWAP(setting)) { // mov swap cp brxx
    return -1;                        //ldi ret
  } else  {
    VPORTE.OUT     &= ~(1 << 6);      // cbi
    if (setting == REGOFF) {          // cpi breq
      return 0;                       // ldi
    }
    if (setting     &  (1 << 7))  {   // sbrc rjmp
      VPORTG.DIR   |=  (1 << 7);      // sbi
      if (setting   &  (1 << 3))  {   // sbrc
        VPORTG.OUT |=  (1 << 7);      // sbi
      } else {                        // sbrs
        VPORTG.OUT &= ~(1 << 7);      // cbi
      }                               // rjmp
    } else {
      VPORTG.DIR   &= ~(1 << 7);      // cbi
    }
    if (!(setting   &  (1 << 6))) {  // sbrs rjmp
      VPORTG.DIR   |=  (1 << 6);     // sbi
      if (setting   &  (1 << 2))  {  // sbrc
        VPORTG.OUT |=  (1 << 6);     // sbi
      } else {                       // sbrs
        VPORTG.OUT &= ~(1 << 6);     // cbi
      }                              // rjmp
    } else {
      VPORTG.DIR   &= ~(1 << 6);     // cbi
    }
    VPORTE.OUT     |=  (1 << 6);     // sbi
    return 1;                        // ldi ret
  }
}
