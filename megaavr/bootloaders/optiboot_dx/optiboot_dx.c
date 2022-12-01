/**********************************************************/
/* Optiboot bootloader for AVR Dx-series                  */
/* This fork is based on the version that was used for    */
/* tinyAVRs in megaTinyCore, and MegaCoreX adapted to the */
/* once-more all new NVM facilities in the Dx-series.     */
/*                                                        */
/* Like those, Dx-series has a conceptually similar       */
/* NVMCTRL - but only erase is done by the page. Writes   */
/* can be done by the word with SPM or the byte with ST.  */
/* Either way, data is written immediately.               */
/*                                                        */
/*                                                        */
/*   Supports DA, DB and will need almost nothing to      */
/*   add support for DD                                   */
/*                                                        */
/*   Writes by word, not byte. Not troubled by the flash  */
/*   errata on the AVR128DA                               */
/*                                                        */
/*   Fits in 512 bytes                                    */
/*                                                        */
/*   Higher baud rate speeds up programming               */
/*                                                        */
/*   Written almost entirely in C                         */
/*                                                        */
/*   Customisable timeout with accurate timeconstant      */
/*                                                        */
/*   Implements a skeleton STK500 protocol which is       */
/*     missing several features including EEPROM          */
/*     programming and non-page-aligned writes            */
/*                                                        */
/*   Hardened against case where execution reached 0x0000 */
/*     without a reset (error condition or naive way to   */
/*     jump to the bootloader                             */
/*                                                        */
/* This version is included as part of DxCore             */
/*                                                        */
/* Copyright 2020-2021 by Spence Konde                    */
/* Copyright 2013-2019 by Bill Westfield.                 */
/* Copyright 2010 by Peter Knight.                        */
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/*                                                        */
/**********************************************************/


/**********************************************************/
/*                                                        */
/* Optional defines:                                      */
/*                                                        */
/**********************************************************/
/*                                                        */
/* BIGBOOT:                                              */
/* Build a 1k bootloader, not 512 bytes. This turns on    */
/* extra functionality.                                   */
/*                                                        */
/* BAUD_RATE:                                             */
/* Set bootloader baud rate.                              */
/*                                                        */
/* LED_START_FLASHES:                                     */
/* Number of LED flashes on bootup.                       */
/*                                                        */
/* LED_DATA_FLASH:                                        */
/* Flash LED when transferring data. For boards without   */
/* TX or RX LEDs, or for people who like blinky lights.   */
/*                                                        */
/* TIMEOUT_MS:                                            */
/* Bootloader timeout period, in milliseconds.            */
/* 500,1000,2000,4000,8000 supported.                     */
/*                                                        */
/* UART:                                                  */
/* UART number (0..n) for devices with more than          */
/* one hardware uart (644P, 1284P, etc)                   */
/*                                                        */
/**********************************************************/
/**********************************************************/
/* Version Numbers!                                       */
/*                                                        */
/* Arduino Optiboot now includes this Version number in   */
/* the source and object code.                            */
/*                                                        */
/* Version 3 was released as zip from the optiboot        */
/*  repository and was distributed with Arduino 0022.     */
/* Version 4 starts with the arduino repository commit    */
/*  that brought the arduino repository up-to-date with   */
/*  the optiboot source tree changes since v3.            */
/*    :                                                   */
/* Version 9 splits off the Mega0/Xtiny support.          */
/*  This is very different from normal AVR because of     */
/*  changed peripherals and unified address space.        */
/*                                                        */
/* It would be good if versions implemented outside the   */
/*  official repository used an out-of-seqeunce version   */
/*  number (like 104.6 if based on based on 4.5) to       */
/*  prevent collisions.  The CUSTOM_VERSION=n option      */
/*  adds n to the high version to facilitate this.        */
/*                                                        */
/**********************************************************/

/**********************************************************/
/* Edit History:                                          */
/* Oct 2021                                               */
/*     Fixed comments so it doesn't talk about the 328p   */
/*     and reflects what this version does.               */
/* Aug 2020                                               */
/*     Adapted to Dx-series -Spence Konde                 */
/* Aug 2019                                               */
/* 9.0 Refactored for Mega0/Xtiny from optiboot.c         */
/*   :                                                    */
/* 4.1 WestfW: put version number in binary.              */
/**********************************************************/
/* *INDENT-OFF* - astyle hates optiboot                   */
/**********************************************************/

#define OPTIBOOT_MAJVER 9
#define OPTIBOOT_MINVER 1

/*
 * OPTIBOOT_CUSTOMVER should be defined (by the makefile) for custom edits
 * of optiboot.  That way you don't wind up with very different code that
 * matches the version number of a "released" optiboot.
 */


#if !defined(OPTIBOOT_CUSTOMVER)
  #define OPTIBOOT_CUSTOMVER 0x11
#endif


unsigned const int __attribute__((section(".version"))) __attribute__((used))
optiboot_version = 256 * (OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;


#include <inttypes.h>
#include <avr/io.h>

/*
 * AVR Dx-series write-from-app
 * The classic way:
 *   1. didn't fit.
 *   2. carried unnecessary baggage function call and C vs asm stuff (writing is always done in asm
 *      because it requires data to be placed in r0 and r1. But r1 needs to always contain zero or
 *      avr-gcc breaks).
 *   3. Has no API documentation, so I don't know how to use it anyway.
 *
 * The *only* thing that needs to run from the bootloader section is the actual instruction
 * that writes to or erases flash, that is, the spm instruction (or for certain cases, st - but we don't
 * concern ourselves with those since the most commonly found part in the hands of our target users
 * is the AVR128DA, and writing through flash-mapping is busted on theose. The NVMCTRL.CTRLA register can be
 * written from anywhere, unlike the tinyAVRs. so in the snippet of assembly to write a word, we would only need to change the spm instruction to an appropriate call instruction, and all will be right with the world.
 * This method consists of just the minimum two instructions in the bootloader section,
 */

#ifndef APP_NOSPM
  const unsigned long int __attribute__((section(".spmtarg"))) __attribute__((used)) magic_number = 0x950895F8UL;
  // spm z+ ret: use the SPM instruction and increment Z, and return.
  // spm z+ is better than straight spm, because it allows z write across the 64k barrier (spm z+ will
  // increment RAMPZ, which would otherwise require fiddly and inefficient code to test for and handle, or
  // one would just declare "no block writes across 64K". But using spm z+, we get that for free.
#else
  const unsigned long int __attribute__((section(".spmtarg"))) __attribute__((used)) magic_number = 0x95080000UL;
  // nop ret: do nothing, then return. The 0x0000 is an unambiguous way to signal that it is disabled
#endif

// For debugging purposes
//#define WRITE_MAPPED_BY_WORD // Forces even parts with fully mapped flash to write by words instead of bytes.
//#define READ_WITHOUT_MAPPED // Forces parts with fully mapped flash to use lpm to read the flash.

#if PROGMEM_SIZE > 65536
  #define AVOID_SPMZPLUS
#endif

#if (!defined(__AVR_XMEGA__)) || ((__AVR_ARCH__ != 102) && (__AVR_ARCH__ != 103) && (__AVR_ARCH__ != 104))
#error CPU not supported by this version of Optiboot.
#include <unsupported>  // include a non-existent file to stop compilation
#endif

/* section about fuses removed because dated to pre-Dx days and was wrong */

/*
 * optiboot uses several "address" variables that are sometimes byte pointers,
 * sometimes word pointers. sometimes 16bit quantities, and sometimes built
 * up from 8bit input characters.  avr-gcc is not great at optimizing the
 * assembly of larger words from bytes, but we can use the usual union to
 * do this manually.  Expanding it a little, we can also get rid of casts.
 */
typedef union {
  uint8_t  *bptr;
  uint16_t *wptr;
  uint16_t word;
  uint8_t bytes[2];
} addr16_t;


/*
 * pin_defs.h
 * This contains most of the rather ugly defines that implement our
 * ability to use UART=n and LED=D3, and some avr family bit name differences.
 */
#include "pin_defs_dx.h"

/*
 * stk500.h contains the constant definitions for the stk500v1 comm protocol
 */
#include "stk500.h"

#ifndef LED_START_FLASHES
  #define LED_START_FLASHES 0
#endif

/*
 * The AVR Dx-series parts all reset to running on the internal oscillator
 *  at 4 MHz (the internal oscillator speed is selectable here, unlike
 *  older generations). This is much like the tinyAVR 0/1/2 and megaAVR
 *  0-series, but even simpler, since there's no fuse to account for.
 * The BRG divisor is also fractional, able to generate accurate baud rates
 *  very close to the maximum. Maximum and minimums shown here for variout
 *  clock speeds. Same UART as other modern AVRs from 2016-2020.
 * https://drive.google.com/file/d/1xszDrr9pD9FcKedqMCcb_GMc14Gofy-R/view?usp=sharing
 */
#ifndef BAUD_RATE
  #define BAUD_RATE   115200L // Highest rate Avrdude win32 will support
#endif
#ifdef F_CPU
  #warning F_CPU is ignored for this chip (run from internal osc.)
#endif
#ifdef SINGLESPEED
  #warning SINGLESPEED ignored for this chip.
#endif
#ifdef UART
  #warning UART is ignored for this chip (use UARTTX=PortPin instead)
#endif

// DX series starts up at 4 MHz; we use it and leave it at that speed.

#define BAUD_SETTING_4 (((4000000) * 64) / (16L * BAUD_RATE))
#define BAUD_ACTUAL_4 ((64L * (4000000)) / (16L * BAUD_SETTING))

#if BAUD_SETTING_4 < 64   // divisor must be > 1.  Low bits are fraction.
  #error Unachievable baud rate (too fast) BAUD_RATE
#endif

#if BAUD_SETTING_4 > 65635
  #error Unachievable baud rate (too slow) BAUD_RATE
#endif // baud rate slow check

/*
 * Watchdog timeout translations from human readable to config vals
 */
#ifndef WDTTIME
  #define WDTPERIOD WDT_PERIOD_1KCLK_gc  // 1 second
#elif WDTTIME == 1
  #define WDTPERIOD WDT_PERIOD_1KCLK_gc  // 1 second
#elif WDTTIME == 2
  #define WDTPERIOD WDT_PERIOD_2KCLK_gc  // 2 seconds
#elif WDTTIME == 4
  #define WDTPERIOD WDT_PERIOD_4KCLK_gc  // 4 seconds
#elif WDTTIME == 8
  #define WDTPERIOD WDT_PERIOD_8KCLK_gc  // 8 seconds
#else
#endif

/*
 * We can never load flash with more than 1 page at a time, so we can save
 * some code space on parts with smaller pagesize by using a smaller int.
 */
#if MAPPED_PROGMEM_PAGE_SIZE > 255
  typedef uint16_t pagelen_t;
  #define GETLENGTH(len) len = getch()<<8; len |= getch()
#else
  typedef uint8_t pagelen_t;
  #define GETLENGTH(len) (void) getch() /* skip high byte */; len = getch()
#endif


/* Function Prototypes
 * The main() function is in init9, which removes the interrupt vector table
 * we don't need. It is also 'OS_main', which means the compiler does not
 * generate any entry or exit code itself (but unlike 'naked', it doesn't
 * suppress some compile-time options we want.)
 */

// Don't need pre_main with new write-from-app system...
// void pre_main(void) __attribute__ ((naked)) __attribute__ ((section (".init8")));


int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9"))) __attribute__((used));

void __attribute__((noinline)) __attribute__((leaf)) putch(char);
uint8_t __attribute__((noinline)) __attribute__((leaf)) getch(void) ;
void __attribute__((noinline)) verifySpace();
void __attribute__((noinline)) watchdogConfig(uint8_t x);
void nvm_cmd(uint8_t cmd);
static void getNch(uint8_t);

#if LED_START_FLASHES > 0
  static inline void flash_led(uint8_t);
#endif

#define watchdogReset()  __asm__ __volatile__ ("wdr\n")

#if (__AVR_ARCH__==103 && !defined(WRITE_MAPPED_BY_WORD)) //fully memory mapped flash
  static inline void writebuffer(int8_t memtype, addr16_t mybuff, addr16_t address, pagelen_t len);
#else
  static inline void writebuffer(int8_t memtype, addr16_t mybuff, addr16_t address, uint8_t len);
  //len is in words, and 0 will be rolled over to 255 before it's checked....
#endif

static inline void read_flash(addr16_t, pagelen_t len);

//#define TRY_USING_EEPROM

/*
 * RAMSTART should be self-explanatory.  It's bigger on parts with a
 * lot of peripheral registers.
 * Note that RAMSTART (for optiboot) need not be exactly at the start of RAM.
 */
#if !defined(RAMSTART)  // newer versions of gcc avr-libc define RAMSTART
  #error RAMSTART not defined.
#endif


/* C zero initialises all global variables. However, that requires */
/* These definitions are NOT zero initialised, but that doesn't matter */
/* This allows us to drop the zero init code, saving us memory */
static addr16_t buff = {(uint8_t *)(RAMSTART)};

/* main program starts here */
int main (void) {
  uint8_t ch;
  /*
   * Making these local and in registers prevents the need for initializing
   * them, and also saves space because code no longer stores to memory.
   * (initializing address keeps the compiler happy, but isn't really
   *  necessary, and uses 4 bytes of flash.)
   */
  register addr16_t address;
  register pagelen_t  length;

  // This is the first code to run.
  //
  // Optiboot C code makes the following assumptions:
  //  No interrupts will execute
  //  SP points to RAMEND
  __asm__ __volatile__ ("clr __zero_reg__"); // known-zero required by avr-libc

  // Here is the reset cause logic:
  // We always clear the reset cause immediately before jumping to the app, stashing it in GPR.GPR0.
  // This makes sure we can honor the reset entry conditions even if the user code doesn't touch
  // the reset cause register (99.99% of arduino code does not).
  // This means that there should only ever be one reset flag other than WDRF - except maybe PORF and BORF in
  // the event of very slow rising power. We use the WDT to reset out of optiboot (without having cleared
  // the flags previously). So if that's set, we clear+stash RSTFR and jump to app.
  // Note that if WDRF is set, we also know that the last reset was from that, because if WDRF is set,
  // we clear RSTFR.
  // POR is optional - unless you've disabled reset, you probably want to start the app immediately on POR
  // but if you have, you probably do. BORF without PORF suggests flaky power.
  // and if no reset flags were set, either app jumped to 0x0000 (presumably to run bootloader)
  // or there's no app present and execution is skidding along the empty app section and wrapping
  // around - either way, we should run the bootloader.

  // 11/14:
  // NASTY bug - we also need to check for no reset flags being set (ie, direct entry)
  // and run bootloader in that case, otherwise bootloader won't run, among other things, after fresh
  // bootloading.
  //
  // 11/29:
  // 1. Realized we never took this branch because makefile never sets START_APP_ON_POR when it should.
  // 2. Realized (!(ch & (~RSTCTRL_BORF_bm))) is the same as (ch==RSTCTRL_BORF_bm) - but saves flash!
  // 3. Realized (!(ch & (~(RSTCTRL_BORF_bm | RSTCTRL_PORF_bm)))), because PORF is bit 0 and BORF is bit 1
  // and we already check that ch is not 0... so that works out to be the same as (ch < 4)
  // on a theoretical part where those bits were in different positions, this wouldn't be valid.
  // but on these parts it is: saves flash.
  ch = RSTCTRL.RSTFR;   // get reset cause

  if (ch == 0) {
    // If app jumped direct to bootloader, we are in dangerous territory; the peripherals could be
    // a million flavors of bad! We assume everything is freshly reset. So, to stay safe, reset
    // immediately via software reset. Since this is an entry condition, we will achieve the naive
    // fool's intent (directly entering the bootloader).
    // We can also end up here on a freshly bootloaded chip, and
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
  /* Entry conditions supported:
   * Cond:      Exclude:  Require:
   * _extr      WDRF      EXTRF,SWRF,orUPDIRF
   * _extronly  WDRF      EXTRF
   * _all       WDRF      !BORF
   * _poronly   WDRF      PORF
   * _swronly   WDRF      SWRF
   * Require: 0x34, 0x04, 0x35, 0x01, 0x10.
   */
  #if defined(ENTRYCOND_REQUIRE)
    // If WDRF is set OR nothing except BORF and/or PORF is set, which are not entry conditions.
    // If this isn't true, EXTRF, SWRF, or UPDIRF set; all are entry condiions.
    if (ch & RSTCTRL_WDRF_bm || ((ch & ENTRYCOND_REQUIRE) == 0) ) {
  #else //No require specified, treat as all.
    if (ch & RSTCTRL_WDRF_bm || ((ch & 0x35) == 0)) {
  #endif
    // Start the app.
    // Dont bother trying to stuff it in r_2, which requires heroic effort to fish out
    // Note 11/30/21: Mangled name of that second register so the asm can be searched to see which regs are unused.
    // we'll put it in GPIOR0 (aka GPR.GPR0) where it won't get stomped on.
    RSTCTRL.RSTFR=ch; //clear the reset causes before jumping to app...
    GPR.GPR0 = ch;    // but, stash the reset cause in GPIOR0 for use by app...
    // 11/29
    // We used to turn off the WDT here - but on these parts, unlike classic AVR, the WDT is not
    // forced on when we reboot from WDT. We can only be here if we last reset was from WDT
    // or was BOR or POR
    // (if it wasn't the last reset that was from WDT, then we'd have gotten here and cleared it when it was)
    // So the WDT is ONLY on if it is fused on, in which case we can't turn it off anyway!
    // thus this call is unnecessary! Save 2 instruction words!
    // watchdogConfig(WDT_PERIOD_OFF_gc);
    __asm__ __volatile__ (
      "jmp app\n"
    );
  } // end of jumping to app

  // Set up watchdog to trigger after a bit
  // No reason not to do this early.
  // Nominally:, 1s for autoreset, 8s for manual reset
  watchdogConfig(WDTPERIOD);

  // 11/29/20
  // Removed "FANCY_RESET_LOGIC" code that we didn't use.
  // We used to reset the WDT here. It is unclear why we felt a need to do that.
  // The only time we could be here without a reset is if app jumped direct to
  // bootloader. In this case reset flags will be empty, which triggers a
  // defensive reset. since the peripherals could be hosed.
  // Hence, if WDT is on, it was fused on so we can't touch it
  // but we just got out of reset less than 3 us ago, so don't have to worry

  MYUART_TXPORT.DIR |= MYUART_TXPIN; // set TX pin to output
  //MYUART_TXPORT.OUT |= MYUART_TXPIN;  // and "1" as per datasheet
  MYUART_RXPINCTRL = 0x08;

  #if (defined(MYUART_PMUX_VAL) && MYUART_PMUX_VAL != 0)
    MYPMUX_REG = MYUART_PMUX_VAL;  // alternate pinout to use
  #endif
  // Saves SIX BYTES! LDI 0 and an STS to the high byte!
  // Why the compiler wastes a word on the ldi 0 is a mystery to me, since it knows it's got a 0 right there in r1 ready to go...
  #if (BAUD_SETTING_4 < 256)
    MYUART.BAUDL = BAUD_SETTING_4;
  #else
    MYUART.BAUD = BAUD_SETTING_4;
  #endif
  //MYUART.DBGCTRL = 1;  // run during debug
  MYUART.CTRLC = (USART_CHSIZE_gm & USART_CHSIZE_8BIT_gc);  // Async, Parity Disabled, 1 StopBit
  //MYUART.CTRLA = 0;  // Interrupts: all off
  MYUART.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

  #if (LED_START_FLASHES > 0) || defined(LED_DATA_FLASH) || defined(LED_START_ON)
    /* Set LED pin as output */
    LED_PORT.DIR |= LED;
  #endif

  #if LED_START_FLASHES > 0
    /* Flash onboard LED to signal entering of bootloader */
    # ifdef LED_INVERT
      flash_led(LED_START_FLASHES * 2+1);
    # else
      flash_led(LED_START_FLASHES * 2);
    # endif
  #else
    #if defined(LED_START_ON)
      #ifndef LED_INVERT
        /* Turn on LED to indicate starting bootloader (less code!) */
        LED_PORT.OUT |= LED;
      #endif
    #endif
  #endif

  /* Forever loop: exits by causing WDT reset */
  for (;;) {
    /* get character from UART */
    ch = getch();

    if (ch == STK_GET_PARAMETER) {
      unsigned char which = getch();
      verifySpace();
      /*
       * Send optiboot version as "SW version"
       * Note that the references to memory are optimized away.
       */
      if (which == STK_SW_MINOR) {
        putch(optiboot_version & 0xFF);
      } else if (which == STK_SW_MAJOR) {
        putch(optiboot_version >> 8);
      } else {
        /*
         * GET PARAMETER returns a generic 0x03 reply for
         * other parameters - enough to keep Avrdude happy
         */
        putch(0x03);
      }
    } else if (ch == STK_SET_DEVICE) {
        // SET DEVICE is ignored
        getNch(20);
    } else if (ch == STK_SET_DEVICE_EXT) {
        // SET DEVICE EXT is ignored
        getNch(5);
    } else if (ch == STK_LOAD_ADDRESS) {
        // LOAD ADDRESS
        address.bytes[0] = getch();
        address.bytes[1] = getch();
        // byte addressed!
        verifySpace();
    } else if (ch == STK_UNIVERSAL) {
      #if defined(RAMPZ) && PROGMEM_SIZE > 65536
        // LOAD_EXTENDED_ADDRESS is needed in STK_UNIVERSAL for addressing more than 128kB
        // 128k? There.
        if ( AVR_OP_LOAD_EXT_ADDR == getch() ) {
          // get address
          getch();  // get '0'
          RAMPZ = getch();  // get address and put it in RAMPZ (not shifted, we're getting byte addresses here!)
          getNch(1); // get last '0'
          // response
          putch(0x00);
        } else {
          getNch(3);
          putch(0x00);
        }
      #else
        // There's no RAMPZ because it has less than 128k of flash
        getNch(4);
        putch(0x00);
      #endif
    /* Write up to 1 page of flash (or EEPROM, except that isn't supported due to space) */
    } else if (ch == STK_PROG_PAGE) {
      uint8_t desttype;
      uint8_t *bufPtr;
      // Starting from about here, the compiler starts generating abysmal code!
      GETLENGTH(length);
      #if (__AVR_ARCH__==103 && !defined(WRITE_MAPPED_BY_WORD))
        pagelen_t savelength;
        savelength = length;
      #else
        uint8_t savelength;
        savelength = length>>1;
        //in the event of a full page on DA/DB, this will get truncated to 0! But that is okay!
      #endif
      desttype = getch();

      /* Having worked with SerialUPDI now and having a better idea of what the Dx
       * series is capable of: This way is dumb. We are only ingesting data at 115 baud.
       * That is something like 9us per bit, so 90 per byte and 180 per word. It only takes 70 us
       * to write a word, so could have it written before even half of the following word has arrived.
       * Serial ports keep working while writing to flash - you just can't react to them in that time
       * We have 2 bytes of buffer (RXDATA, and a buffer behind it), plus the shift register. So as
       * long as you can write pages faster than they come in, you don't need to make a buffer in RAM
       * At 115200 baud, we exceed it by nearly a factor of 3. Removing that would save a few bytes
       * BUT WE WOULD NEED TO HAVE ERASED THE PAGE!!
       */
      // read a page worth of contents
      bufPtr = buff.bptr;
      do {
        *bufPtr++ = getch();
      } while (--length);

      // Read command terminator, start reply
      verifySpace();
      // end of abysmal compiler output....
      writebuffer(desttype, buff, address, savelength);

    /* Read memory block mode, length is big endian.  */
    } else if (ch == STK_READ_PAGE) {
      uint8_t desttype;
      GETLENGTH(length);

      desttype = getch();

      verifySpace();

      #if (__AVR_ARCH__==103) && !defined(READ_WITHOUT_MAPPED)
        // handle fully mapped flash - duplicating the test for type of memory
        // is far more readable than putting it before the #if macro
        // set address.word to point to start of mapped flash
        if (desttype == 'F') {
          address.word += MAPPED_PROGMEM_START;
        } else {
          address.word += MAPPED_EEPROM_START;
        }
        do {
          putch(*(address.bptr++));
        } while (--length);
      #else
        // the entire flash does not fit in the same address space
        // so we call that helper function.
        if (desttype == 'F') {
          read_flash(address, length);
        } else { // it's EEPROM and we just read it
          address.word += MAPPED_EEPROM_START;
          do {
            putch(*(address.bptr++));
          } while (--length);
        }
    #endif
    /* Get device signature bytes  */
    } else if (ch == STK_READ_SIGN) {
      // Easy, they're already in a mapped register... but we know the flash size at compile time, and it saves us 2 bytes of flash for each one we don't need to know...
      verifySpace();
      putch(0x1E);          // why even bother reading this, ever? If it's running on something, and the first byte isn't 0x1E, something weird enough has happened that nobody will begrudge you a bootloader rebuild!
      #if (PROGMEM_SIZE==131072)  // need different binary for 128k vs 64k vs 32k-and-under anyway, as 32k-and-under is all mapped, 64k isn't mapped, but doesn't have RAMPZ and 128k has RAMPZ...
        putch(0x97);
      #elif (PROGMEM_SIZE==65536)
        putch(0x96);
      #else
        #if defined(__AVR_DD__)
          putch(SIGROW_DEVICEID1); // There is both 16k DD-series vs 32k DD-series.
        #else
        putch(0x94); // but only 32k DA/DB
        #endif
      #endif
      putch(SIGROW_DEVICEID2);
    } else if (ch == STK_LEAVE_PROGMODE) { /* 'Q' */
      // Adaboot no-wait mod
      watchdogConfig(WDT_PERIOD_8CLK_gc);
      verifySpace();
    } else {
      // This covers the response to commands like STK_ENTER_PROGMODE
      verifySpace();
    }
    putch(STK_OK);
  }
}

void putch (char ch) {
  while (0 == (MYUART.STATUS & USART_DREIF_bm))
    ;
  MYUART.TXDATAL = ch;
}

uint8_t getch (void) {
  uint8_t ch, flags;
  while (!(MYUART.STATUS & USART_RXCIF_bm))
    ;
  flags = MYUART.RXDATAH;
  ch = MYUART.RXDATAL;
  if ((flags & USART_FERR_bm) == 0)
  watchdogReset();
  #ifdef LED_DATA_FLASH
    LED_PORT.IN |= LED;
  #endif
  return ch;
}

void getNch (uint8_t count) {
  do getch(); while (--count);
  verifySpace();
}

void verifySpace () {
    if (getch() != CRC_EOP) {
  watchdogConfig(WDT_PERIOD_8CLK_gc);    // shorten WD timeout
  while (1)            // and busy-loop so that WD causes
      ;              //  a reset and app start.
    }
    putch(STK_INSYNC);
}

#if LED_START_FLASHES > 0
  void flash_led (uint8_t count) {
    uint16_t delay;  // at 4MHz, a 16bit delay counter is enough
    while (count--) {
      LED_PORT.IN |= LED;
      // delay based on 4 MHz clock since that's what we have
      /* This delay is calculated from 4,000,000 Hz CPU clock and the
       * desired frequency (15 Hz), and the duration of the loop (10 clocks)
       */
      for (delay = ((4000000U)/150); delay; delay--) {
        watchdogReset();
        if (MYUART.STATUS & USART_RXCIF_bm) {
          return;
        }
      }
    }
    watchdogReset(); // for breakpointing
  }
#endif


/*
 * Change the watchdog configuration.
 *  Could be a new timeout, could be off...
 */
void watchdogConfig (uint8_t x) {
  while(WDT.STATUS & WDT_SYNCBUSY_bm)
    ;  // Busy wait for sycnhronization is required!
  _PROTECTED_WRITE(WDT.CTRLA, x);
}


/*
 * void writebuffer(memtype, buffer, address, length)
 */
#if (__AVR_ARCH__==103) && !defined(WRITE_MAPPED_BY_WORD)
  static inline void writebuffer(int8_t memtype, addr16_t mybuff, addr16_t address, pagelen_t len) {
    switch (memtype) {
      #if (defined(BIGBOOT) && BIGBOOT) || defined(TRY_USING_EEPROM)
        case 'E': // EEPROM
          address.word += MAPPED_EEPROM_START;
          nvm_cmd(NVMCTRL_CMD_EEERWR_gc);
          while(len--) {
            *(address.bptr++)= *(mybuff.bptr++);
          }
          break;
        #endif
      default:  // FLASH
      {
        /*
         * Default to writing to Flash program memory.  By making this
         * the default rather than checking for the correct code, we save
         * space on chips that don't support any other memory types.
         * Start the page erase.
         * The CPU is halted during this time, so
         * no need to NVMCTRL.STATUS.
         */
        nvm_cmd(NVMCTRL_CMD_FLPER_gc);
        address.word += MAPPED_PROGMEM_START;
        *(address.bptr)=0xFF;
        nvm_cmd(NVMCTRL_CMD_FLWR_gc);
        while(len--) {
          *(address.bptr++)= *(mybuff.bptr++);
        }
      } // default block
    } // switch
  }
#else
  // Write non-mapped flash the only way we can without being caught by the outstanding errata with
  // flash mapping on AVR128DA
  static inline void writebuffer(int8_t memtype, addr16_t mybuff, addr16_t address, uint8_t len) {
    //VPORTB.OUT=len;
    switch (memtype) {
      #if (defined(BIGBOOT) && BIGBOOT) || defined(TRY_USING_EEPROM)
        case 'E': // EEPROM
          address.word += MAPPED_EEPROM_START;
          nvm_cmd(NVMCTRL_CMD_EEERWR_gc);
          while(len--) {
            *(address.bptr++)= *(mybuff.bptr++);
          }
          break;
      #endif

      default:  // FLASH
        {
          /*
           * Default to writing to Flash program memory.  By making this
           * the default rather than checking for the correct code, we save
           * space on chips that don't support any other memory types.
           */
          /*
           * First, we Start the page erase.
           * The CPU is halted during this time, so
           * there is no need to check NVMCTRL.STATUS!
           * Hmmph - converting all of that to assembly didn't save
           * as much as I had been hoping, just one instruction!
           */
        __asm__ __volatile__ ("ldi r24, 8"                  "\n\t" // page erase
                              "rcall nvm_cmd"               "\n\t" // r25 has now been shat on
                              "spm"                         "\n\t" // erase the page!
                              "ldi r24, 2"                  "\n\t" // page write
                              "rcall nvm_cmd"               "\n\t"
                              "mov r25, %[len]"             "\n\t" // now copy the len, safely passed as read only
                              "head:"                       "\n\t" // to the already shat on r25.
                              "ld r0, %a[ptr]+"             "\n\t"
                              "ld r1, %a[ptr]+"             "\n\t"
        #if !defined(AVOID_SPMZPLUS) && PROGMEM_SIZE > 65536
                              "spm Z+"                      "\n\t"
        #else
                              "spm"                         "\n\t"
                              "adiw r30,2"                  "\n\t"
        #endif
                              "dec r25"                     "\n\t" // and use the copy in r25 to count down.
                              "brne head"                   "\n\t"
                              "clr r1"                      "\n\t"
                              : "+z" ((uint16_t)address.word), [ptr] "+e" ((uint16_t)mybuff.bptr): [len]   "l" (len): "r0", "r24", "r25"); // and declare r25 clobbered
        // which we would always be required to do if we call something that clobbers a register from asm.
      } // default block
    } // switch
  }
#endif

void nvm_cmd(uint8_t cmd) {
  //take advantage of the fact that NVMCTRL_CMD_NONE_gc=0x00 and use zero_reg
  //Also, we don't need to change NVMCTRL.CTRLA to NONE/NOOP until we want to change it.
  // 11/20/22: AAaarrgghhh! Okay so... We can't let the compiler pick the random
  // register that we use to store the ccp spm magic number, 0x9D, because we are calling this from
  // asm in one place (immediately above), for two reasons. First, the reason I discovered this
  // the compiler was picking r25 to give len to me above. Then I called nvmcmd, and it trashed
  // the value in len, which was legal, because that's a call clobbered register.
  // Secondly, we would need to either declare every call clobbered register as a clobber,
  // or save and restore every register, unless we knew which registers were being shat on.
  // So what do we do? We can't save and restore every register, and we don't want to clobber everything
  // the above function gets inlined, and the compiler would then need to save everything it needs,
  // but we don't have the flash for that either. Remember, we only have 6 instruction words tops.
  // So - remove the request for the compiler to give that constant to us in a register. Instead, we pick r25 for it,
  // use it, and declare it clobbered. THEN, in the above routine, we instead ask for len as a read-only operand, and then
  // copy it to r25, use that normally, and again declare r25 clobbered.
  // We know that the command will be in r24, since that's where it starts, and the compiler won't
  // move a register if inline asm call
  __asm__ __volatile__("ldi r25, 0x9D"               "\n\t"
                       "out %[ccp], r25"             "\n\t"
                       "sts %[ioreg], r1"            "\n\t"
                       "out %[ccp], r25"             "\n\t"
                       "sts %[ioreg], %[cmd]"        "\n\t"
                       :
                       : [ccp]          "I" (_SFR_IO_ADDR(CCP)),
                         [ioreg]        "n" (_SFR_MEM_ADDR(NVMCTRL.CTRLA)),
                         [cmd]          "r" ((uint8_t)cmd)
                       : "r25");
}

static inline void read_flash(addr16_t address, pagelen_t length)
{
  uint8_t ch;
  do {
    #if defined(RAMPZ) && PROGMEM_SIZE > 65536
      // Since RAMPZ should already be set, we need to use EPLM directly.
      // Also, we can use the autoincrement version of lpm to update "address"
      //      do putch(pgm_read_byte_near(address++));
      //      while (--length);
      // read a Flash and increment the address (may increment RAMPZ)
      __asm__ ("elpm %0,Z+\n" : "=r" (ch), "+z" (address.bptr));
    #else
      // read a Flash byte and increment the address
      __asm__ ("lpm %0,Z+\n" : "=r" (ch), "+z" (address.bptr));
    #endif
    putch(ch);
  } while (--length);
}







#ifdef BIGBOOT
/*
 * Optiboot is designed to fit in 512 bytes, with a minimum feature set.
 * Some chips have a minimum bootloader size of 1024 bytes, and sometimes
 * it is desirable to add extra features even though 512bytes is exceedded.
 * In that case, the BIGBOOT can be used.
 * Our extra features so far don't come close to filling 1k, so we can
 * add extra "frivolous" data to the image.   In particular, we can add
 * information about how Optiboot was built (which options were selected,
 * what version, all in human-readable form (and extractable from the
 * binary with avr-strings.)
 *
 * This can always be removed or trimmed if more actual program space
 * is needed in the future.  Currently the data occupies about 160 bytes,
 */
#define xstr(s) str(s)
#define str(s) #s
#define OPTFLASHSECT __attribute__((section(".fini8")))
#define OPT2FLASH(o) OPTFLASHSECT const char f##o[] = #o "=" xstr(o)


#ifdef LED_START_FLASHES
OPT2FLASH(LED_START_FLASHES);
#endif
#ifdef LED_DATA_FLASH
OPT2FLASH(LED_DATA_FLASH);
#endif
#ifdef LED_START_ON
OPT2FLASH(LED_START_ON);
#endif
#ifdef LED_NAME
OPTFLASHSECT const char f_LED[] = "LED=" LED_NAME;
#endif

#ifdef SUPPORT_EEPROM
OPT2FLASH(SUPPORT_EEPROM);
#endif

#ifdef BAUD_RATE
OPT2FLASH(BAUD_RATE);
#endif
#ifdef UARTTX
OPTFLASHSECT const char f_uart[] = "UARTTX=" UART_NAME;
#endif

OPTFLASHSECT const char f_date[] = "Built:" __DATE__ ":" __TIME__;
#ifdef BIGBOOT
OPT2FLASH(BIGBOOT);
#endif
OPTFLASHSECT const char f_device[] = "Device=" xstr(__AVR_DEVICE_NAME__);
#ifdef OPTIBOOT_CUSTOMVER
  #if OPTIBOOT_CUSTOMVER != 0
OPT2FLASH(OPTIBOOT_CUSTOMVER);
  #endif
#endif
OPTFLASHSECT const char f_version[] = "Version=" xstr(OPTIBOOT_MAJVER) "." xstr(OPTIBOOT_MINVER);

#endif

// Dummy application that will loop back into the bootloader if not overwritten
// This gives the bootloader somewhere to jump, and by referencing otherwise
//  unused variables/functions in the bootloader, it prevents them from being
//  omitted by the linker, with fewer mysterious link options.
void  __attribute__((section( ".application")))
      __attribute__((naked)) app();
void app()
{
    uint8_t ch;

    ch = RSTCTRL.RSTFR;
    RSTCTRL.RSTFR = ch; // reset causes
    *(volatile uint16_t *)(&optiboot_version);   // reference the version
    *(volatile uint32_t *)(&magic_number);   // reference the version
    //do_nvmctrl(0, NVMCTRL_CMD_NOOP_gc, 0); // reference this function!
    //__asm__ __volatile__ ("jmp 0");    // similar to running off end of memory
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1); // cause new reset - doesn't this make more sense?!
}
