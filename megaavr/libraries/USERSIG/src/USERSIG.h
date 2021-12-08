/* USERSIG.h USERROW library
 * Copyright (c) Spence Konde 2021
 * Based on EEPROM.h by David A. Mellis (2006) and Christopher Andrews (2015).
 *
 * This is an exact copy of the EEPROM library, only modified to point to the
 * User Signature Space (otherwise known as the USERROW), a section of 32 bytes
 * of EEPROM-like space on the Dx-series parts. This does NOT support modifying
 * the USERROW on tinyAVR 0/1/2 and megaAVR 0-series - they are simpler and use
 * a different version of the library.
 * On those parts, it has byte erase granularity here, but all we can do is
 * erase the whole damned thing. Implementing the analog of thayt library
 * without modification would have made it too easy for someone to unintentionally
 * exhaust the write endurance of the USERROW (automatically doing an read,
 * modify, erase, rewrite cycle instead of write could amplify the number of writes
 * (imagine, writing a new value to every address. That would generate 32
 * erase-rewrite cycles when only one was actually necessary). A new approach
 * was required.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PUSPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef USERSIG_h
#define USERSIG_h

#include <inttypes.h>
#include <avr/io.h>


/* These are INTERNAL functions, not for public consumption */
/* Forward declarations */
int8_t __USigload();
int8_t __USigflush(uint8_t justerase);
uint8_t __USigread(uint8_t idx);
uint8_t __USigreadraw(uint8_t idx);
int8_t __USigwrite(uint8_t idx, uint8_t data);
int8_t __USigwriteraw(uint8_t idx, uint8_t data);
int8_t __USigflush(uint8_t justerase);

/* USRef class.
 *
 * This object references an USERSIG cell.
 * Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
 * This class has an overhead of two bytes, similar to storing a pointer to an USERSIG cell.
 */

struct USRef {

  USRef(const uint8_t index)
    : index(index)                   {}

  // Access/read members.
  uint8_t operator*() const          {
    return __USigread(index);
  }
  operator uint8_t() const           {
    return **this;
  }

  // Assignment/write members.
  USRef &operator=(const USRef &ref) {
    return *this = *ref;
  }
  USRef &operator=(uint8_t in)       {
    return __USigwrite(index, in), *this;
  }

  USRef &operator +=(uint8_t in)     {
    return *this = **this + in;
  }
  USRef &operator -=(uint8_t in)     {
    return *this = **this - in;
  }
  USRef &operator *=(uint8_t in)     {
    return *this = **this * in;
  }
  USRef &operator /=(uint8_t in)     {
    return *this = **this / in;
  }
  USRef &operator ^=(uint8_t in)     {
    return *this = **this ^ in;
  }
  USRef &operator %=(uint8_t in)     {
    return *this = **this % in;
  }
  USRef &operator &=(uint8_t in)     {
    return *this = **this & in;
  }
  USRef &operator |=(uint8_t in)     {
    return *this = **this | in;
  }
  USRef &operator <<=(uint8_t in)    {
    return *this = **this << in;
  }
  USRef &operator >>=(uint8_t in)    {
    return *this = **this >> in;
  }

  USRef &update(uint8_t in)          {
    return  in != *this ? *this = in : *this;
  }

  /* Prefix increment/decrement */
  USRef &operator++()                {
    return *this += 1;
  }
  USRef &operator--()                {
    return *this -= 1;
  }

  /* Postfix increment/decrement */
  uint8_t operator++ (int)           {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator-- (int)           {
    uint8_t ret = **this;
    return --(*this), ret;
  }

  uint8_t index; // Index of current USERSIG cell.
};

/* USPtr class.
 *
 * This object is a bidirectional pointer to USERROW cells represented by USRef objects.
 * Just like a normal pointer type, this can be dereferenced and repositioned using
 * increment/decrement operators.
 */

struct USPtr {

  USPtr(const uint8_t index)
    : index(index)                  {}

  operator int() const              {
    return index;
  }
  USPtr &operator=(int in)          {
    return index = (in), *this;
  }

  // Iterator functionality.
  bool operator!=(const USPtr &ptr) {
    return index != ptr.index;
  }
  USRef operator*()                 {
    return index;
  }

  /* Prefix & Postfix increment/decrement */
  USPtr &operator++()               {
    return ++index, *this;
  }
  USPtr &operator--()               {
    return --index, *this;
  }
  USPtr operator++ (int)            {
    return index++;
  }
  USPtr operator-- (int)            {
    return index--;
  }

  uint8_t index; // Index of current USERROW cell.
};

/* USERSIGClass class.
 *
 *  This object represents the entire USERROW space.
 *  It wraps the functionality of USPtr and USRef into a basic interface.
 */

#define USIG_FROM_INTERRUPT    -16  // this library doesn't support writing to the USERROW from an interrupt.
#define USIG_WRITE_LOADED       -2  // You called the **internal** raw write function when the buffer was loaded.
#define USIG_BAD_ADDRESS        -4  // You called the **internal** raw write function with an invalid address.

#if USER_SIGNATURES_SIZE >= 128
  #error "The USERROW on this part is too large, and probably works differently from what this library was expecting: STOP"
#endif

static uint8_t __USigBuffer[USER_SIGNATURES_SIZE];
static uint8_t __USigLoaded = 0;

int8_t __USigload() {
  if (CPUINT.STATUS != 0) {
    return -16;
  }
  for (byte i = 0; i < USER_SIGNATURES_SIZE; i++) {
    __USigBuffer[i] = *((volatile uint8_t *) USER_SIGNATURES_START + i);
  }
  __USigLoaded = 1;
  return 0;
}
uint8_t __USigread(uint8_t idx) {
  idx &= (USER_SIGNATURES_SIZE - 1);
  if (__USigLoaded == 0) {
    return *((volatile uint8_t *) USER_SIGNATURES_START + idx);
  }
  return __USigBuffer[idx];
}

uint8_t __USigreadraw(uint8_t idx) {
  return *((volatile uint8_t *) USER_SIGNATURES_START + idx);
}

int8_t __USigwrite(uint8_t idx, uint8_t data) {
  if (CPUINT.STATUS != 0) {
    return -16;
  }
  idx &= (USER_SIGNATURES_SIZE - 1);
  if (!__USigLoaded) {
    if ((__USigreadraw(idx) & data) != data) {
      __USigload();
    } else {
      return __USigwriteraw(idx, data);
    }
  }
  __USigBuffer[idx] = data;
  return 0;
}

int8_t __USigwriteraw(uint8_t idx, uint8_t data) {
  if (CPUINT.STATUS != 0) {
    return -16;
  }
  if (__USigLoaded == 1) {
    return -2;
  }
  if (idx > USER_SIGNATURES_SIZE) {
    return -4;
  }
  uint8_t oldSREG = SREG;
  while (NVMCTRL.STATUS & 3);
  cli();
  _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_NOOP_gc);
  _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);
  *((volatile uint8_t *) USER_SIGNATURES_START + idx) = data;
  SREG = oldSREG;
  return 1;
}

int8_t __USigflush(uint8_t justerase) {
  uint8_t oldSREG = SREG;
  volatile uint8_t *ptr;
  uint8_t retval = 0;
  ptr = (volatile uint8_t *) USER_SIGNATURES_START;
  if (CPUINT.STATUS != 0) {
    return -16;
  }
  if (!justerase) {
    if (__USigLoaded == 0) {
      return 0;                                        // No pending writes.
    }
    for (byte i = 0; i < USER_SIGNATURES_SIZE; i++) {  //
      if (*ptr++ == __USigBuffer[i]) {                  // Loop over the USERROW and tally up
        retval++;                                      // bytes that are different.
      }
    }
    if (retval == 0) {
      __USigLoaded = 0;                                // Mark USERROW as not being loaded
      memset(__USigBuffer, 0xFF, USER_SIGNATURES_SIZE);// Clear it
      return 0;                                        // No diff
    }
    ptr = (volatile uint8_t *) USER_SIGNATURES_START;   // Reset ptr to start
  }
  while (NVMCTRL.STATUS & 3);
  cli();
  _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_NOOP_gc);// Must reset to NOOP first
  _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);// Flash Page ERase
  while (NVMCTRL.STATUS & 3);                          // Wait for write
  _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_NOOP_gc);// Reset to NOOP - do this even if not writing
  *ptr = 0;                                            // erase the USERROW by writing with FLPER
  if (!justerase) {
    _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);  // Flash WRite
    for (byte i = 0; i < USER_SIGNATURES_SIZE; i++) {  // loop over the USERROW
      *ptr++ = __USigBuffer[i];                        // write each byte in turn
    }
  }
  __USigLoaded = 0;                                    // Finally, we mark the buffer as unused
  memset(__USigBuffer, 0xFF, USER_SIGNATURES_SIZE);    // And clear it.
  SREG = oldSREG;                                      // re-enable interrupts.
  return retval;
}


struct USERSIGClass {

  // Basic user access methods.
  USRef operator[](const int idx)    {
    return idx;
  }
  uint8_t read(int idx)              {
    return __USigread(idx);
  }
  int8_t write(int idx, uint8_t val) {
    return __USigwrite(idx, val);
  }
  int8_t flush() {
    return __USigflush(0);
  }
  int8_t erase() {
    return __USigflush(1);
  }
  int8_t pending() {
    return __USigLoaded;
  }
  int8_t update(int idx, uint8_t val)  {
    return __USigwrite(idx, val);
  }

  // STL and C++11 iteration capability.
  USPtr begin()                      {
    return 0x00;
  }

  // Standards require this to be the item after the last valid entry. The returned pointer is invalid.
  USPtr end()                        {
    return length();
  }

  static constexpr uint8_t length()  {
    return USER_SIGNATURES_SIZE;
  }

  // Functionality to 'get' and 'put' objects to and from USERROW.
  template< typename T > T &get(int idx, T &t) {
    uint8_t *ptr = (uint8_t *) &t;
    for (int count = sizeof(T); count; --count) {
      *ptr++ = __USigread(idx++);
    }
    return t;
  }

  template< typename T > const T &put(int idx, const T &t) {
    const uint8_t *ptr = (const uint8_t *) &t;
    for (int count = sizeof(T); count; --count) {
      __USigwrite(idx++, *ptr++);   // Write the new byte with __USigwrite()
      if (sizeof(T) > 4) {          // if we are writing something that's not a primitive
        __USigflush(0);             // we will automatically commit
      }
    }
    return t;
  }
};

static USERSIGClass USERSIG;
#endif
