/* main.cpp - Main loop for Arduino sketches
   Part of DxCore - github.com/SpenceKonde/DxCore
   Copyright (c) 2018~2022 Spence Konde, (c) 2005-2013 Arduino
   Free Software - LGPL 2.1, please see LICENCE.md for details */

#include <Arduino.h>
/* Required by some libraries to compile successfully. Even though it's nonsense in Arduino. */
int atexit(void ( * /*func*/)()) { return 0; }

/* What the bloody hell is going on? Why does everything fall down around my ears if disable
 * millis is set and these functions are located in any other file. What is different?!
 * It doesn't seem to be recognizing the "default" definition, because it compiles to a call
 * pointed at 0x0000, which constitutes directly requesting a dirty reset, and that is indeed
 * what you get. And it bootloops very rapidly as a result, because that call haoppens at
 * OnBeforeInit(). As the name suggests, that's the callback called before init() is called
 * to set up the hardware */

void __attribute__((weak)) initVariant() {return;} // Reserved for library use.
void __attribute__((weak)) onPreMain() {return;}   // Called in .init3. Remember to bring your stone tools and loincloth
// you must survive under the most primitive of conditions. No millis, no delay, F_CPU is defined to a speed other than the
// frequency it's running at, constructors have not run, etc.
void __attribute__((weak)) onBeforeInit() {return;} // Called before init. You can step up to maybe the bronze age, but
// things are still pretty rough, because the init() function has not been called and that sets up the clock, the timers
// and a wide variety of other things.
uint8_t __attribute__((weak)) onAfterInit() {return 0;} // Called between init() and the call to sei() to enable interrupts immediately prior to jumping to main.
// Unlike the others, this one has a return value. If you return a 1, sei() will not be called.


#if defined(LOCK_FLMAP)
  inline void doFLMAP() {
    uint8_t temp = 0;
    #if defined(FLMAPSECTION11)
      #error "No parts have been released with this much flash, and they'd have to change the layout of the register to make that work, so we can't guess what they'll do"
    #elif defined(FLMAPSECTION10)
      #error "No parts have been released with this much flash, and they'd have to change the layout of the register to make that work, so we can't guess what they'll do"
    #elif defined(FLMAPSECTION9)
      #error "No parts have been released with this much flash, and they'd have to change the layout of the register to make that work, so we can't guess what they'll do"
    #elif defined(FLMAPSECTION8)
      #error "No parts have been released with this much flash, and they'd have to change the layout of the register to make that work, so we can't guess what they'll do"
    #elif defined(FLMAPSECTION7)
      temp = 0x70;
      #warning "No parts have been released with this much flash as of time of writing. Assuming that bit 6 of NVMCTRL.CTRLB is FLMAP2 and everything else is the same"
    #elif defined(FLMAPSECTION6)
      temp = 0x60;
      #warning "No parts have been released with this much flash as of time of writing. Assuming that bit 6 of NVMCTRL.CTRLB is FLMAP2 and everything else is the same"
    #elif defined(FLMAPSECTION5)
      temp = 0x50;
      #warning "No parts have been released with this much flash as of time of writing. Assuming that bit 6 of NVMCTRL.CTRLB is FLMAP2 and everything else is the same"
    #elif defined(FLMAPSECTION4)
      temp = 0x40;
      #warning "No parts have been released with this much flash as of time of writing. Assuming that bit 6 of NVMCTRL.CTRLB is FLMAP2 and everything else is the same"
    #elif defined(FLMAPSECTION3)
      temp = 0x30;
    #elif defined(FLMAPSECTION2)
      temp = 0x20;
      #pragma message("PROGMEM_MAPPED points to section 2 of the flash")
    #elif defined(FLMAPSECTION1)
      temp = 0x10;
      #pragma message("PROGMEM_MAPPED points to section 1 of the flash")
    #elif defined(FLMAPSECTION0)
      temp = 0x00;
      #pragma message("PROGMEM_MAPPED points to section 0 of the flash")
    #else
      #warning "no FLMAP section defined, yet LOCK_FLMAP IS!"
      temp = 0x30; // should always end up as the highest up to 3.
    #endif
    NVMCTRL.CTRLB = temp;
    temp |= 0x80;
    _PROTECTED_WRITE(NVMCTRL_CTRLB, temp);
  }
#else
  #pragma message("Notice: PROGMEM_MAPPED not available as flash mapping is not locked.")
#endif

int main()  __attribute__((weak));
/* The main function - call initialization functions (in wiring.c) then setup, and finally loop *
 * repeatedly. If SerialEvent is enabled (which should be unusual, as it is no longer a menu    *
 * option even, that gets checked for after each call to loop). Note that _pre_main() is        *
 * called first in non-optiboot configurations (neither is needed on Optibooot configurations() *
 * an extra bit of initialization code in .init3 to fix the vectors and still happen if user    *
 * overrides main. In the past there was a USB-related function here, that is removed, as work  *
 * will be needed in any event at the core level if VUSB-based "stuff" arrives, but really I'm  *
 * just waiting for the DU-series now                                                           */
int main() {
  onBeforeInit(); // Emnpty callback called before init but after the .init stuff. First normal code executed
  init(); // Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  // enable interrupts.
  setup();
  for (;;) {
    loop();
  }
}




#if (!defined(USING_OPTIBOOT))



/*********************************** CHECK RESET FLAGS ******************************************/
/* If we are not using Optiboot, we need to check the reset flagss, and reset via software for  *
 * a clean start. Unfortunately, if we clear the registers here, we'll prevent user code from   *
 * seeing them, which isn't helpful. As documented in the reset guide, we suggest overriding     *
 * this function with your own version. One example is included below and others in that guide  *
 * init_reset_Flags() should be overridden with one of the ones from the reset guide in any     *
 * production code.                                                                             *
 * If using optiboot, this will never be called, because Optiboot does the same thing.          *
 * By the time app runs, the flags will have been cleared and moved to GPIOR0* (it needs to     *
 * clear flags to honor bootloader entry conditions, so I didn't have a choice about that.      *
 * This function is called before *anything* else, so the chip is a blank slate - or it's       *
 * state is unknown. You're probably running at 4 MHz unless it was a dirty reset, in which     *
 * case it could be anything. No timekeeping is possible, period. The only exception is the     *
 * WDT reset timer with is independent of the HF oscillators and is designed to reset you out   *
 * of hangs amd bad states fthat you end up with when a bug causes the code but not the         *
 * hardware to reset,                                                                           *
 * Interrupts are disabled, or in event of dirty reset                                          *
 * LVL0EX bit will block them. In the event of a clean reset, nothing is set up.  There is no   *
 * PWM, no timekeeping of any millis/micros/delay see no time passing and and all delays,       *
 * delay_microseconds, _delay_ms() and _delay_us(), are the wrong length because they are       *
 * based on F_CPU.                                                                              *
 * If you end up here from a dirty reset, you know nothing about the configuration of the       *
 * peripherals. Check the flags, save them if you need them, and maybe turn on an LED while     *
 * waiting for the WDT to trigger. If you're debugging something really nasty, you can try to   *
 * gather data about the nature of the fault. For example, turn on an LED if\ LVL0EX is set     *
 * meaning you got here from a missing ISR. With one of those little boards with 6 LEDs on      *
 * (many are available reasonably cheaply on aliexpress et al.) end up being very useful        *
 * for this sort of thing.
 *
 * * The register in question is GPIOR0 on megaTinyCore, GPR.GPR0 on Dx-series, but both names  *
 * are aliases of each other per core_devices for compatibility
 */

  /* Minimum: Reset if we wound up here through malfunction - this relies on user clearing the  *
   * register on startup, which is rarely done in Arduino land.                                 */
  void __attribute__((weak)) init_reset_flags() ;
  void __attribute__((weak)) init_reset_flags() {
    uint8_t flags = RSTCTRL.RSTFR;
    RSTCTRL.RSTFR = flags;
    if (flags == 0) {
      _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
    }
    GPIOR0 = flags;
  }

#endif

/* If using SPM from app, but not actually using Optiboot, we need to point the vector tables in the right place.
 * since the "application" is actually split across "boot" and "application" pages of flash... and it's vectors
 * are all in the section defined as "boot" section, tell the interrupt controller that, otherwise nothing'll work!
 * This could just as well be set in init() but for the fact that we support overriding main(). I don't know if
 * anyone who is doing that wants to use my flashwrite library, but it seems plausible.
 * And while we way you need to take full responsibility for setting up the part if you do, nobody is going
 * to figure this out; that's not a reasonable expectation.
 * We also at the same time make sure there's a reset flag. We can't clear it, even though that
 * needs to be done becauwe then it wouldn't be there if user needed it. But we will document the
 * need to clear it and suggest overriding init_reset_flags(), and give the examples.
 */

/* So we need to do 1 or 2 things - as long as we're not using Optiboot, we should force a      *
 * software reset if we don't see any reset flags on startup - init_reset_flags() does that,    *
 * Then if we're using SPM from app, we need to also flip the it that move s the interrupts     *
 * to the start of flash.
 */

/**************************************************************************************************
 * INITIALIZATION FUNCTIONS LOCATED ANYWHERE SPECIAL GO HERE!                                     *
 *                                                                                                *
 * They *MUST* be declared with both the ((naked)) ahd ((used)) attributes! Without the latter,   *
 * the optimizer will eliminate them. Without the former, the sketch will not start...            *
 * Wait what? Yeah, it was generating and outputting a ret instruction, which caused the          *
 * sketch to return to nowhere under certain conditions and never reach main() at all.            *
 * I do not understand how the old vector fixer allowed the sketch to start ever... but           *
 * since it was only compiled in when flash write was enabled it could have been missed for a     *
 * long time.                                                                                     *
 **************************************************************************************************/

void _initThreeStuff() __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".init3")));
#if (!defined(USING_OPTIBOOT))
  #if !defined(SPM_FROM_APP)
    // If we're not doing the SPM stuff, we need only check the flags
    void _initThreeStuff() {
      init_reset_flags();
      #if defined(LOCK_FLMAP)
        doFLMAP();
      #endif
      onPreMain(); // this runs, as the name implies, before the main() function is called - and indeed, even before class constructors.
    }
  #else
  /*******************************************
  * THIS FUNCTIONALITY IS ONLY EXPOSED ON    *
  * DX-SERIES PARTS SO THIS CODE CANT-HAPPEN *
  * megaTinyCore. You must write to flash    *
  * using optiboot if required               *
  *******************************************/
    // if we are, we also need to move the vectors IFF we allow unrestricted writes only.
    #if (SPM_FROM_APP == -1)
      void _initThreeStuff() {
        init_reset_flags();
        #if defined(LOCK_FLMAP)
          doFLMAP();
        #endif
        _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
        onPreMain();
      }
    #else
      void _initThreeStuff() {
        init_reset_flags();
        #if defined(LOCK_FLMAP)
          doFLMAP();
        #endif
        onPreMain();
      }
    #endif
    #if (SPM_FROM_APP == -1)
      #if defined(__AVR_EA__) || defined(__AVR_EB__)
        #warning "SPM from app not yet supported."

        void __spm_entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".trampolines")));
        void __spm_entrypoint (void)
        {
          __asm__ __volatile__(
                    "rjmp .+4"                "\n\t" // Jump over this if we got here the wrong way
                   "EntryPointSPM:"           "\n\t" // this is the label we call
                    "nop"                     "\n\t" // If this worked, there would have to be code here,
                    "ret"::);                        // by 2, and then return.
        }
      #else
        /* Declared as being located in .trampolines so it gets put way at the start of the binary. This guarantees that
         * it will be in the first page of flash. Must be marked ((used)) or LinkTime Optimization (LTO) will see
         * that nothing actually calls it and optimize it away. The trick of course is that it can be called if
         * the user wants to - but it's designed to be called via constructions like:
         * __asm__ __volatile__ ("call EntryPointSPM" : "+z" (uint16_t) (address);
         * see Flash.h
         * This must be located before 0x0200 in the flash. The interrupt vectors go from 0x0004 to 0x0103 (0x0000-0x0003 is
         * the reset vector). The only isn that matters is the spm z+. If it were easier to modify the crt.s file, we could use some
         * interrupt vector we weren't using (like the NMI vector, which is never enabled by the core and can only ever trigger
         * from CFD int (which can also be a normal interrupt) and checksum errors (which we don't support because we would then
         * need to post-process the hex files to add checksums)) - then we could replace the jmp instruction dictated by the CRT
         * with the spm z+, ret seqauence. But we can't easily change that, as it's one of the things precompiled by the toolchain
         * package. So we need to work out another way for those 4 bytes (actually, we only care bout the first 2 bytes, the spm)
         * to be located within those remaining 248 bytes of the first page. The priorities given to different sections however
         * don't make this easy: PROGMEM has an extremely high priority, and can of course be longer than 242 bytes. So it has
         * to come before PROGMEM, which comes before even .init. Only place I could find to put it was this - trampolines.
         * .trampolines comes before progmem. The function must be ((naked)) and it must be ((used)). But that's not enough
         * because sometimes execution passes through .trampolines, and would run into this, execute an SPM instruction
         * (failing because the NVMCTRL.CTRLA hadn't been set), and then return. But this was happening before a return address
         * (or anything else) had been pushed onto the stack, so the SP still pointed to the top of flash, so ret would not work.
         * To make this work, you need to guard it. This is done with the rjmp that hops over the spm and ret instructions unless
         * you jump directly to the entrypoint.
         */
        void __spm_entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".trampolines")));
        void __spm_entrypoint (void)
        {
          __asm__ __volatile__(
                    "rjmp .+4"                "\n\t" // Jump over this if we got here the wrong way
                   "EntryPointSPM:"           "\n\t" // this is the label we call
                    "spm z+"                  "\n\t" // write r0, r1 to location pointed to by r30,r31 with posrincrement
                    "ret"::);                        // by 2, and then return.
        }
      #endif // end platform specific.
    #endif // end if spm from app == -1
  #endif // end if spm from app defined
  // Finally, none of these three things need to be done if running optiboot!
  // We want the vectors in the alt location, it checks, clears, and stashes the reset flags (in GPR0)
  // and it providews the entrypoint we call to write to flash.
#else
  void _initThreeStuff() {
    #if defined(LOCK_FLMAP)
      doFLMAP();
    #endif
    onPreMain();
  }
#endif
