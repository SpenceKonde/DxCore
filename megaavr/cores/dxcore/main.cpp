/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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
#include <Arduino.h>

int atexit(void (* /*func*/ )()) { return 0; }


// Weak empty variant initialization function.
// May be redefined by variant files (we don't
// do that) or other libraries. FreeRTOS is one
// such library.

void initVariant() __attribute__((weak));
void initVariant() { }

// these parts don't have USB - but hey, VUSB
// has been ported to AVRxt, so you never know!

void setupUSB() __attribute__((weak));
void setupUSB() { }

int main(void)
{
  #if (!defined(USING_OPTIBOOT) && defined(SPM_FROM_APP))
    _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
    // since the "application" is actually split across "boot" and "application" pages of flash... and it's vectors
    // are all in the section defined as "boot" section, tell the interrupt controller that, otherwise nothing'll work!
    // This could just as well be set in init() - any time before interrupts are enabled - but this way as much of
    // the stuff pulled in by this as possible is kept in one place.
    // This shit is so hideous, I'll spare the normal core-modifying-people having to deal with it, and they can
    // just imagine there's an actual bootloader, instead spm z+ ret shoved into a section that as I read the gcc docs
    // doesn't even make sense on a <256k processor that doesn't call ijmp or icall anyway!
  #endif
  init();

  initVariant();

#if defined(USBCON)
  USBDevice.attach();
#endif

  setup();

  for (;;) {
    loop();
    #if defined (ENABLE_SERIAL_EVENT)
    if (serialEventRun) serialEventRun();
    #endif
  }
  return 0;
}

// Declared as being located in .trampolines so it gets put way at the start of the binary. This guarantees that
// it will be in the first page of flash. Must be marked ((used)) or LinkTime Optimization (LTO) will see
// that nothing actually calls it and optimize it away. The trick of course is that it can be called if
// the user wants to - but it's designed to be called via hideous methods like
// __asm__ __volatile__ ("call EntryPointSPM" : "+z" (zaddress))
// see Flash.h

#if (!defined(USING_OPTIBOOT) && defined(SPM_FROM_APP) && SPM_FROM_APP==-1)
void entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".trampolines")));
void entrypoint (void)
{
    __asm__ __volatile__(
               "EntryPointSPM:"           "\n\t" // this is the label we call
                "spm z+"                  "\n\t" // write r0, r1 to location pointed to by r30,r31, increment r30
                "ret"::);                        // by 2, and then return.
}
#endif
