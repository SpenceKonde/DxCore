/*
 * SPI Master library for Arduino Zero.
 * Copyright (c) 2015 Arduino LLC
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

#include "SPI1.h"
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

const SPI1Settings DEFAULT_SPI_SETTINGS = SPI1Settings();

SPI1Class::SPI1Class()
{
  initialized = false;

}

bool SPI1Class::pins(uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCK, uint8_t pinSS)
{
  #if defined(PORTMUX_SPIROUTEA)
    #if (defined(PIN_SPI1_MOSI_PINSWAP_2) && defined(PIN_SPI1_MISO_PINSWAP_2) && defined(PIN_SPI1_SCK_PINSWAP_2) && defined(PIN_SPI1_SS_PINSWAP_2))
      if (pinMOSI == PIN_SPI1_MOSI_PINSWAP_2 && pinMISO == PIN_SPI1_MISO_PINSWAP_2 && pinSCK == PIN_SPI1_SCK_PINSWAP_2 && pinSS == PIN_SPI1_SS_PINSWAP_2)
      {
        _uc_mux=2;
        return true;
      } else
    #endif
    if(pinMOSI == PIN_SPI1_MOSI_PINSWAP_1 && pinMISO == PIN_SPI1_MISO_PINSWAP_1 && pinSCK == PIN_SPI1_SCK_PINSWAP_1 && pinSS == PIN_SPI1_SS_PINSWAP_1)
    {
      _uc_mux=1;
      return true;
    }
    else if(pinMOSI == PIN_SPI1_MOSI && pinMISO == PIN_SPI1_MISO && pinSCK == PIN_SPI1_SCK && pinSS == PIN_SPI1_SS)
    {
      _uc_mux=0;
      return true;
    }
    else {
       _uc_mux=0;
      return false;
    }
  #endif
  return false;
}
bool SPI1Class::swap(uint8_t state)
{
  #if defined(PORTMUX_SPIROUTEA)
    #if (defined(PIN_SPI1_MOSI_PINSWAP_2) && defined(PIN_SPI1_MISO_PINSWAP_2) && defined(PIN_SPI1_SCK_PINSWAP_2) && defined(PIN_SPI1_SS_PINSWAP_2))
      if (state == 2)
      {
        _uc_mux=2;
        return true;
      } else
    #endif
      if(state == 1)
      {
        _uc_mux=1;
        return true;
      }
      else if(state == 0)
      {
        _uc_mux=0;
        return true;
      }
      else {
         _uc_mux=0;
        return false;
      }

  #endif
  return false;
}

void SPI1Class::begin()
{
  init();
  #if defined(PORTMUX_SPIROUTEA)
    PORTMUX.SPIROUTEA = _uc_mux | (PORTMUX.SPIROUTEA & ~3);
  #endif

  #if ((defined(PIN_SPI1_MOSI_PINSWAP_1) && defined(PIN_SPI1_MISO_PINSWAP_1) && defined(PIN_SPI1_SCK_PINSWAP_1) && defined(PIN_SPI1_SS_PINSWAP_1)) || (defined(PIN_SPI1_MOSI_PINSWAP_2) && defined(PIN_SPI1_MISO_PINSWAP_2) && defined(PIN_SPI1_SCK_PINSWAP_2) && defined(PIN_SPI1_SS_PINSWAP_2)))
    if(_uc_mux == 0)
    {
      pinMode(PIN_SPI1_MOSI, OUTPUT);
      pinMode(PIN_SPI1_SCK, OUTPUT);
    }
    #if (defined(PIN_SPI1_MOSI_PINSWAP_1) && defined(PIN_SPI1_MISO_PINSWAP_1) && defined(PIN_SPI1_SCK_PINSWAP_1) && defined(PIN_SPI1_SS_PINSWAP_1))
      else if(_uc_mux == 1)
      {
        pinMode(PIN_SPI1_MOSI_PINSWAP_1, OUTPUT);
        pinMode(PIN_SPI1_SCK_PINSWAP_1, OUTPUT);
      }
    #endif
    #if (defined(PIN_SPI1_MOSI_PINSWAP_2) && defined(PIN_SPI1_MISO_PINSWAP_2) && defined(PIN_SPI1_SCK_PINSWAP_2) && defined(PIN_SPI1_SS_PINSWAP_2))
      else if(_uc_mux == 2)
      {
        pinMode(PIN_SPI1_MOSI_PINSWAP_2, OUTPUT);
        pinMode(PIN_SPI1_SCK_PINSWAP_2, OUTPUT);
      }
    #endif
  #else
    // MISO is set to input by the controller
    pinMode(PIN_SPI1_MOSI, OUTPUT);
    pinMode(PIN_SPI1_SCK, OUTPUT);
  #endif

  SPI1.CTRLB |= (SPI_SSD_bm);
  SPI1.CTRLA |= (SPI_ENABLE_bm | SPI_MASTER_bm);

  config(DEFAULT_SPI_SETTINGS);
}

void SPI1Class::init()
{
  if (initialized)
    return;
  interruptMode = SPI_IMODE_NONE;
  interruptSave = 0;
  interruptMask_lo = 0;
  interruptMask_hi = 0;
  initialized = true;
}

void SPI1Class::config(SPISettings settings)
{
  SPI1.CTRLA = settings.ctrla;
  SPI1.CTRLB = settings.ctrlb;
}

void SPI1Class::end()
{
  SPI1.CTRLA &= ~(SPI_ENABLE_bm);
  initialized = false;
}

void SPI1Class::usingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT))
    return;

  if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS)
    interruptMode = SPI_IMODE_GLOBAL;
  else
  {
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

void SPI1Class::notUsingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT))
    return;

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

void SPI1Class::detachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
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
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
      irqMap[shift] = *pin_ctrl_reg;
      *pin_ctrl_reg &= ~(PORT_ISC_gm);
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPI1Class::reattachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPI1Class::beginTransaction(SPISettings settings)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
      noInterrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
    {
      detachMaskedInterrupts();
    }
  }
  config(settings);
}

void SPI1Class::endTransaction(void)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
        interrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
      reattachMaskedInterrupts();
  }
}

void SPI1Class::setBitOrder(uint8_t order)
{
  if (order == LSBFIRST)
    SPI1.CTRLA |=  (SPI_DORD_bm);
  else
    SPI1.CTRLA &= ~(SPI_DORD_bm);
}

void SPI1Class::setDataMode(uint8_t mode)
{
  SPI1.CTRLB = ((SPI1.CTRLB & (~SPI_MODE_gm)) | mode );
}

void SPI1Class::setClockDivider(uint8_t div)
{
  SPI1.CTRLA = ((SPI1.CTRLA &
                  ((~SPI_PRESC_gm) | (~SPI_CLK2X_bm) ))  // mask out values
                  | div);                           // write value
}

byte SPI1Class::transfer(uint8_t data)
{
  /*
  * The following NOP introduces a small delay that can prevent the wait
  * loop from iterating when running at the maximum speed. This gives
  * about 10% more speed, even if it seems counter-intuitive. At lower
  * speeds it is unnoticed.
  */
  asm volatile("nop");

  SPI1.DATA = data;
  while ((SPI1.INTFLAGS & SPI_RXCIF_bm) == 0);  // wait for complete send
  return SPI1.DATA;                             // read data back
}

uint16_t SPI1Class::transfer16(uint16_t data) {
  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;

  t.val = data;

  if ((SPI1.CTRLA & SPI_DORD_bm) == 0) {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  } else {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  }

  return t.val;
}

void SPI1Class::transfer(void *buf, size_t count)
{
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i=0; i<count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

#if SPI_INTERFACES_COUNT > 0
  SPI1Class SPI1;
#endif
