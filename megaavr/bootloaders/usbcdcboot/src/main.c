/*
 * avrdu_cdc_bl/src/main.c
 * --------------------------------------------------------------------
 *  Clean-room implementation.  References:
 *    - AVR64DU32 datasheet (Microchip DS40002676A or later)
 *    - Atmel AVR061 - STK500 Communication Protocol App Note
 *    - USB 2.0 specification, USB CDC PSTN 1.20 specification
 *    - Microchip ATPACK device headers (avr/io.h)
 *  No source from Optiboot, LUFA, TinyUSB, V-USB or any other
 *  bootloader / USB project was consulted while writing this file.
 *
 *  Role:
 *    Entry point of the AVRDU CDC bootloader.  Decides whether to
 *    remain in the bootloader for an avrdude session or jump to the
 *    application at 0x1000, and runs the main poll loop while
 *    resident.
 *
 *  License: LGPL 2.1 (matches the host DxCore repository).
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb_min.h"
#include "cdc_min.h"
#include "stk500.h"
#include "nvm.h"

/* ----- Coordination with the runtime (mirrored in runtime/usb_cdc.c) -----
 *
 *  Stay-in-bootloader handshake: the runtime's 1200 bps touch issues a
 *  SOFTWARE RESET (RSTCTRL.SWRR). That sets RSTCTRL.RSTFR.SWRF, which the
 *  datasheet guarantees is retained across the reset ("After any Reset, the
 *  source that caused the Reset is found in the Reset Flag"). main() stays
 *  resident whenever it sees SWRF.
 *
 *  An earlier scheme used a magic word in GPR1/GPR2 (and before that, SRAM
 *  0x7FFE). Both were dead ends: the datasheet lists the GPRn reset value as
 *  0x00, so the reset clears the GPRs before main() can read them, and 0x7FFE
 *  is clobbered by the first crt0 CALL push (RAMEND is 0x7FFF). The reset
 *  flag is the only store that survives the reset by design.
 *
 *  GPR.GPR0 carries the reset cause to the application (the Optiboot/DxCore
 *  convention); main() writes it there before clearing RSTFR. */
#define APP_RESET_VECTOR_BYTE  0x1000   /* must match BOOTEND in fuse */
#define APP_RESET_VECTOR_WORD  (APP_RESET_VECTOR_BYTE >> 1)

/* Double-tap reset state.  Kept in .noinit so it survives the button-press
 * reset: SRAM is retained across a non-POR reset and crt0 never touches the
 * .noinit section.  DBLTAP_ARMED is a distinctive value, so random SRAM at
 * power-on is very unlikely to look armed - and the POR/BOD path in main()
 * clears it before the first button press can ever be evaluated. */
#define DBLTAP_ARMED  0x6B9Du
static volatile uint16_t s_dbltap_flag __attribute__((section(".noinit")));

/* --------------------------------------------------------------------
 *  DFU-mode LED indicator (default PA7, active LOW)
 *
 *  The LED is driven only while the bootloader is in its "stay" loop, so
 *  the user can see at a glance whether the chip is in BL mode or running
 *  the application.  We encode WHY the BL is staying in the blink rate:
 *     - software reset (1200 bps touch from the host) -> fast ~4-5 Hz
 *     - double-tap on RESET (button)                  -> medium ~2 Hz
 *     - blank application slot                         -> slow ~1 Hz
 *  This is the equivalent of the Caterina/Pro-Micro pulse pattern, but
 *  much simpler: just a counter-driven OUTTGL, no timer needed.
 *
 *  Default pin is PA7, matching DxCore's optiboot LED convention
 *  (LED=A7) for the parts that have it: the 20/28/32-pin DU packages.
 *  The 14-pin DU (16/32DU14) has no PA7, so those builds override the
 *  pin to PD6 -- the same split optiboot uses for its *dd vs *dd14
 *  classes.  There is no PD4 fallback as in optiboot's 14-pin UART
 *  case: this is a USB bootloader, so the LED never collides with a
 *  UART pin position.  Active LOW: drive the pin LOW to light the LED;
 *  if your board's LED is active HIGH, swap OUTCLR/OUTSET in
 *  bl_led_on()/bl_led_off_state() below.  Override the pin at build
 *  time with -DBL_LED_PORT=PORTx -DBL_LED_PIN=n, e.g. PD6 for a 14-pin
 *  DU (-DBL_LED_PORT=PORTD -DBL_LED_PIN=6) or PF2 for the Curiosity
 *  Nano LED0 (-DBL_LED_PORT=PORTF -DBL_LED_PIN=2).
 * -------------------------------------------------------------------- */
#ifndef BL_LED_PORT
#define BL_LED_PORT          PORTA
#endif
#ifndef BL_LED_PIN
#define BL_LED_PIN           7
#endif
#define BL_LED_PIN_BM        (1u << BL_LED_PIN)

static uint16_t s_led_period_counts;   /* main-loop iters per toggle */

static inline void bl_led_init(uint16_t period_counts) {
    s_led_period_counts = period_counts;
    BL_LED_PORT.DIRSET = BL_LED_PIN_BM;
    BL_LED_PORT.OUTSET = BL_LED_PIN_BM;   /* LED off (output high) */
}

static inline void bl_led_deinit(void) {
    BL_LED_PORT.OUTSET = BL_LED_PIN_BM;   /* off */
    BL_LED_PORT.DIRCLR = BL_LED_PIN_BM;   /* back to input */
}

static inline void bl_led_toggle(void) {
    BL_LED_PORT.OUTTGL = BL_LED_PIN_BM;
}

static inline void bl_led_on(void) {
    BL_LED_PORT.OUTCLR = BL_LED_PIN_BM;   /* active LOW */
}

static inline void bl_led_off_state(void) {
    BL_LED_PORT.OUTSET = BL_LED_PIN_BM;
}

/* ------------------------------------------------------------------
 *  Coarse busy-wait calibrated for the 4 MHz reset-default clock.
 *
 *  The double-tap window runs in the decision phase, BEFORE clocks_init()
 *  promotes OSCHF to 24 MHz (the reset-default main clock is OSCHF at
 *  4 MHz with our OSCCFG fuse - datasheet 12.3.3).  A volatile inner loop
 *  compiles to ~4 cycles/iteration, so ~1000 iterations per ms at 4 MHz.
 *  The exact length is not critical; the window only needs to be roughly
 *  half a second.
 * ------------------------------------------------------------------ */
static void bl_wait_ms_4mhz(uint16_t ms) {
    while (ms--) {
        for (volatile uint16_t i = 0; i < 1000U; i++) {
            __asm__ __volatile__("nop");
        }
    }
}

/* Double-tap reset window.  Called on the FIRST external (button) reset,
 * after s_dbltap_flag has been armed: wait briefly with a visible LED
 * flicker that invites a second tap.  If the user presses RESET again
 * within this window, that reset re-enters main() with the flag still
 * armed, and we go straight to DFU.  If the window elapses, the caller
 * disarms and the application is started.
 *
 * ~500 ms is the usual compromise: long enough to double-tap comfortably,
 * short enough that an ordinary single press does not noticeably delay the
 * application restart. */
#define DBLTAP_WINDOW_MS  500U
static void bl_dbltap_window(void) {
    BL_LED_PORT.DIRSET = BL_LED_PIN_BM;
    for (uint8_t i = 0; i < (DBLTAP_WINDOW_MS / 50U); i++) {
        bl_led_on();
        bl_wait_ms_4mhz(25);
        bl_led_off_state();
        bl_wait_ms_4mhz(25);
    }
    BL_LED_PORT.OUTSET = BL_LED_PIN_BM;   /* off */
    BL_LED_PORT.DIRCLR = BL_LED_PIN_BM;   /* release the LED pin for the application */
}

/* ----- WDT timeout we ask the silicon to apply when leaving prog mode ----- */
#define WDT_PERIOD_EXIT_gc  WDT_PERIOD_8CLK_gc  /* ~8 ms */

/* --------------------------------------------------------------------
 *  Vector-table relocation
 *
 *  The AVR DU has IVSEL (in CPUINT.CTRLA) which makes the CPU read
 *  interrupt vectors from the BOOT section instead of APPCODE.  The
 *  bootloader does not actually use any interrupts (we poll), but
 *  setting IVSEL is still required so that the reset vector at 0x0000
 *  takes us into BOOT, not into APPCODE.  This is normally the silicon
 *  default after a reset when running from BOOT, but we make it
 *  explicit so we don't depend on undocumented startup behaviour.
 *  Conversely, before jumping to the app we clear IVSEL so that the
 *  app's own interrupt handlers in APPCODE are reachable.
 * -------------------------------------------------------------------- */
static inline void vectors_to_boot(void) {
    _PROTECTED_WRITE(CPUINT.CTRLA, CPUINT.CTRLA | CPUINT_IVSEL_bm);
}

static inline void vectors_to_app(void) {
    _PROTECTED_WRITE(CPUINT.CTRLA, CPUINT.CTRLA & (uint8_t)~CPUINT_IVSEL_bm);
}

/* --------------------------------------------------------------------
 *  Clock setup - AVR DU needs OSCHF running at one of the
 *  USB-compatible frequencies (12/16/20/24 MHz) and the USB peripheral
 *  selects its own 48 MHz internal oscillator automatically when
 *  enabled.  We pick 24 MHz here for the core.
 *  See datasheet 11.5 (CLKCTRL) and 28.3.4 (USB clocking).
 * -------------------------------------------------------------------- */
static void clocks_init(void) {
    /* Read-modify-write OSCHFCTRLA: keep the AUTOTUNE field's reset
     * value (will be promoted to SOF tracking by usb_min_init), and
     * set FRQSEL to 0x9 = 24 MHz.  Bits [5:2] hold FRQSEL. */
    uint8_t oschf = CLKCTRL.OSCHFCTRLA;
    oschf = (oschf & ~(0x0F << 2)) | CLKCTRL_FRQSEL_24M_gc;
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, oschf);

    /* Disable main clock prescaler so the CPU runs at the full
     * OSCHF rate (24 MHz). */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);

    /* Wait until OSCHF is stable. */
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_OSCHFS_bm)) { /* spin */ }
}

/* --------------------------------------------------------------------
 *  Heuristic: is the application slot blank?
 *
 *  Blank flash reads as 0xFFFF.  If the first instruction of the app
 *  (the RJMP / JMP at byte 0x1000) is 0xFFFF, there is nothing to jump
 *  to and we must stay in the bootloader.  This protects users who
 *  fuse the bootloader before any sketch has been uploaded.
 *
 *  IMPORTANT: byte 0x1000 is in Flash *code* space.  Per datasheet 8.2
 *  / Figure 8-1, data space 0x1000 is the extended I/O area, NOT
 *  Flash.  Flash is visible in data space only via the FLMAP window at
 *  0x8000-0xFFFF.  We therefore use LPM (via pgm_read_word_near) to
 *  read the app reset vector directly from code space, where address
 *  0x1000 means Flash byte 0x1000 regardless of FLMAP.
 * -------------------------------------------------------------------- */
#include <avr/pgmspace.h>
static bool app_appears_invalid(void) {
    return (pgm_read_word_near(APP_RESET_VECTOR_BYTE) == 0xFFFF);
}

/* --------------------------------------------------------------------
 *  Jump to application.
 *
 *  Implementation note (datasheet 15.5.1):
 *
 *    "A system reset will cause the Program Counter to be reset to
 *     0x0000, regardless of the IVSEL bit value."
 *
 *  This rules out a SW reset as the exit path - we would end up back
 *  in the bootloader's reset vector.  Instead we IJMP directly to the
 *  app's reset vector at byte 0x1000 == word 0x0800.  The app's
 *  startup code (crt*) sets up its own SP, copies .data, clears .bss
 *  and lands in main, so we do not need to scrub SRAM here.
 *
 *  AVR64DU32 has a 16-bit PC (max byte address 0xFFFF = word 0x7FFF),
 *  so IJMP via Z is sufficient; EIJMP/EIND is not needed.
 * -------------------------------------------------------------------- */
__attribute__((noreturn))
static void jump_to_app(void) {
    /* Disable interrupts; the app re-enables them when ready. */
    cli();

    /* Turn the DFU LED off and release the LED pin back to input so the app sees
     * a clean GPIO state (and the LED doesn't stick "on" mid-startup). */
    bl_led_deinit();

    /* Clear IVSEL so the CPU reads vectors from APPCODE (start at 0x1000). */
    vectors_to_app();

    /* Quiesce any peripheral we may have started up. Just USB so far. */
    USB0.CTRLA = 0;
    USB0.CTRLB = 0;

    /* Restore FLMAP to its reset value (SECTION3) so the app starts with
     * a cold-boot FLMAP state.  Per datasheet table 11-8, NVMCTRL.CTRLB
     * requires the CCP IOREG key (0xD8). */
    {
        uint8_t new_ctrlb = (NVMCTRL.CTRLB & ~NVMCTRL_FLMAP_gm) | (3u << NVMCTRL_FLMAP_gp);
        _PROTECTED_WRITE(NVMCTRL.CTRLB, new_ctrlb);
    }

    /* IJMP to word address (APP_RESET_VECTOR_BYTE / 2) = 0x0800.
     * Z = (high << 8) | low. */
    __asm__ __volatile__ (
        "ldi r30, %[lo]   \n\t"
        "ldi r31, %[hi]   \n\t"
        "ijmp             \n\t"
        :
        : [lo] "M" ((uint8_t)(APP_RESET_VECTOR_WORD & 0xFF)),
          [hi] "M" ((uint8_t)((APP_RESET_VECTOR_WORD >> 8) & 0xFF))
        : "r30", "r31"
    );

    __builtin_unreachable();
}

/* --------------------------------------------------------------------
 *  Trigger WDT, used after avrdude sends LEAVE_PROGMODE.
 * -------------------------------------------------------------------- */
__attribute__((noreturn))
void bl_exit_via_wdt(void) {
    /* LED off and pin released. The chip is about to WDT-reset and the
     * post-reset app will configure the LED pin as it wishes. The WDT reset sets
     * WDRF (not SWRF), so the next boot runs the freshly uploaded app. */
    bl_led_deinit();

    /* Detach USB so the host sees us go away. */
    usb_min_detach();

    _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_EXIT_gc);
    while (1) { }
}

/* ====================================================================
 *  Entry point
 * ==================================================================== */
int main(void) {
    /* 1. Read the reset cause, hand it to the application via GPR0 (the
     *    Optiboot/DxCore convention - the app is built with -DUSING_OPTIBOOT
     *    so its own init_reset_flags() is compiled out and it relies on us
     *    to have stashed the cause here), then clear the flags so the next
     *    reset's cause is unambiguous and a stale SWRF cannot pin us in DFU. */
    uint8_t  rstfr = RSTCTRL.RSTFR;
    GPR.GPR0 = rstfr;
    RSTCTRL.RSTFR  = 0xFF;

    /* 2. Make sure the CPU is reading vectors from BOOT.  Required for
     *    correct ISR dispatch even though we currently poll. */
    vectors_to_boot();

    /* 3. Decide which path to take.  Priority: the runtime's explicit
     *    request (software reset -> SWRF) wins; then a double tap on the
     *    RESET button; then the blank-app guard; otherwise run the app.
     *
     *    SWRF is the host handshake: the runtime's 1200 bps touch issues a
     *    software reset (RSTCTRL.SWRR), the only ordinary source of SWRF, so
     *    seeing it here means "the host asked for the bootloader". (This
     *    replaced an earlier GPR-magic scheme that could never work, because
     *    the GPRs are reset to 0x00 before main() can read them.)
     *
     *    EXTRF is the button.  Arduino-style double tap: a lone press runs
     *    the application; a second press within a short window enters DFU.
     *    The "armed" state is held in s_dbltap_flag across the second reset. */
    bool stay_in_bl = false;
    uint16_t led_period = 0;   /* main-loop iters per LED toggle */

    if (rstfr & RSTCTRL_SWRF_bm) {
        s_dbltap_flag = 0;
        stay_in_bl = true;
        led_period = 8000U;     /* fast blink (~4-5 Hz) - host (1200bps touch) */
    } else if (rstfr & RSTCTRL_EXTRF_bm) {
        if (s_dbltap_flag == DBLTAP_ARMED) {
            s_dbltap_flag = 0;              /* consume the armed state */
            stay_in_bl = true;
            led_period = 16000U;            /* medium blink (~2 Hz) - double tap */
        } else {
            s_dbltap_flag = DBLTAP_ARMED;
            bl_dbltap_window();             /* ~500 ms; a 2nd reset re-enters main */
            s_dbltap_flag = 0;              /* window elapsed: it was a lone press */
            /* fall through to the app / blank-app handling below */
        }
    } else {
        /* POR / BOD / WDT / undefined: clear any stale double-tap state so a
         * subsequent first press starts clean (POR leaves SRAM random). */
        s_dbltap_flag = 0;
    }

    if (!stay_in_bl) {
        if (app_appears_invalid()) {
            stay_in_bl = true;
            led_period = 32000U;            /* slow blink (~1 Hz) - blank app */
        } else {
            jump_to_app();                  /* normal boot to the application */
        }
    }

    /* 4. Stay in the bootloader.  Bring up the clock tree and USB. */
    clocks_init();
    usb_min_init();
    cdc_min_init();
    stk500_init();
    bl_led_init(led_period);
    usb_min_attach();

    /* 6. Polling loop.  All three modules cooperate via the bulk
     *    OUT/IN pipes managed by usb_min.  stk500_poll() consumes
     *    one byte from CDC RX, runs the state machine, and pushes
     *    responses back to CDC TX. The LED counter is purely a visual
     *    indicator that we are in DFU mode; the encoded blink rate also
     *    tells the user which entry path was taken (magic vs button vs
     *    blank-app). */
    uint16_t led_count = 0;
    for (;;) {
        usb_min_poll();
        cdc_min_poll();
        stk500_poll();
        if (++led_count >= s_led_period_counts) {
            led_count = 0;
            bl_led_toggle();
        }
    }
}
