#include <Arduino.h>
#if defined(PORTA_EVGENCTRL) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.

  uint8_t _setEventPin(uint8_t pin, uint8_t chan) {
    // Works the same was as
    uint8_t temp = digitalPinToPort(pin)
    if (temp != NOT_A_PIN && (chan+1) < 3) {
      volatile uint8_t* P;
      p=(volatile uint8_t*)(uint16_t)(digitalPinToPortStruct(temp));
      p += 0x18; // now p pointing to evgenctrl.
      uint8_t oldsreg = SREG;
      cli();
      uint8_t r = *p; // r is current setting
      if (chan == 255) {
        if (!(r & 0x08)) {
          chan = 0;
        } else if (!(r & 0x80)) {
          chan = 1;
        } else {
          chan = 254;
        }
      }
      if (chan < 2) {
        uint8_t bp = 0x08 | digitalPinToBitPosition(pin);
        if (chan) {
          _SWAP(bp);
          r &= 0x0F;
        } else {
          r &= 0xF0;
        }
        r |= bp
      }
      *p = r;
      SREG = oldsreg;
      return chan;
    }
    return 255;
  }
  uint8_t _getPortEventConfig(uint8_t port){
    uint8_t temp = digitalPinToPort(pin)
    return *(((uint8_t*) temp) + 0x18);
  } // just shorthand for looking up the port and returning it's EVGENCTRL value

  uint8_t _setRTCEventChan(uint8_t val, uint8_t chan) {  //
    if (val < 0x0F && (chan + 1) < 3) {
      uint8_t oldsreg = SREG;
      cli(); //gotta disable interrupts while using the evgenctrl value - this needs to be atomic
      uint8_t r = RTC.PITEVGENCTRLA; // this approach takes 5 clocks and 8 bytes plus the manipulations. Doing it in situ would at best be similar, but could at worst be
      if (chan == 255) {             // 6 clocks slower, and would be 8 bytes larger, and about the same amount of code would have to be made atomic.
        if(!(r & 0x0F)) {
          chan = 0;
        } else if (!(r & 0xF0)) {
          chan = 1;
        } else {
          chan = 254;
        } // chan = 255/-1 has been transformed into 0 or 1 if there was an available generator output, or 254/-2 otherwise
      }
      if (chan < 2) {
        if (chan) {  // chan = 1 since we know it's less than 2
          _SWAP(val);
          r &= 0x0F;
          r |= val;
        } else {     // chan = 0
          r &= 0xF0;
          r |= val;
        }
        RTC.PITEVGENCTRLA = r;
      }
      // nothing gets set if chan isn't 1 or 0
      SREG = oldsreg;
      return chan; // this will be 0 or 1 if a channel was set, or 254 if not.
    } else {
      return NOT_A_CHANNEL;
    }
  }
  uint8_t _getRTCEventConfig() { //simply returns the RTC channel configuration.
    return RTC.PITEVGENCTRLA;
  }
  uint8_t _RTCPrescaleToVal(uint16_t prescale) {
    if ((prescale & (prescale - 1)) != 0) {
      return NOT_A_CHANNEL;
    }
    if (prescale < 4) {
      return NOT_A_CHANNEL;
    }
    uint8_t ret = 255;
    __asm__ __volatile__ (
      "cpi  %1A,   0"   "\n\t" // see if low byte is 0.
      "brne .+6"        "\n\t" // if low byte is not equal to 0, then high byte is 0, so we don't need to loop over a 2 byte value
      "ldi   %0,   6"   "\n\t" // if it is, then value must be 7 or more, so load 6 into it
      "mov  %1A, %1B"   "\n\t" // and copy the high byte which we know has a single 1 bit...
      "inc  %0"         "\n\t" // Increment ret. After this, is 7, 8, 9 ... if low byte was zero (and high byte is now where low byte was), or 0, 1, 2 ... if it wasn't.
      "lsr  %1A"        "\n\t" // shift the byte we care about (handled above). We know that if the low byte was non zero, the first two bits must be 0 so we want an extra lsr.
      "brcc .-6"        "\n\t" // goes back to the inc unless carry bit was set by the lsr shifting out a 1. So after each increment we check if a carry bit was set.
                               // The brne lands on the lsr if low byte wasn't 0, shifting out the first 0, then it increments ret to 0 and shifts out second 0,
                               // So if original prescale was 4, that would get shifted into the carry bit as soon as ret was incremented to 1. Perfect.
                               // And if the prescale was 256, 6 will increment to 7 as a 1 is shifted out, Perfect.
      :"+d" ((uint8_t)ret),
      "+d" ((uint16_t)prescale) //doesn't matter that we shit on prescale, it's not referenced again and was passed by value, so what we do to it doesn't matter.
    );
    return ret;
  }
  uint16_t _getRTCPrescale(uint8_t chan) {
    if (chan < 2) {
      uint8_t readval = (chan ? 0xF0 : 0x0F);
      if (chan == 1) {
        _SWAP(readval);
      }
      if (readval) {
        return ((uint16_t) 2) << readval;
      }
      return 0;
    } else {
      return -1;
    }
  }
#endif
