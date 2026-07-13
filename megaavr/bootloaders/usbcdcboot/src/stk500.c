/*
 * avrdu_cdc_bl/src/stk500.c
 * --------------------------------------------------------------------
 *  Clean-room implementation.  Reference: Atmel AVR061 STK500
 *  Communication Protocol application note (publicly published).
 *  No code from Optiboot or any other STK500 implementation was
 *  consulted while writing this.
 *
 *  Implements just enough of STK500v1 for avrdude's "-c arduino"
 *  upload protocol to write the application section over USB CDC.
 *
 *  License: LGPL 2.1.
 */

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stk500.h"
#include "cdc_min.h"
#include "nvm.h"

/* Forward decl from main.c. */
extern void bl_exit_via_wdt(void) __attribute__((noreturn));

/* ----------------------------------------------------------
 *  Receive state machine.  STK500v1 messages have the form
 *    CMD [args ...] CRC_EOP
 *  where the CMD byte determines how many argument bytes
 *  follow.  We use a small lookup to remember the number of
 *  argument bytes to absorb.
 * ---------------------------------------------------------- */
typedef enum {
    ST_IDLE,
    ST_READ_ARGS,
    ST_WAIT_EOP,
    ST_PROG_PAGE_HEADER,    /* 3 bytes: size_hi, size_lo, memtype     */
    ST_PROG_PAGE_DATA,      /* size bytes of payload, then CRC_EOP    */
    ST_READ_PAGE_HEADER,    /* 3 bytes: size_hi, size_lo, memtype     */
} state_t;

static state_t  g_state;
static uint8_t  g_cmd;
static uint16_t g_byte_addr;    /* current load address (in BYTES)   */
static uint8_t  g_args_pending;
static uint8_t  g_args[20];     /* SET_DEVICE is the longest arg list */
static uint8_t  g_args_idx;
static uint16_t g_pp_size;
static uint8_t  g_pp_memtype;
static uint8_t  g_page_buf[512];  /* AVR64DU32 page size              */
static uint16_t g_page_buf_idx;

/* ----------------------------------------------------------
 *  Small helpers (TX side) - push individual bytes through
 *  cdc_min's bulk-IN buffer.
 * ---------------------------------------------------------- */
static inline void put1(uint8_t b)            { cdc_min_tx_byte(b); }
static inline void put2(uint8_t a, uint8_t b) { put1(a); put1(b); }
static inline void put_n(const uint8_t *p, uint16_t n) {
    while (n--) put1(*p++);
}

static inline void reply_insync_ok(void)        { put2(STK_INSYNC, STK_OK); }
static inline void reply_insync_x_ok(uint8_t x) { put1(STK_INSYNC); put1(x); put1(STK_OK); }
static inline void reply_nosync(void)           { put1(STK_NOSYNC); }

/* ----------------------------------------------------------
 *  How many argument bytes does each command carry?
 *  Returns 0xFF for "variable length, handled specially".
 * ---------------------------------------------------------- */
static uint8_t cmd_arglen(uint8_t cmd) {
    switch (cmd) {
        case STK_GET_SYNC:        return 0;
        case STK_GET_SIGN_ON:     return 0;
        case STK_GET_PARAMETER:   return 1;
        case STK_SET_PARAMETER:   return 2;
        case STK_SET_DEVICE:      return 20;
        case STK_SET_DEVICE_EXT:  return 5;
        case STK_ENTER_PROGMODE:  return 0;
        case STK_LEAVE_PROGMODE:  return 0;
        case STK_CHIP_ERASE:      return 0;
        case STK_LOAD_ADDRESS:    return 2;
        case STK_UNIVERSAL:       return 4;
        case STK_READ_SIGN:       return 0;
        case STK_PROG_PAGE:       return 0xFF;  /* handled specially */
        case STK_READ_PAGE:       return 0xFF;  /* handled specially */
        default:                  return 0xFE;  /* unknown */
    }
}

/* Forward decls for sentinels invoked from dispatch(). */
static void prog_page_finalise(void);
static void read_page_emit(void);

/* ----------------------------------------------------------
 *  Dispatch a fully-received command (after CRC_EOP).
 * ---------------------------------------------------------- */
static void dispatch(void) {
    /* Sentinels for the PROG_PAGE_DATA / READ_PAGE_HEADER paths land
     * here too - handle them before falling through to the regular
     * command table. */
    if (g_cmd == 0xF0) { prog_page_finalise(); return; }
    if (g_cmd == 0xF1) { read_page_emit();     return; }

    switch (g_cmd) {

        case STK_GET_SYNC:
            reply_insync_ok();
            break;

        case STK_GET_SIGN_ON: {
            /* AVR061 says "AVR STK" (7 bytes) followed by OK. */
            static const uint8_t sig[] = "AVR STK";
            put1(STK_INSYNC);
            put_n(sig, sizeof(sig) - 1);
            put1(STK_OK);
            break;
        }

        case STK_GET_PARAMETER: {
            /* Return a sane stub value for any parameter avrdude asks
             * about.  Real STK500 parameters are not used by the
             * arduino programmer mode. */
            uint8_t param = g_args[0];
            uint8_t value;
            if      (param == 0x80) { value = 1;    } /* HW VER major */
            else if (param == 0x81) { value = 8;    } /* SW VER major */
            else if (param == 0x82) { value = 2;    } /* SW VER minor */
            else                     { value = 0x03; } /* generic; matches optiboot_dx */
            reply_insync_x_ok(value);
            break;
        }

        case STK_SET_PARAMETER:
        case STK_SET_DEVICE:
        case STK_SET_DEVICE_EXT:
            reply_insync_ok();
            break;

        case STK_ENTER_PROGMODE:
            reply_insync_ok();
            break;

        case STK_LEAVE_PROGMODE:
            put2(STK_INSYNC, STK_OK);
            cdc_min_flush();
            /* Give the host a moment to receive the OK, then bail. */
            for (volatile uint32_t i = 0; i < 200000; i++) { /* ~10 ms @24 MHz */ }
            bl_exit_via_wdt();
            /* not reached */
            break;

        case STK_CHIP_ERASE:
            /* avrdude in arduino mode does not actually require this
             * to do anything - the per-page write does erase+program.
             * For safety we acknowledge it without action. */
            reply_insync_ok();
            break;

        case STK_LOAD_ADDRESS:
            /* Little-endian word address. */
            /* AVR Dx/DU: avrdude sends a BYTE address here, not a word
             * address.  Store it as-is. */
            g_byte_addr = (uint16_t)g_args[0] | ((uint16_t)g_args[1] << 8);
            reply_insync_ok();
            break;

        case STK_UNIVERSAL:
            /* avrdude sends low-level SPI commands here; for the DU
             * we have nothing to do, so return a dummy 0x00 byte. */
            reply_insync_x_ok(0x00);
            break;

        case STK_READ_SIGN:
            /* Report the running device's signature read live from SIGROW
             * (datasheet sec. 33 Device IDs), not a build-time constant,
             * so one per-class hex serves every package/flash-size variant
             * sharing this build's RAM layout: avrdude always sees the true
             * DEVICEID of the chip it is talking to (cf. STK_SIG_BYTE_* in
             * stk500.h, kept only as the AVR64DU32 reference value). */
            put1(STK_INSYNC);
            put1(SIGROW.DEVICEID0);
            put1(SIGROW.DEVICEID1);
            put1(SIGROW.DEVICEID2);
            put1(STK_OK);
            break;

        default:
            reply_nosync();
            break;
    }
}

/* ----------------------------------------------------------
 *  PROG_PAGE and READ_PAGE need their own micro-state machine
 *  because the size byte determines how many payload bytes
 *  follow before CRC_EOP.
 * ---------------------------------------------------------- */
static void prog_page_finalise(void) {
    /* Write the buffer at the byte address avrdude loaded (AVR Dx/DU
     * send byte addresses, so no doubling - same for flash and EEPROM). */
    if (g_pp_memtype == 'F' || g_pp_memtype == 'f') {
        /* On AVR Dx/DU, avrdude sends BYTE addresses in STK_LOAD_ADDRESS
         * (confirmed by DxCore Optiboot_dx "byte addressed!").  Use the
         * loaded value directly - do NOT double it. */
        nvm_write_page((uint32_t)g_byte_addr, g_page_buf, g_pp_size);
    } else if (g_pp_memtype == 'E' || g_pp_memtype == 'e') {
        /* EEPROM byte offset, same byte-addressing convention as flash. */
        nvm_write_eeprom(g_byte_addr, g_page_buf, g_pp_size);
    }
    /* Other memtypes are acknowledged without action. */
    reply_insync_ok();
}

static void read_page_emit(void) {
    put1(STK_INSYNC);
    uint32_t byte_addr = (uint32_t)g_byte_addr;
    for (uint16_t i = 0; i < g_pp_size; i++) {
        uint8_t b;
        if (g_pp_memtype == 'F' || g_pp_memtype == 'f') {
            b = nvm_read_byte(byte_addr + i);
        } else if (g_pp_memtype == 'E' || g_pp_memtype == 'e') {
            b = nvm_read_eeprom((uint16_t)(g_byte_addr + i));
        } else {
            b = 0xFF;
        }
        put1(b);
    }
    put1(STK_OK);
}

/* ----------------------------------------------------------
 *  Per-byte feeder.  cdc_min_rx_pop() returns -1 if no byte
 *  is available, otherwise 0..255.
 * ---------------------------------------------------------- */
static void feed_byte(uint8_t b) {
    switch (g_state) {

        case ST_IDLE:
            g_cmd = b;
            {
                uint8_t n = cmd_arglen(g_cmd);
                if (n == 0xFE) {
                    /* Unknown command.  Wait for the EOP to clear the
                     * line, then NOSYNC. */
                    g_state = ST_WAIT_EOP;
                } else if (n == 0xFF) {
                    /* PROG_PAGE / READ_PAGE - read 3-byte header. */
                    g_args_idx     = 0;
                    g_args_pending = 3;
                    g_state = (g_cmd == STK_PROG_PAGE)
                                ? ST_PROG_PAGE_HEADER
                                : ST_READ_PAGE_HEADER;
                } else if (n == 0) {
                    g_state = ST_WAIT_EOP;
                } else {
                    g_args_idx     = 0;
                    g_args_pending = n;
                    g_state = ST_READ_ARGS;
                }
            }
            break;

        case ST_READ_ARGS:
            g_args[g_args_idx++] = b;
            if (--g_args_pending == 0) {
                g_state = ST_WAIT_EOP;
            }
            break;

        case ST_WAIT_EOP:
            if (b == STK_CRC_EOP) {
                dispatch();
            } else {
                reply_nosync();
            }
            g_state = ST_IDLE;
            break;

        case ST_PROG_PAGE_HEADER:
            g_args[g_args_idx++] = b;
            if (--g_args_pending == 0) {
                /* avrdude sends size big-endian. */
                g_pp_size    = ((uint16_t)g_args[0] << 8) | g_args[1];
                g_pp_memtype = g_args[2];
                if (g_pp_size > sizeof(g_page_buf)) {
                    /* Bad packet; abort and re-sync next round. */
                    g_state = ST_WAIT_EOP;
                } else {
                    g_page_buf_idx = 0;
                    if (g_pp_size == 0) {
                        g_state = ST_WAIT_EOP;
                    } else {
                        g_state = ST_PROG_PAGE_DATA;
                    }
                }
            }
            break;

        case ST_PROG_PAGE_DATA:
            g_page_buf[g_page_buf_idx++] = b;
            if (g_page_buf_idx >= g_pp_size) {
                /* All payload bytes received; next byte is CRC_EOP. */
                g_state = ST_IDLE;  /* will be set below before dispatch */
                /* Re-use ST_IDLE -> ST_WAIT_EOP path: peek for next
                 * byte = EOP, then run prog_page_finalise.            */
                g_cmd   = 0xF0;     /* sentinel for finalise           */
                g_state = ST_WAIT_EOP;
            }
            break;

        case ST_READ_PAGE_HEADER:
            g_args[g_args_idx++] = b;
            if (--g_args_pending == 0) {
                g_pp_size    = ((uint16_t)g_args[0] << 8) | g_args[1];
                g_pp_memtype = g_args[2];
                if (g_pp_size > sizeof(g_page_buf)) {
                    g_state = ST_WAIT_EOP;
                } else {
                    g_cmd   = 0xF1;     /* sentinel for read finalise */
                    g_state = ST_WAIT_EOP;
                }
            }
            break;
    }

    /* If we hit the EOP sentinel for a finalise, do it now. */
    if (g_state == ST_IDLE) return;
}

/* ---- Per-byte feeder, continued from feed_byte() below.  The
 *      0xF0 (PROG_PAGE finalise) and 0xF1 (READ_PAGE finalise) sentinel
 *      values used in g_cmd are routed through ST_WAIT_EOP and handled
 *      at the top of dispatch().                                       ---- */


void stk500_init(void) {
    g_state         = ST_IDLE;
    g_byte_addr     = 0;
    g_args_pending  = 0;
    g_args_idx      = 0;
    g_pp_size       = 0;
    g_pp_memtype    = 0;
    g_page_buf_idx  = 0;
    memset(g_args,     0, sizeof(g_args));
    memset(g_page_buf, 0xFF, sizeof(g_page_buf));
}

void stk500_poll(void) {
    int c;
    while ((c = cdc_min_rx_pop()) >= 0) {
        feed_byte((uint8_t)c);
    }
}
