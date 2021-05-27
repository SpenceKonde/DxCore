/* main.cpp - Main loop for Arduino sketches
   Part of DxCore - github.com/SpenceKonde/DxCore
   Copyright (c) 2018~2021 Spence Konde, (c) 2005-2013 Arduino
   Free Software - LGPL 2.1, please see LICENCE.md for details */

#include <Arduino.h>

/* Required by some libraries to compile successfully. Even though it's nonsense in Arduino. */
int atexit(void ( * /*func*/)()) { return 0; }

void initVariant() __attribute__((weak));
/* Weak empty variant initialization function. The purpose is unclear. It sounds like it was intended
 * initialize the variant, and specific variants would have their own implementation. But in practice
 * it seems to be instead used as an initialization callback that libraries can use to run code before
 * setup, like FreeRTOS - it would have been nice if the official API included such a callback. */
void initVariant() { }

int main()  __attribute__((weak));
/* The main function - call initialization functions (in wiring.c) then setup, and finally loop repeatedly.
 * If serial event is enabled (which should be unusual, as it is no longer a menu option even, that gets
 * checked for after each call to loop). Note that if SPM from App without optiboot is enabled, there is
 * an extra bit of initialization code in .init3 to fix the vectors and still happen if user overrides main.
 * In the past there was a USB-related function here, that is removed, as work will be needed in any event
 * at the core level if VUSB-based "stuff" arrives that needs support at this level */
int main() {
  init(); //Interrupts are turned on just prior to init() returning.
  initVariant();
  setup();
  for (;;) {
    loop();
    #ifdef ENABLE_SERIAL_EVENT
      if (serialEventRun) serialEventRun();
    #endif
  }
}

/* If using SPM from app, but not actually using Optiboot, we need to point the vector tables in the right place.
 * since the "application" is actually split across "boot" and "application" pages of flash... and it's vectors
 * are all in the section defined as "boot" section, tell the interrupt controller that, otherwise nothing'll work!
 * This could just as well be set in init() - any time before interrupts are enabled - but this way as much of
 * the stuff related to this is in the same file. We need to do this even if the user overrides main() which we
 *supposedly support. Hence, it gets placed in .init3, so it runs after stack and zero reg are set up. */

#if (!defined(USING_OPTIBOOT) && defined(SPM_FROM_APP))
  void fixVectors() __attribute__((used)) __attribute__ ((section (".init3")));
  void fixVectors() {
    _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
  }
  #if (SPM_FROM_APP==-1)
    /* Declared as being located in .trampolines so it gets put way at the start of the binary. This guarantees that
     * it will be in the first page of flash. Must be marked ((used)) or LinkTime Optimization (LTO) will see
     * that nothing actually calls it and optimize it away. The trick of course is that it can be called if
     * the user wants to - but it's designed to be called via hideous methods like
     * __asm__ __volatile__ ("call EntryPointSPM" : "+z" (zaddress))
     * see Flash.h */
    void __spm_entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".trampolines")));
    void __spm_entrypoint (void)
    {
      __asm__ __volatile__(
               "EntryPointSPM:"           "\n\t" // this is the label we call
                "spm z+"                  "\n\t" // write r0, r1 to location pointed to by r30,r31, increment r30
                "ret"::);                        // by 2, and then return.
    }
  #endif
#endif
