/*
  EEPROM.h - EEPROM library
  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/io.h>

/***
    EERef class.

    This object references an EEPROM cell.
    Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
    This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.
***/

struct EERef {

  EERef(int index)
    : index(index)                 {}

  //Access/read members.
  uint8_t operator*() const            {
    return eeprom_read_byte((uint8_t *)index);
  }
  operator uint8_t() const             {
    return **this;
  }

  //Assignment/write members.
  EERef &operator=(const EERef &ref) {
    return *this = *ref;
  }
  EERef &operator=(uint8_t in)       {
    while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
    // check at start - not end. Makes writing single bytes faster
    // uint8_t nvmctrla=NVMCTRL.CTRLA;
    // if (nvmctrla != NVMCTRL_CMD_EEERWR_gc) {
    // Don't bother checking, the EEPROM write time is measured in milliseconds
    // testing it first would take 4 words of flash in exchange for what?
    // saving 6 clock cycles (but the test overhead is 4, so net 2) when we clearly
    // don't care much about execution time because the next act takes 11ms!

    // Wouldn't have disabled interrupts here, except that as noted
    // if we're writing multiple bytes, we're gonna be waiting for ~11ms per
    // byte after the first, so performance doesn't matter. Because those sort of
    // use cases are worst for the memory barrier.
    uint8_t oldSREG = SREG;
    cli();

    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
    *(uint8_t *)(MAPPED_EEPROM_START + index) = in;

    SREG = oldSREG; //restore SREG and interrupts

    // _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
    // should be covered in docs that if instead of using the
    // included libraries, which support writing to all types of
    // NVM, the user insists on writing or using some different
    // one, but also uses our libraries, that our libraries
    // assume that anyone who needs to do this will clear it
    // themself first (as that is better practice -no chance
    // of collision). If some important library does it the
    // other way, well, then I'll uncomment that - but I won't
    // be happy about it!
    return *this;
  }
  EERef &operator +=(uint8_t in)     {
    return *this = **this + in;
  }
  EERef &operator -=(uint8_t in)     {
    return *this = **this - in;
  }
  EERef &operator *=(uint8_t in)     {
    return *this = **this * in;
  }
  EERef &operator /=(uint8_t in)     {
    return *this = **this / in;
  }
  EERef &operator ^=(uint8_t in)     {
    return *this = **this ^ in;
  }
  EERef &operator %=(uint8_t in)     {
    return *this = **this % in;
  }
  EERef &operator &=(uint8_t in)     {
    return *this = **this & in;
  }
  EERef &operator |=(uint8_t in)     {
    return *this = **this | in;
  }
  EERef &operator <<=(uint8_t in)    {
    return *this = **this << in;
  }
  EERef &operator >>=(uint8_t in)    {
    return *this = **this >> in;
  }

  EERef &update(uint8_t in)          {
    return  in != *this ? *this = in : *this;
  }

  /** Prefix increment/decrement **/
  EERef &operator++()                  {
    return *this += 1;
  }
  EERef &operator--()                  {
    return *this -= 1;
  }

  /** Postfix increment/decrement **/
  uint8_t operator++ (int) {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator-- (int) {
    uint8_t ret = **this;
    return --(*this), ret;
  }

  const int index; //Index of current EEPROM cell.
};

/***
    EEPtr class.

    This object is a bidirectional pointer to EEPROM cells represented by EERef objects.
    Just like a normal pointer type, this can be dereferenced and repositioned using
    increment/decrement operators.
***/

struct EEPtr {

  EEPtr(int index)
    : index(index)                {}

  operator int() const                {
    return index;
  }
  EEPtr &operator=(int in)          {
    return index = in, *this;
  }

  //Iterator functionality.
  bool operator!=(const EEPtr &ptr) {
    return index != ptr.index;
  }
  EERef operator*()                   {
    return index;
  }

  /** Prefix & Postfix increment/decrement **/
  EEPtr &operator++()                 {
    return ++index, *this;
  }
  EEPtr &operator--()                 {
    return --index, *this;
  }
  EEPtr operator++ (int)              {
    return index++;
  }
  EEPtr operator-- (int)              {
    return index--;
  }

  int index; //Index of current EEPROM cell.
};

/***
    EEPROMClass class.

    This object represents the entire EEPROM space.
    It wraps the functionality of EEPtr and EERef into a basic interface.
    This class is also 100% backwards compatible with earlier Arduino core releases.
***/

struct EEPROMClass {

  //Basic user access methods.
  EERef operator[](int idx)        {
    return idx & EEPROM_END;
  }
  uint8_t read(int idx)              {
    return EERef(idx);
  }
  void write(int idx, uint8_t val)   {
    (EERef(idx)) = val;
  }
  void update(int idx, uint8_t val)  {
    EERef(idx).update(val);
  }

  //STL and C++11 iteration capability.
  EEPtr begin()                        {
    return 0x00;
  }
  EEPtr end()                          {
    return length();  //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
  }
  static constexpr int16_t length()   {
    return EEPROM_SIZE;
  }

  //Functionality to 'get' and 'put' objects to and from EEPROM.
  template< typename T > T &get(int idx, T &t) {
    EEPtr e = idx;
    uint8_t *ptr = (uint8_t *) &t;
    for (int count = sizeof(T) ; count ; --count, ++e) {
      *ptr++ = *e;
    }
    return t;
  }

  template< typename T > const T &put(int idx, const T &t) {
    EEPtr e = idx;
    const uint8_t *ptr = (const uint8_t *) &t;
    for (int count = sizeof(T) ; count ; --count, ++e) {
      (*e).update(*ptr++);
    }
    return t;
  }
};

static EEPROMClass EEPROM;
#endif
