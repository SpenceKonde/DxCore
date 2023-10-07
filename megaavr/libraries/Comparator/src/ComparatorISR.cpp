/*  OBLIGATORY LEGAL BOILERPLATE
 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
 either version 2.1 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this library;
 if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*//*
   Modern AVR Comparator library for tinyAVR 0/1/2, megaAVR0, Dx, and  Ex
   Developed in 2019 by MCUdude    https://github.com/MCUdude/
   Ported to tinyAVR & Dx-series by Spence Konde for megaTinyCore and
   DxCore 2021-2023: https://github.com/SpenceKonde/
*/

#include <Comparator.h>
// Array for storing ISR function pointers
#if defined(AC2_AC_vect)
  static volatile voidFuncPtr intFuncAC[3];
#elif defined(AC1_AC_vect)
  static volatile voidFuncPtr intFuncAC[2];
#elif defined(AC0_AC_vect)
  static volatile voidFuncPtr intFuncAC[1];
#else
  #error "Unsupported part? This device does not have an analog comparator!"
#endif

void AnalogComparator::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  #if defined(__AVR_DD__) || !defined(DXCORE)
  AC_INTMODE_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_t)AC_INTMODE_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_t)AC_INTMODE_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_t)AC_INTMODE_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #else
  AC_INTMODE_NORMAL_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #endif
  // Store function pointer
  intFuncAC[comparator_number] = userFunc;


  // Set interrupt trigger and enable interrupt
  #if !defined(DXCORE)
  AC.CTRLA = (AC.CTRLA & ~AC_INTMODE_gm) | intmode ;
  AC.INTCTRL = AC_CMP_bm;
  #else
  AC.INTCTRL = intmode | AC_CMP_bm;
  #endif
}

void AnalogComparator::detachInterrupt() {
  // Disable interrupt
  AC.INTCTRL = 0;
}

#ifdef AC0_AC_vect
ISR(AC0_AC_vect) {
  // Run user function
  intFuncAC[0]();

  // Clear flag
  AC0.STATUS = AC_CMP_bm;
}
#endif

#ifdef AC1_AC_vect
ISR(AC1_AC_vect) {
  // Run user function
  intFuncAC[1]();

  // Clear flag
  AC1.STATUS = AC_CMP_bm;
}
#endif

#ifdef AC2_AC_vect
ISR(AC2_AC_vect) {
  // Run user function
  intFuncAC[2]();

  // Clear flag
  AC2.STATUS = AC_CMP_bm;
}
#endif
