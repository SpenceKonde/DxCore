#ifndef COMPARATOR_h
#define COMPARATOR_h

#include <Arduino.h>
// *INDENT-OFF* - astyle wants me to indent the #if's in a way that makes it harder to read.
namespace out {
  enum output_t : uint8_t {
    disable         = 0x00,
    disable_invert  = 0x80,
    enable          = 0x40,
    invert          = 0xC0,
    enable_invert   = 0xC0,
  };
  enum pinswap_t : uint8_t {
    no_swap  = 0x00,
    pin_swap = 0x01,
  };
  enum initval_t : uint8_t {
    init_low  = 0x00,
    init_high = 0x40,
  };
};

namespace hyst {
  enum hysteresis_t : uint8_t {
    disable = 0x00, // No hysteresis
    small   = 0x02, // 10 mV
    medium  = 0x04, // 25 mV
    large   = 0x06, // 50 mV
  };
};

namespace in_p {
  enum inputP_t : uint8_t {
    in0    = 0x00,
#ifndef __AVR_DD__
    in1    = 0x01,
    in2    = 0x02,
#endif
    in3    = 0x03,
#ifdef __AVR_DD__
    in4    = 0x04,
#endif
    pd2    = 0x00,
    pd6    = 0x03,
  };
};

namespace in_n {
  enum inputN_t : uint8_t {
    in0    = 0x00,
#if !(defined(__AVR_DD__) || defined(DB_28_PINS) || defined(DB_32_PINS) )
    in1    = 0x01,
    pd0    = 0x00,
#endif
    in2    = 0x02,
    dacref = 0x03,
    pd7    = 0x02,
  };
};
// *INDENT-ON*
// Unknown how these will be numbered on DD-series, which has an AINN3 and DACREF - initial header release is copied verbatim from DA/DB

namespace ref {
  enum reference_t : uint8_t {
    vref_1v024 = 0x00, // 1.024V
    vref_2v048 = 0x01, // 2.048V
    vref_2v500 = 0x03, // 2.5V
    vref_2v5   = 0x03,
    vref_4v096 = 0x02, // 4.096V
    vref_vdd   = 0x05, // VDD as reference
    vref_vrefa = 0x06, // External reference from the VREFA pin
    disable    = 0x08,
  };
};

class AnalogComparator {
  public:
    AnalogComparator(const uint8_t comparator_number,
                     AC_t &ac,
                     register8_t &in0_p,
                     register8_t &in1_p,
                     register8_t &in2_p,
                     register8_t &in3_p,
                     register8_t &in0_n,
                     register8_t &in1_n,
                     register8_t &in2_n);
    void init();
    void start(bool state = true);
    void stop(bool restorepins = false);
    void attachInterrupt(voidFuncPtr callback, uint8_t mode);
    void detachInterrupt();
    bool read() {
      return !!(AC.STATUS & AC_CMPSTATE_bm);
    }

    out::output_t      output         = out::disable;
    out::pinswap_t     output_swap    = out::no_swap;
    out::initval_t     output_initval = out::init_low;
    hyst::hysteresis_t hysteresis     = hyst::disable;
    in_p::inputP_t     input_p        = in_p::in0;
    in_n::inputN_t     input_n        = in_n::in0;
    ref::reference_t   reference      = ref::disable;
    uint8_t dacref = 0xff;

  private:
    const uint8_t comparator_number;
    AC_t &AC;
    register8_t &IN0_P;
    register8_t &IN1_P;
    register8_t &IN2_P;
    register8_t &IN3_P;
    register8_t &IN0_N;
    register8_t &IN1_N;
    register8_t &IN2_N;
    bool enable = false;
};

#if defined(AC0_AC_vect)
  extern AnalogComparator Comparator0;
  #define Comparator Comparator0
#endif

#if defined(AC1_AC_vect)
  extern AnalogComparator Comparator1;
#endif

#if defined(AC2_AC_vect)
  extern AnalogComparator Comparator2;
#endif

#endif
