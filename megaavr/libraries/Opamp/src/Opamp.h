#ifndef OPAMP_H
#define OPAMP_H

#include <Arduino.h>

// Constants to control the positive opamp input
namespace in_p {
  enum input_p_t : uint8_t {
    pin         = 0x00,
    wiper       = 0x01,
    dac         = 0x02,
    gnd         = 0x03,
    vdd_div2    = 0x04,
    link_output = 0x05,
    link_out    = 0x05,
    link_wiper  = 0x06,
  };
};

// Constants to control the negative opamp input
namespace in_n {
  enum input_n_t : uint8_t {
    pin    = 0x00,
    wiper  = 0x10,
    output = 0x20,
    dac    = 0x30,
  };
};

// Constants to control the opamp input range
// Conventional consumes less power, but rail to rail accepts a wider input range
namespace in {
  enum inrange_t : uint8_t {
    rail_to_rail = 0x00,
    conventional = 0x01,
    unconfigured = 0xFF,
  };
};

// Constants to control the where the top of the resistor ladder connects to
namespace top {
  enum top_t : uint8_t {
    off    = 0x00,
    output = 0x01,
    vdd    = 0x02,
  };
};

// Constants to control the resistor ladder values
// Resistor R2 connects to the top, and resistor R1 connects to the bottom
namespace wiper {
  enum wiper_t : uint8_t {
    wiper0 = 0x00, // R1 = 60k, R2 = 4k
    wiper1 = 0x20, // R1 = 56k, R2 = 8k
    wiper2 = 0x40, // R1 = 48k, R2 = 16k
    wiper3 = 0x60, // R1 = 32k, R2 = 32k
    wiper4 = 0x80, // R1 = 24k, R2 = 40k
    wiper5 = 0xA0, // R1 = 16k, R2 = 48k
    wiper6 = 0xC0, // R1 = 8k,  R2 = 56k
    wiper7 = 0xE0, // R1 = 4k,  R2 = 60k
  };
};

// Constants to control the where the bottom of the resistor ladder connects to
namespace bottom {
  enum bottom_t : uint8_t {
    off  = 0x00,
    in_p = 0x04,
    in_n = 0x08,
    dac  = 0x0C,
    link = 0x10,
    gnd  = 0x14,
  };
};


// Constants to enable or disable the opamp output
namespace out {
  enum output_t : uint8_t {
    disable     = 0x00,
    enable      = 0x04,
    drive_event = 0x00,
  };
};

// Constants to enable or disable the opamp event trigger
namespace event {
  enum event_t : uint8_t {
    disable = 0x00,
    enable  = 0x02,
  };
};

// Constants to control whenever the opamp should be powered during standby
namespace power {
  enum power_t : uint8_t {
    no_standby = 0x00,
    standby    = 0x80,
  };
};

// Constant for setting whether opamp should be "always on" or "event controlled"
namespace enable {
  enum enable_t : uint8_t {
    disable      = 0x00,
    enable       = 0x01,
    event        = 0x00,
    always_on    = 0x01,
    unconfigured = 0xFF,
  };
};


class Opamp {
  public:
    Opamp(uint8_t ainp,                \
          uint8_t out,                 \
          uint8_t ainn,                \
          uint8_t op_num,              \
          volatile uint8_t &op_ctrla,  \
          volatile uint8_t &op_status, \
          volatile uint8_t &op_resmux, \
          volatile uint8_t &op_inmux,  \
          volatile uint8_t &op_settle, \
          volatile uint8_t &op_cal     \
         );
    uint8_t get_number();
    bool status();
    void calibrate(uint8_t cal_value);
    void init();
    static void start(bool state = true);
    static void stop();
    const uint8_t input_p_pin;
    const uint8_t output_pin;
    const uint8_t input_n_pin;
    in_p::input_p_t  input_p       = in_p::pin;            // Positive opamp input
    in_n::input_n_t  input_n       = in_n::pin;            // Negative opamp input
    in::inrange_t    inrange       = in::unconfigured;     // Opamp input mode
    top::top_t       ladder_top    = top::off;             // Resistor ladder top setting
    wiper::wiper_t   ladder_wiper  = wiper::wiper0;        // Resistor ladder "wiper" setting
    bottom::bottom_t ladder_bottom = bottom::off;          // Resistor ladder bottom setting
    out::output_t    output        = out::enable;          // Opamp output state
    event::event_t   event         = event::enable;        // Event enable
    power::power_t   standby       = power::no_standby;    // Run when MCU is in standby
    uint8_t          settle        = 0x7F;                 // Microseconds needed for the opamp output to settle
    bool             wait_settle   = true;                 // If false, Opamp::start() will not wait for this to settle before returning.
    enable::enable_t enable        = enable::unconfigured; // set to disable to turn off this opamp (or have it run only when it's event input is on). unconfigured means neither set, nor has init been called.

  private:
    const    uint8_t opamp_number;  // Holds the opamp number
    volatile uint8_t &opamp_ctrla;  // Reference to OPAMP_OPxCTRLA
    volatile uint8_t &opamp_status; // Reference to OPAMP_OPxSTATUS
    volatile uint8_t &opamp_resmux; // Reference to OPAMP_OPxRESMUX
    volatile uint8_t &opamp_inmux;  // Reference to OPAMP_OPxINMUX
    volatile uint8_t &opamp_settle; // Reference to OPAMP_OPxSETTLE
    volatile uint8_t &opamp_cal;    // Reference to OPAMP_OPxCAL
};


#if defined(OPAMP_OP0CTRLA)
  extern Opamp Opamp0;
#endif
#if defined(OPAMP_OP1CTRLA)
  extern Opamp Opamp1;
#endif
#if defined(OPAMP_OP2CTRLA)
  extern Opamp Opamp2;
#endif

#endif // OPAMP_H
