/*
 * SPI Master library for DxCore.
 * Copyright (c) 2021 Spence Konde, based om earlier version
 * of SPI.h (c) 2015 Arduino LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
// *INDENT-OFF* astyle hates how we formatted this

#include "SPI.h"
#include <Arduino.h>

#ifndef SPI_IMODE_NONE
  #define SPI_IMODE_NONE   0
#endif
#ifndef SPI_IMODE_EXTINT
  #define SPI_IMODE_EXTINT 1
#endif
#ifndef SPI_IMODE_GLOBAL
  #define SPI_IMODE_GLOBAL 2
#endif

#ifdef SPI1
  // For the DA/DB-series parts with two SPI modules, SPI.h can use either SPI0 or SPI1.
  #define SPI_MODULE (*_hwspi_module)
#else
  // when there's only one SPI module, we simplify the code and just use that.
  #define SPI_MODULE SPI0
#endif

const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();

SPIClass::SPIClass() {
  initialized = false;

}

bool SPIClass::pins(uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCK, __attribute__ ((unused)) uint8_t pinSS) {
  if (initialized) return false;
  // calling swap() without turning off the peripheral with SPI.end() would have led to a bunch of
  // oddly configured registers left behind, and things not working quite right - and there is no
  // particular up-side to this.
  #if (!defined(SPI1))
    // implementation for tinyAVR and AVR DD-series, which only have a single SPI interface.
    // The AVR DD-series has a lot of pinswap options...
    #if (defined(SPI_MUX_PINSWAP_6))
      if(pinMOSI      == PIN_SPI_MOSI_PINSWAP_6 && pinMISO == PIN_SPI_MISO_PINSWAP_6 && pinSCK == PIN_SPI_SCK_PINSWAP_6  /* && pinSS == PIN_SPI_SS_PINSWAP_6 */) {
        _uc_mux        = SPI_MUX_PINSWAP_6;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_6;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_6;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_5))
      if(pinMOSI      == PIN_SPI_MOSI_PINSWAP_5 && pinMISO == PIN_SPI_MISO_PINSWAP_5 && pinSCK == PIN_SPI_SCK_PINSWAP_5  /* && pinSS == PIN_SPI_SS_PINSWAP_5 */) {
        _uc_mux        = SPI_MUX_PINSWAP_5;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_5;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_5;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_4))
      if(pinMOSI      == PIN_SPI_MOSI_PINSWAP_4 && pinMISO == PIN_SPI_MISO_PINSWAP_4 && pinSCK == PIN_SPI_SCK_PINSWAP_4  /* && pinSS == PIN_SPI_SS_PINSWAP_4 */) {
        _uc_mux        = SPI_MUX_PINSWAP_4;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_4;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_4;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_3))
      if(pinMOSI == PIN_SPI_MOSI_PINSWAP_3 && pinMISO == PIN_SPI_MISO_PINSWAP_3 && pinSCK == PIN_SPI_SCK_PINSWAP_3  /* && pinSS == PIN_SPI_SS_PINSWAP_3 */) {
        _uc_mux        = SPI_MUX_PINSWAP_3;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_3;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_3;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_2))
      if(pinMOSI == PIN_SPI_MOSI_PINSWAP_2 && pinMISO == PIN_SPI_MISO_PINSWAP_2 && pinSCK == PIN_SPI_SCK_PINSWAP_2  /* && pinSS == PIN_SPI_SS_PINSWAP_2 */) {
        _uc_mux        = SPI_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_2;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_1))
      if(pinMOSI == PIN_SPI_MOSI_PINSWAP_1 && pinMISO == PIN_SPI_MISO_PINSWAP_1 && pinSCK == PIN_SPI_SCK_PINSWAP_1  /* && pinSS == PIN_SPI_SS_PINSWAP_1 */) {
        _uc_mux        = SPI_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_1;
        return true;
      } else
    #endif
    #if defined(SPI_MUX)
      if(pinMOSI == PIN_SPI_MOSI && pinMISO == PIN_SPI_MISO && pinSCK == PIN_SPI_SCK  /* && pinSS == PIN_SPI_SS */) {
        _uc_mux        = SPI_MUX;
        _uc_pinMOSI    = PIN_SPI_MOSI;
        _uc_pinSCK     = PIN_SPI_SCK;
        return true;
      }
    #endif
    _uc_mux        = DEFAULT_SPI_MUX;
    _uc_pinMOSI    = DEFAULT_SPI_MOSI;
    _uc_pinSCK     = DEFAULT_SPI_SCK;
    return false;

    // end of single-SPI implementation

  #else
    // we have two SPI interfaces to deal with, each with up to two pinswap options
    #if defined(SPI_MUX_PINSWAP_2)
      if (pinMOSI == PIN_SPI_MOSI_PINSWAP_2 && pinMISO == PIN_SPI_MISO_PINSWAP_2 && pinSCK == PIN_SPI_SCK_PINSWAP_2  /* && pinSS == PIN_SPI_SS_PINSWAP_2 */) {
        _uc_mux        = SPI_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_2;
        _hwspi_module  =&SPI0;
        return true;
      } else
    #endif

    #if defined(SPI_MUX_PINSWAP_1)
      if(pinMOSI == PIN_SPI_MOSI_PINSWAP_1 && pinMISO == PIN_SPI_MISO_PINSWAP_1 && pinSCK == PIN_SPI_SCK_PINSWAP_1  /* && pinSS == PIN_SPI_SS_PINSWAP_1 */) {
        _uc_mux        = SPI_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_1;
        _hwspi_module  =&SPI0;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX_PINSWAP_2)
      if (pinMOSI == PIN_SPI1_MOSI_PINSWAP_2 && pinMISO == PIN_SPI1_MISO_PINSWAP_2 && pinSCK == PIN_SPI1_SCK_PINSWAP_2  /* && pinSS == PIN_SPI1_SS_PINSWAP_2 */) {
        _uc_mux        = SPI1_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI1_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI1_SCK_PINSWAP_2;
        _hwspi_module  =&SPI1;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX_PINSWAP_1)
      if(     pinMOSI == PIN_SPI1_MOSI_PINSWAP_1 && pinMISO == PIN_SPI1_MISO_PINSWAP_1 && pinSCK == PIN_SPI1_SCK_PINSWAP_1  /* && pinSS == PIN_SPI1_SS_PINSWAP_1 */) {
        _uc_mux        = SPI1_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI1_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI1_SCK_PINSWAP_1;
        _hwspi_module  = &SPI1;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX)
      if(     pinMOSI == PIN_SPI1_MOSI  && pinMISO == PIN_SPI1_MISO && pinSCK == PIN_SPI1_SCK  /* && pinSS == PIN_SPI1_SS */) {
        _uc_mux        = SPI1_MUX;
        _uc_pinMOSI    = PIN_SPI1_MOSI;
        _uc_pinSCK     = PIN_SPI1_SCK;
        _hwspi_module  = &SPI1;
        return true;
      } else
    #endif
    {
      _uc_mux        = SPI_MUX;
      _uc_pinMOSI    = PIN_SPI_MOSI;
      _uc_pinSCK     = PIN_SPI_SCK;
      _hwspi_module  = &SPI0;
      return ( pinMOSI == PIN_SPI_MOSI && pinMISO == PIN_SPI_MISO && pinSCK == PIN_SPI_SCK);
    }
  #endif
}

bool SPIClass::swap(uint8_t state) {
  if (initialized) return false;
  // calling swap() without turning off the peripheral with SPI.end() would have led to a bunch of
  // oddly configured registers left behind, and things not working quite right - and there is no
  // particular up-side to this.
  #if (!defined(SPI1))
    // implementation for tinyAVR and AVR DD-series, which only have a single SPI interface.
    // The AVR DD-series has a lot of pinswap options...
    #if (defined(SPI_MUX_PINSWAP_6))
      if(state == 6) {
        _uc_mux        = SPI_MUX_PINSWAP_6;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_6;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_6;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_5))
      if(state == 5) {
        _uc_mux        = SPI_MUX_PINSWAP_5;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_5;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_5;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_4))
      if(state == 4) {
        _uc_mux        = SPI_MUX_PINSWAP_4;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_4;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_4;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_3))
      if(state == 3) {
        _uc_mux        = SPI_MUX_PINSWAP_3;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_3;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_3;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_2))
      if(state == 2) {
        _uc_mux        = SPI_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_2;
        return true;
      } else
    #endif

    #if (defined(SPI_MUX_PINSWAP_1))
      if(state == 1) {
        _uc_mux        = SPI_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_1;
        return true;
      } else
    #endif
    #if defined(SPI_MUX)
      if(state == 0) {
        _uc_mux        = SPI_MUX;
        _uc_pinMOSI    = PIN_SPI_MOSI;
        _uc_pinSCK     = PIN_SPI_SCK;
        return true;
      }
    #endif
    _uc_mux        = DEFAULT_SPI_MUX;
    _uc_pinMOSI    = DEFAULT_SPI_MOSI;
    _uc_pinSCK     = DEFAULT_SPI_SCK;
    return false;
    // end of single-SPI implementation

  #else
    // we have two SPI interfaces to deal with, each with up to two alternate pinswap options
    #if defined(SPI_MUX_PINSWAP_2)
      if(state == SPI0_SWAP_ALT2) {
        _uc_mux        = SPI_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_2;
        _hwspi_module  = &SPI0;
        return true;
      } else
    #endif

    #if defined(SPI_MUX_PINSWAP_1)
      if(state == SPI0_SWAP_ALT1) {
        _uc_mux        = SPI_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI_SCK_PINSWAP_1;
        _hwspi_module  = &SPI0;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX_PINSWAP_2)
      if(state == SPI1_SWAP_ALT2) {
        _uc_mux        = SPI1_MUX_PINSWAP_2;
        _uc_pinMOSI    = PIN_SPI1_MOSI_PINSWAP_2;
        _uc_pinSCK     = PIN_SPI1_SCK_PINSWAP_2;
        _hwspi_module  = &SPI1;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX_PINSWAP_1)
      if(state == SPI1_SWAP_ALT1) {
        _uc_mux        = SPI1_MUX_PINSWAP_1;
        _uc_pinMOSI    = PIN_SPI1_MOSI_PINSWAP_1;
        _uc_pinSCK     = PIN_SPI1_SCK_PINSWAP_1;
        _hwspi_module  = &SPI1;
        return true;
      } else
    #endif

    #if defined(SPI1_MUX)
      if(state == SPI1_SWAP_DEFAULT) {
        _uc_mux        = SPI1_MUX;
        _uc_pinMOSI    = PIN_SPI1_MOSI;
        _uc_pinSCK     = PIN_SPI1_SCK;
        _hwspi_module  = &SPI1;
        return true;
      } else
    #endif

    if(state == SPI0_SWAP_DEFAULT) {
      _uc_mux        = SPI_MUX;
      _uc_pinMOSI    = PIN_SPI_MOSI;
      _uc_pinSCK     = PIN_SPI_SCK;
      _hwspi_module  = &SPI0;
      return true;
    }
    else {
      _uc_mux        = SPI_MUX;
      _uc_pinMOSI    = PIN_SPI_MOSI;
      _uc_pinSCK     = PIN_SPI_SCK;
      _hwspi_module  = &SPI0;
      return false;
    }
  #endif
  return false;
}

void SPIClass::begin() {
  init();
  #if !defined(SPI1)
    // Implementation for tinyAVR 0/1/2-series, megaAVR 0-series and AVR DD-series, which only have a single SPI interface.
    // First, configure PORTMUX.
    #if defined(PORTMUX_SPIROUTEA)
      // AVR DD-series, tinyAVR 2-series
      PORTMUX.SPIROUTEA = _uc_mux | (PORTMUX.SPIROUTEA & ~PORTMUX_SPI0_gm);
    #elif defined(PORTMUX_TWISPIROUTEA)
      // megaAVR 0-series
      PORTMUX.TWISPIROUTEA = _uc_mux | (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm);
    #else // defined(PORTMUX_CTRLB) - tiny 0 or 1-series.
      PORTMUX.CTRLB = uc_mux | (PORTMUX.CTRLB & ~PORTMUX_SPI0_bm) ;
    #endif
  #else
    // AVR DA-series or DB-series
    // We have two SPI interfaces to deal with, each with up to two alternate pinswap options
    // Check which SPI module is in use, and then go from there.
    // we only change PORTMUX bits affiliated with the peripheral we are using.
    if (_hwspi_module == &SPI0) {
      // If we're set up to use SPI0
      PORTMUX.SPIROUTEA = _uc_mux | (PORTMUX.SPIROUTEA & (~PORTMUX_SPI0_gm));

    } else {
      // we're set to use SPI1
      PORTMUX.SPIROUTEA = _uc_mux | (PORTMUX.SPIROUTEA & (~PORTMUX_SPI1_gm));
    }
  #endif
  // no matter what we had to do about the mux; MOSI and SCK need to be set output - but now we set that up already instead of doing it here.
  pinMode(_uc_pinSCK,  OUTPUT);
  pinMode(_uc_pinMOSI, OUTPUT);
  //SPI_MODULE.CTRLB |= (SPI_SSD_bm);
  //SPI_MODULE.CTRLA |= (SPI_ENABLE_bm | SPI_MASTER_bm);
  // We don't call this now because we are about to call config which does the same thing.
  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::init() {
  if (initialized)
    return;
  interruptMode = SPI_IMODE_NONE;
  #ifdef CORE_ATTACH_OLD
  interruptSave = 0;
  interruptMask_lo = 0;
  interruptMask_hi = 0;
  #else
  in_transaction = 0;
  old_sreg = 0x80;
  #endif
  initialized = true;
}

void SPIClass::config(SPISettings settings) {
  SPI_MODULE.CTRLA = settings.ctrla;
  SPI_MODULE.CTRLB = settings.ctrlb;
}

void SPIClass::end() {
  SPI_MODULE.CTRLA = 0;
  // Is clearing this register a sane use of time? I don't thnk it is, we already turn off the port, and every time begin is called we reconfigure the PORTMUX.
  // If user code is manually taking over SPI, and they're also using the SPI library, with the same SPI port at different times... do you really think that they'll be using the
  // default pinset? And that, with all those moving parts, that the thing that they forget is going to have anything to do with the PORTMUX?
  // Someone doing crazy things is either a fool, in which case why are we bothering? They won't make it work without a brain transplant.
  // Or they're a skilled programmer, and can be treated like an adult. Thus we can trust them to check the PORTMUX when they switch their implementation back on after the SPI library has been used.
  // One really does wonder if they claim to be in the second group though - I understand using a custom spi library, you need that for slave mode. But WTF are they doing screwing with this library?

  /*
  #if defined(SPI1)
    PORTMUX.SPIROUTEA &= ~((_hwspi_module == &SPI0) ? PORTMUX_SPI0_gm : PORTMUX_SPI1_gm);
  #elif defined(PORTMUX_SPIROUTEA)
    PORTMUX.SPIROUTEA &= ~PORTMUX_SPI0_gm;
  #elif defined(PORTMUX_TWISPIROUTEA)
    PORTMUX.SPIROUTEA &= ~PORTMUX_SPI0_gm;
  #else // defined(PORTMUX_CTRLB)
    PORTMUX.CTRLB &= PORTMUX_SPI0_bm;
  #endif
  */
  pinMode(_uc_pinSCK,  INPUT);
  pinMode(_uc_pinMOSI, INPUT);
  initialized = false;
}


#ifdef CORE_ATTACH_OLD
void SPIClass::usingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT))
    return;

  if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS)
    interruptMode = SPI_IMODE_GLOBAL;
  else {
    #if USE_MALLOC_FOR_IRQ_MAP
      if (irqMap == NULL) {
        irqMap = (uint8_t*)malloc(EXTERNAL_NUM_INTERRUPTS);
      }
    #endif

    interruptMode |= SPI_IMODE_EXTINT;
    if (interruptNumber < 32) {
      interruptMask_lo |= 1 << interruptNumber;
    } else {
      interruptMask_hi |= 1 << (interruptNumber - 32);
    }
  }
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }

  if (interruptMode & SPI_IMODE_GLOBAL)
    return; // can't go back, as there is no reference count

  if (interruptNumber < 32) {
    interruptMask_lo &= ~(1 << interruptNumber);
  } else {
    interruptMask_hi &= ~(1 << (interruptNumber - 32));
  }

  if (interruptMask_lo == 0 && interruptMask_hi == 0) {
    interruptMode = SPI_IMODE_NONE;
    #if USE_MALLOC_FOR_IRQ_MAP
      free(irqMap);
      irqMap = NULL;
    #endif
  }
}

void SPIClass::detachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      irqMap[shift] = *pin_ctrl_reg;
      *pin_ctrl_reg &= ~(PORT_ISC_gm);
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      irqMap[shift] = *pin_ctrl_reg;
      *pin_ctrl_reg &= ~(PORT_ISC_gm);
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPIClass::reattachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPIClass::beginTransaction(SPISettings settings) {
  if (interruptMode != SPI_IMODE_NONE) {
    if (interruptMode & SPI_IMODE_GLOBAL) {
      noInterrupts();
    } else if (interruptMode & SPI_IMODE_EXTINT) {
      detachMaskedInterrupts();
    }
  }
  config(settings);
}

void SPIClass::endTransaction(void) {
  if (interruptMode != SPI_IMODE_NONE) {
    if (interruptMode & SPI_IMODE_GLOBAL) {
      interrupts();
    } else if (interruptMode & SPI_IMODE_EXTINT) {
      reattachMaskedInterrupts();
    }
  }
}
#else // End of old interrupt related stuff, start of new-attachInterrupt-compatible implementation.
// We aren't going to bother with all that wacky machinery to mask interrupts selectively. If you need it, you're doing something wrong.
// You can send a very large chunk of data before you have to worry about things getting backed up even if you do have to disable interrupts.
// But that would mean you're calling an SPI function from an interrupt, and you shouldn't be doing that anyway. So we can simply disable interrupts in a transaction globally,
// and then you can use it from within interrupts, and unless your slave device is slow or your wiring long and janky, you can run SPI fast. 4 MHz to a 16 MHz AVR, most commercial
// SPI slaves are much faster than that. That means two us per byte, and millis can wait for up to 1ms before
// you can potentially lose 1 millisecond, or around 500 bytes. That's a damned big chunk to read in one go on an AVR.
// If we're instead reading from a W25 flash or something (a more reasonable place to be getting 500 bytes from), the speed limits are much higher.
// In fact, higher than not just the maximum SPI clock, but also the rated cpu frequency of these parts, and even the maximum practically achievable overclocked CPU frequency.
//

void SPIClass::usingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }
  interruptMode = SPI_IMODE_GLOBAL;
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }
  interruptMode = SPI_IMODE_NONE;
}

void SPIClass::beginTransaction(SPISettings settings) {
  if (interruptMode != SPI_IMODE_NONE) {
    old_sreg = SREG;
    cli(); // NoInterrupts();
  }
  in_transaction = 1;
  config(settings);
}

void SPIClass::endTransaction(void) {
  if (in_transaction) {
    in_transaction = 0;
    if (interruptMode != SPI_IMODE_NONE) {
      SREG = old_sreg;
    }
  }
}
#endif // End new attachInterrupt-compatible implementation.


void SPIClass::setBitOrder(uint8_t order) {
  if (order == LSBFIRST) {
    SPI_MODULE.CTRLA |=  (SPI_DORD_bm);
  }  else {
    SPI_MODULE.CTRLA &= ~(SPI_DORD_bm);
  }
}

void SPIClass::setDataMode(uint8_t mode) {
  mode |= 4;
  SPI_MODULE.CTRLB = ((SPI_MODULE.CTRLB & (~SPI_MODE_gm)) | (mode));
}

void SPIClass::setClockDivider(uint8_t div) {
  SPI_MODULE.CTRLA = ((SPI_MODULE.CTRLA &
                  ((~SPI_PRESC_gm | SPI_CLK2X_bm)))  // mask out values
                  | div);                           // write value
}

uint8_t SPIClass::transfer(uint8_t data) {
  /*
  * The following NOP introduces a small delay that can prevent the wait
  * loop from iterating when running at the maximum speed. This gives
  * about 10% more speed, even if it seems counter-intuitive. At lower
  * speeds it is unnoticed.
  */
  asm volatile("nop");

  SPI_MODULE.DATA = data;
  while ((SPI_MODULE.INTFLAGS & SPI_RXCIF_bm) == 0);  // wait for complete send
  return SPI_MODULE.DATA;                             // read data back
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union {
    uint16_t val;
    struct {
      uint8_t lsb;
      uint8_t msb;
    };
  } t;

  t.val = data;

  if ((SPI_MODULE.CTRLA & SPI_DORD_bm) == 0) {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  } else {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  }

  return t.val;
}

void SPIClass::transfer(void *buf, size_t count) {
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i = 0; i < count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

#if SPI_INTERFACES_COUNT > 0
  SPIClass SPI;
#endif
