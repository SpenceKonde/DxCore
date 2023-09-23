#ifndef __CORE_PARAMETERS_H__
#define __CORE_PARAMETERS_H__

#if !defined(DXCORE_NUM)
  #if !(defined(DXCORE_MAJOR) && defined(DXCORE_MINOR) && defined(DXCORE_PATCH) && defined(DXCORE_RELEASED))
    #warning "All of the version defines are missing, please correct your build environment; it is likely failing to define other critical values"
    // Version related defines now handled in platform.txt
  #else
    #define DXCORE_NUM ((DXCORE_MAJOR << 24) + (DXCORE_MINOR << 16) + (DXCORE_PATCH << 8) + DXCORE_RELEASED)
  #endif
#endif
#if !(defined(MEGATINYCORE) || defined(DXCORE) || defined(ATTIYNCORE))

  // This define can get black-hole'ed somehow (reported on platformio) likely the ugly syntax to pass a string define from platform.txt via a -D
  // directive passed to the compiler is getting mangled somehow, though I'm amazed it doesn't cause a  compile error. But checking for defined(DXCore)
  // is the documented method to detect that DxCore is in use, and without it things that tried to do conditional compilation based on that were not
  // recognizing it as DxCore and hence would fail to compile when that conditional compilation was required to make it build.
  // From: https://github.com/adafruit/Adafruit_BusIO/issues/43

  // The whole purpose of this file is largely for for the purpose of being something that can be included anywhere to make sure we know what core we are
  // which becomes more and more important as more code is shared between the cores.

  #define DXCORE "Unknown 1.6.9+"
  #if !defined(DXCORE_NUM)
    #if !(defined(DXCORE_MAJOR) && defined(DXCORE_MINOR) && defined(DXCORE_PATCH) && defined(DXCORE_RELEASED))
      #warning "All of the version defines are missing, please correct your build environment; it is likely failing to define other critical values"
      // Version related defines now handled in platform.txt
    #else
      #define DXCORE_NUM ((DXCORE_MAJOR << 24) + (DXCORE_MINOR << 16) + (DXCORE_PATCH << 8) + DXCORE_RELEASED)
    #endif
  #endif
#else
  #if defined(MEGATINYCORE) || defined(ATTINYCORE)
    #error "This is DxCore being compiled, but DXCORE and/or ATTINYCORE is defined already"
  #endif
#endif

/* Check for invalid options, mostly to help point the PIO and other third party IDE users in the right direction */

#if defined(MILLIS_USE_TIMERA0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERA0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERA1) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERA1 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERA2) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERA2 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB1) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB1 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB2) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB2 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB3) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERA0) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB3 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB4) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB4 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB5) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB5 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB6) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB6 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB7) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERA0) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERB7 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERE0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERF0))
  #error "MILLIS_USE_TIMERE0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERF0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA2) || defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERA0))
  #error "MILLIS_USE_TIMERF0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL)
  #error "A millis timer not supported on this core was passed. this should only be possible on third party IDEs. "
#endif
#if defined(UARTBAUD5V) || defined(UARTBAUD3V)
  #warning "The UARTBAUDxV baud rate correction was removed in 1.3.0 because it had excessive overhead and negligible benefit. The define you specified does nothing."
#endif
#endif
