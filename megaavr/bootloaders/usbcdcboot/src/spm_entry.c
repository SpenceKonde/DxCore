/* spm_entry.c - Application-callable SPM entry point.
 *
 * Generalizes the Optiboot_dx convention (spm z+; ret at the last 6 bytes
 * of the boot section) to this 4 KB bootloader: the stub lives at
 * BOOTSIZE-6 = 0x0FFA and a version word at BOOTSIZE-2 = 0x0FFE.
 * The application's Flash library sets up NVMCTRL.CTRLA and Z (+RAMPZ),
 * then calls 0x0FFA; because the stub executes from the BOOT section it
 * is allowed to program the APPCODE/APPDATA sections.
 *
 * Build with -DAPP_NOSPM to ship the bootloader with this entry disabled
 * (nop; ret - the 0x0000 opcode signals "disabled" to the Flash library).
 */
#include <stdint.h>

#ifndef APP_NOSPM
const uint32_t __attribute__((section(".spmtarg"), used))
  bl_spm_entry = 0x950895F8UL;   /* spm z+ ; ret */
#else
const uint32_t __attribute__((section(".spmtarg"), used))
  bl_spm_entry = 0x95080000UL;   /* nop    ; ret */
#endif

/* Bootloader identity/version word, read by FlashClass::checkWritable().
 * 0x1Axx = Wazamono AVR-DU CDC bootloader family (Optiboot_dx uses 0x19xx). */
const uint16_t __attribute__((section(".blversion"), used))
  bl_version = 0x1A01;
