#ifndef ZCD_h
#define ZCD_h

#include <Arduino.h>

#if defined(__AVR_DD__)
#define ZCD ZCD3
#endif

namespace out {
  enum output_t : uint8_t {
    disable = 0x00,
    enable  = 0x40,
    invert  = 0x08
  };
  enum pinswap_t : uint8_t {
    no_swap  = 0x00,
    #if (defined(__AVR_DA__)||defined(__AVR_DB__))
    pin_swap = 0x01,
    swap_all = 0x07
    #endif
  };
};

class ZeroCross {
  public:
    #if !defined(__AVR_DD__)
    ZeroCross(const uint8_t zcd_number, ZCD_t &zcd, register8_t &input_pin);
    #else
    ZeroCross();
    #endif
    void init();
    void start(bool state = true);
    void stop();
    bool read();
    void attachInterrupt(voidFuncPtr callback, uint8_t mode);
    void detachInterrupt();

    static bool have_separate_mux();

    out::output_t   output = out::disable;
    out::pinswap_t  output_swap = out::no_swap;

  private:
    #if !defined(__AVR_DD__)
    const uint8_t zcd_number;
    ZCD_t &ZCD;
    register8_t &INPUT_PIN;
    #endif
    bool enable = false;
};

#if defined(ZCD0_ZCD_vect)
  extern ZeroCross zcd0;
  #define zcd zcd0
#endif

#if defined(ZCD1_ZCD_vect)
  extern ZeroCross zcd1;
#endif

#if defined(ZCD2_ZCD_vect)
  extern ZeroCross zcd2;
#endif

#if defined(ZCD3_ZCD_vect)
  extern ZeroCross zcd3;
  #define zcd zcd3
#endif

#endif
