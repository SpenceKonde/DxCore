/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdint.h>
/**
 * Empty yield() hook.
 *
 * This function is intended to be used by library writers to build
 * libraries or sketches that supports cooperative threads.
 *
 * Its defined as a weak symbol and it can be redefined to implement a
 * real cooperative scheduler.
 */
static void __empty() {
  // Empty
}
void yield(void) __attribute__ ((weak, alias("__empty")));

/* Hooks to get in real early. onPreMain runs in init3,
 * (it gets called by _initthreestuff, which is setting IVSEL if we are using a fake bootloader section,
 * checking reset flags, if not using optiboot. If none, dirty reset occurred so do a clean software reset
 * and start over, otherwise stash the reset flags we saw in GPIOR0, clear them, and call onPreMain();)
 *
 * In pre-main, you are operating under caveman conditions. There is no timekeeping, the clock is not running at the speed you think it is, so not
 * even the blocking delays will give correct times. No class constructors have been called, In fact, the previous paragraph is pretty much
 * all that has been done, other than clearing r1 so we can execute compiled C code ()
 */
void onPreMain() __attribute__((weak, alias("__empty")));

/* onBeforeInit() after class constructors but before any hardware is initialized per core settings. */
void onBeforeInit() __attribute__((weak, alias("__empty")));

/* onAfterInit() is called after the init routines, immediately before enabling interrupts. Setup() is called after this.
 * And that's really the main purpose of this function - it gives you a way to tell the initialization routines "Hey, don't enable interrupts yet"
 * You can prevent interrupts from being enabled by returning a non-zero value
 */
uint8_t onAfterInit() __attribute__((weak));
uint8_t onAfterInit() {
  return 0;
}
