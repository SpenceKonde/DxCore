/*
  Copyright (c) 2018 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
 * Defines for 16 bit timers used with Servo library
 *
 */

// Adapted by Spence Konde for megaTinyCore 2019


#ifndef __SERVO_TIMERS_H__
  #define __SERVO_TIMERS_H__

  #if defined(SERVO_USE_TIMERB1) && !defined(MILLIS_USE_TIMERB1)
    #ifndef TCB1
      #error "SERVO_USE_TIMERB1 defined, but this part doesn't have that timer!"
    #endif
    #define USE_TIMERB1
  #elif defined(SERVO_USE_TIMERB2) && !defined(MILLIS_USE_TIMERB2)
    #ifndef TCB2
      #error "SERVO_USE_TIMERB2 defined, but this part doesn't have that timer!"
    #endif
    #define USE_TIMERB2
  #elif defined(SERVO_USE_TIMERB3) && !defined(MILLIS_USE_TIMERB3)
    #ifndef TCB3
      #error "SERVO_USE_TIMERB3 defined, but this part doesn't have that timer!"
    #endif
    #define USE_TIMERB3
  #elif defined(SERVO_USE_TIMERB4) && !defined(MILLIS_USE_TIMERB4)
    #ifndef TCB4
      #error "SERVO_USE_TIMERB4 defined, but this part doesn't have that timer!"
    #endif
    #define USE_TIMERB4
  #elif (defined(SERVO_USE_TIMERB1) || defined(SERVO_USE_TIMERB2) || defined(SERVO_USE_TIMERB3) || defined(SERVO_USE_TIMERB4)) && defined(MILLIS_USE_TIMERB0)
    #error "Servo is configured to use timer used by millis. Servo needs full use of a type B timer."
  #else
    #define USE_TIMERB0
  #endif

  static volatile TCB_t* _timer =
  #if defined(USE_TIMERB0)
    &TCB0;
  #elif defined(USE_TIMERB1)
    &TCB1;
  #elif defined(USE_TIMERB2)
    &TCB2;
  #elif defined(USE_TIMERB3)
    &TCB3;
  #elif defined(USE_TIMERB4)
    &TCB4;
  #endif

  typedef enum {
      timer0,
      _Nbr_16timers } timer16_Sequence_t;


#endif  /* __SERVO_TIMERS_H__ */
