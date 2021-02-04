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
	init();

	initVariant();

#if defined(USBCON)
	USBDevice.attach();
#endif

	setup();
	for (;;) {
		loop();
    #ifndef NOSERIALEVENT
		if (serialEventRun) serialEventRun();
    #endif
	}
	return 0;
}

#if (!defined(USING_OPTIBOOT) && defined(SPM_FROM_APP))
// Declared as being located in .init3 so it gets put way at the start of the binary. This guarantees that
// it will be in the first page of flash. Must be marked ((used)) or LinkTime Optimization (LTO) will see
// that nothing actually calls it and optimize it away. The trick of course is that it can be called if
// the user wants to - but it's designed to be called via hideous methods like
// __asm__ __volatile__ ("call EntryPointSPM")
// see Flash.h
void entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".init3")));
void entrypoint (void)
{
__asm__ __volatile__(
            "rjmp .+4"                "\n\t" // skip over these when this runs during startup
           "EntryPointSPM:"           "\n\t" // this is the label we call
            "spm z+"                  "\n\t" // write r0, r1 to location pointed to by r30,r31, increment r30
            "ret"::);                        // by 2, and then return.
CPUINT.CTRLA=CPUINT_IVSEL_bm;
// since the "application" is actually split across "boot" and "application" pages of flash... and it's vectors
// are all in the section defined as "boot" section, tell the interrupt controller that, otherwise nothing'll work!
// This could just as well be set in init() - any time before interrupts are enabled - but this way as much of
// the stuff pulled in by this as possible is kept in one place.
}
#endif
