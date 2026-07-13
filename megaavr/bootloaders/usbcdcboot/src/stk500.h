/*
 * avrdu_cdc_bl/src/stk500.h
 * --------------------------------------------------------------------
 *  Clean-room implementation.  Reference: Atmel AVR061 - "STK500
 *  Communication Protocol" application note (publicly published by
 *  Atmel/Microchip).  No source consulted from Optiboot or any other
 *  STK500 implementation.
 *  License: LGPL 2.1.
 */
#ifndef AVRDU_BL_STK500_H
#define AVRDU_BL_STK500_H

#include <stdint.h>

/* Protocol literals from AVR061 section 4 ("Command set"). */
#define STK_OK              0x10
#define STK_INSYNC          0x14
#define STK_NOSYNC          0x15
#define STK_CRC_EOP         0x20

#define STK_GET_SYNC        0x30
#define STK_GET_SIGN_ON     0x31
#define STK_GET_PARAMETER   0x41
#define STK_SET_PARAMETER   0x40
#define STK_SET_DEVICE      0x42
#define STK_SET_DEVICE_EXT  0x45
#define STK_ENTER_PROGMODE  0x50
#define STK_LEAVE_PROGMODE  0x51
#define STK_CHIP_ERASE      0x52
#define STK_LOAD_ADDRESS    0x55
#define STK_UNIVERSAL       0x56
#define STK_PROG_PAGE       0x64
#define STK_READ_PAGE       0x74
#define STK_READ_SIGN       0x75

/* AVR64DU32 signature row from datasheet section 33 (Device IDs).
 * Reference value only: STK_READ_SIGN now reports SIGROW.DEVICEID0..2
 * read at runtime, so the same hex serves all variants of a build class.
 *   SIGROW.DEVICEID0 = 0x1E
 *   SIGROW.DEVICEID1 = 0x96
 *   SIGROW.DEVICEID2 = 0x21 (DU32, per datasheet Table 8-4).
 *   Note: 0x22 is the AVR64DU28 (28-pin) variant.                     */
#define STK_SIG_BYTE_0  0x1E
#define STK_SIG_BYTE_1  0x96
#define STK_SIG_BYTE_2  0x21

void stk500_init(void);
void stk500_poll(void);  /* called from main loop; non-blocking */

#endif  /* AVRDU_BL_STK500_H */
