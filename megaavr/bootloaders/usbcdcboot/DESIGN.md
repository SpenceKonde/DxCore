# AVRDU CDC Bootloader  EDesign Document

**Target**: AVR64DU32 on Microchip Curiosity Nano (EV59F82A) and similar
boards exposing the AVR DU's native USB-D+/D- to a host PC.

**Goal**: Allow `avrdude -c arduino -P COMx` to write the application
section over the on-chip USB CDC port, using the 1200 bps touch reset
convention shared with Arduino Leonardo / Pro Micro.

**License**: This bootloader is a clean-room implementation. It does not
reuse code from Optiboot, LUFA, TinyUSB, V-USB, or any other USB or AVR
bootloader project. Implementation references are limited to:

 - USB 2.0 specification (chapter 9)
 - USB CDC 1.20 / PSTN 1.20 specifications
 - AVR64DU32 datasheet (DS40002676 rev. A or later)
 - AVR061: STK500 Communication Protocol Application Note
 - Microchip ATPACK device headers (`avr/io.h`, `avr/iousbxxx.h`)

A two-line per-file provenance header in every source file restates this.

---

## 1. Memory Layout

The AVR64DU32 has 64 KB of program flash divided by the BOOTSIZE fuse
into two contiguous regions, with optional APPDATA on top:

```
  0x0000 ┌─────────────────────────────━E         ━E                            ━E         ━E BOOT section (4 KB)        ━E ↁEthis bootloader lives here
         ━E - reset / vector table     ━E         ━E - usb_min_*                ━E         ━E - cdc_min_*                ━E         ━E - stk500_parser            ━E         ━E - nvm_self_program         ━E         ━E                            ━E  0x1000 ├─────────────────────────────┤  ↁEBOOTEND = BOOTSIZE * 512
         ━E                            ━E         ━E APPCODE section (60 KB)    ━E ↁEthe user sketch
         ━E - relocated vector table   ━E         ━E - .text                    ━E         ━E - .data init image         ━E         ━E                            ━E  0xFFFF └─────────────────────────────━E```

`BOOTSIZE = 0x08` (8 blocks ÁE512 B = 4096 B) is the boards.txt fuse
value. The corresponding linker flag is `-Wl,--section-start=.text=0x0`
for the bootloader and `-Wl,--section-start=.text=0x1000` for the app.

SRAM (8 KB, 0x6000..0x7FFF) is shared between bootloader and app at
different times. The bootloader uses a magic word at the very top of
SRAM (`0x7FFE`, two bytes) to coordinate entry mode with the app  Esee
section 3.

---

## 2. Entry Decision

On every reset, the CPU starts at 0x0000 inside the BOOT section. The
bootloader's reset handler must promptly decide between staying in the
bootloader to accept a flash upload, or jumping to the application.

### 2.1 RSTCTRL.RSTFR reset cause flags

The AVR DU records the last reset cause in `RSTCTRL.RSTFR`. The
bootloader reads and clears this register first. Possible flags:

 - `PORF`   Epower-on reset (cold boot)
 - `BORF`   Ebrown-out reset
 - `EXTRF`  Eexternal reset (RESET pin)
 - `WDRF`   Ewatchdog timer reset (used by the 1200 bps touch path)
 - `SWRF`   Esoftware reset
 - `UPDIRF` EUPDI reset (after a chip-erase, app may be invalid)

### 2.2 Magic-word handshake

A two-byte magic at `AVRDU_BL_MAGIC_ADDR = 0x7FFE` distinguishes
"please stay in the bootloader" from "this was just a normal reset":

 - `AVRDU_BL_MAGIC_STAY  = 0xB007`   Erequest set by the app right
                                       before a WDT reset (the avrdude
                                       1200 bps touch path uses this)
 - anything else                       Eproceed to the application

The runtime's `usb_cdc.c` does the following when the host opens the CDC
port at 1200 bps and then drops DTR:

```c
*(volatile uint16_t *)AVRDU_BL_MAGIC_ADDR = AVRDU_BL_MAGIC_STAY;
USB0.CTRLA &= ~USB_ENABLE_bm;          // physical detach
_PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8CLK_gc);  // ~8 ms WDT
while (1) { }                          // wait for reset
```

The 8 ms WDT timeout is long enough for the host to notice the
disconnect, and short enough that avrdude's port-reappear timeout
(several seconds) easily covers the round trip.

### 2.3 Decision flow on bootloader entry

```
on reset:
    save_rstfr = RSTCTRL.RSTFR
    RSTCTRL.RSTFR = 0xFF           // clear all flags
    relocate_vector_table_to_BOOT()

    magic = *(uint16_t*)0x7FFE
    *(uint16_t*)0x7FFE = 0          // consume the request

    if (magic == 0xB007)             goto stay_in_bootloader
    if (save_rstfr & EXTRF)          goto stay_in_bootloader  // user pressed RESET
    if (app_appears_invalid())       goto stay_in_bootloader  // 0xFFFF at 0x1000
    goto jump_to_app

stay_in_bootloader:
    enable_USB()
    run_cdc_stk500_loop()

jump_to_app:
    point_vector_table_back_to_APPCODE()
    ((void (*)(void))0x1000)()       // far jump into app reset vector
```

`app_appears_invalid()` looks for `0xFFFF` at the application's reset
vector (offset 0 of APPCODE), which is what blank flash reads as.

---

## 3. USB CDC subset

The bootloader's USB stack is a subset of the runtime stack already
written in `AVRDU_CDC/`. Features kept:

 - USB 2.0 Full-Speed device, single configuration
 - One control endpoint (EP0)
 - One bulk IN endpoint  (EP1 IN,  64 B, application data ↁEhost)
 - One bulk OUT endpoint (EP2 OUT, 64 B, host ↁEapplication data)
 - SET_LINE_CODING / GET_LINE_CODING (no actual UART, but avrdude reads
   the current line coding to compare against 1200; the bootloader
   doesn't itself need this, but the runtime does)
 - SET_CONTROL_LINE_STATE (DTR / RTS)
 - GET_DESCRIPTOR (device, config, string, language ID)

Features dropped from the runtime stack:

 - Interrupt notification endpoint (EP1 IN bulk replaces it; some hosts
   tolerate a CDC ACM without the notification EP, and avrdude only
   does bulk transfers)
 - HID interfaces and endpoints (no Keyboard/Mouse/Joystick in the BL)
 - Ring buffers (the bootloader processes one packet at a time)
 - Remote wakeup / suspend / resume handling (bootloader stays running)

The descriptor set is rebuilt for the bootloader to advertise only the
two CDC interfaces (control + data), and uses a different product
string ("AVRDU CDC Bootloader") so the host can distinguish.

VID/PID:

 - VID = 0x1209 (pid.codes)
 - PID = 0xDA33 (one more than the runtime's 0xDA32, so it appears as a
   separate COM port and avrdude can target it explicitly)

---

## 4. STK500v1 protocol

avrdude's `-c arduino` driver speaks STK500v1, a small ASCII-byte
protocol documented in Atmel AVR061. Each request is a single command
byte (sometimes with parameters), terminated by `CRC_EOP = 0x20`. The
bootloader replies `INSYNC = 0x14`, optional data, then `OK = 0x10`.

Implemented commands (the minimum avrdude actually uses for the
`arduino` programmer):

| Cmd  | Hex  | What we do                                              |
|------|------|---------------------------------------------------------|
| GET_SYNC          | 0x30 | reply INSYNC, OK                                 |
| GET_SIGN_ON       | 0x31 | reply INSYNC, "AVR STK", OK                      |
| GET_PARAMETER     | 0x41 | reply INSYNC, parameter byte, OK                 |
| SET_PARAMETER     | 0x42 | discard parameter byte; reply INSYNC, OK         |
| SET_DEVICE        | 0x42 | (alias) consume 20 bytes; reply INSYNC, OK       |
| SET_DEVICE_EXT    | 0x45 | consume 5 bytes; reply INSYNC, OK                |
| ENTER_PROGMODE    | 0x50 | reply INSYNC, OK                                 |
| LEAVE_PROGMODE    | 0x51 | reply INSYNC, OK; arm WDT to exit bootloader     |
| CHIP_ERASE        | 0x52 | NVMCTRL chip erase via APP+BOOT all FFs (skipped)|
| LOAD_ADDRESS      | 0x55 | consume 2-byte little-endian word address        |
| UNIVERSAL         | 0x56 | consume 4 bytes; reply INSYNC, 0x00, OK          |
| PROG_PAGE         | 0x64 | consume N+3 (size hi/lo, memtype, payload); write|
| READ_PAGE         | 0x74 | consume 3 (size hi/lo, memtype); reply payload   |
| READ_SIGN         | 0x75 | reply 3-byte signature (1E 96 22 for 64DU32)     |

Unknown commands trigger `NOSYNC = 0x15` and the host re-syncs.

The address loaded by `LOAD_ADDRESS` is a **word** address (ÁE for
bytes). For PROG_PAGE the data is written byte-for-byte to flash at
`load_addr * 2`. For 64 KB flash we must also handle the
`UNIVERSAL` command `0x4D xx xx` which avrdude uses to set the
high-order byte (since STK500v1 addresses are only 16 bits and we have
a 16-bit byte address space already  Ethe AVR64DU32 has a 64 KB
program space which is 32 K *words*, so a 16-bit word address covers
the whole part and the byte-extension command can be safely treated as
a no-op for the 64DU32).

---

## 5. NVMCTRL self-programming

AVR DU NVMCTRL operations are issued via the `CCP` protection
mechanism:

```c
ccp_spm_unlock();             // write 0x9D to CCP
NVMCTRL.CTRLA = NVMCTRL_CMD_FLPER_gc;  // page-erase command
*(volatile uint8_t*)addr = 0; // dummy write triggers operation
while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm) { }

ccp_spm_unlock();
NVMCTRL.CTRLA = NVMCTRL_CMD_FLWR_gc;   // word-write command
while (page_offset < 512) {
    *(volatile uint16_t*)addr = word_from_host;
    addr += 2;
    page_offset += 2;
    while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm) { }
}
```

The 512 B page size is hard-coded for AVR64DU32. The bootloader's
`nvm_write_page()` accepts a 256-word buffer and walks it.

Importantly: code running in BOOT can write to APPCODE/APPDATA but NOT
to BOOT itself. The BOOTLOCK fuse bit is not set (we don't lock the
bootloader against UPDI rewrites), but the silicon block on BOOT
self-write is unconditional. This is desirable  Eit protects the
bootloader from a runaway app.

---

## 6. File / Module Layout

```
bootloaders/usbcdcboot/
    Makefile                  GNU make build rules; produces hex/elf/lst
    usbcdcboot_64du32.hex   compiled bootloader for AVR64DU32
    src/
        main.c                reset handler, RSTFR check, jump_to_app
        usb_min.c             USB peripheral init, EP polling
        usb_min.h
        usb_desc.c            descriptors (device/config/strings)
        usb_desc.h
        cdc_min.c             CDC class request handler
        cdc_min.h
        stk500.c              STK500v1 parser
        stk500.h
        nvm.c                 NVMCTRL self-program routines
        nvm.h
    link/
        avr64du32_bl.x        linker script (.text=0, BOOTEND markers)
    LICENSE.md                MIT (or LGPL2.1 to match DxCore)
    PROVENANCE.md             clean-room provenance statement
    README.md                 build / flash / test instructions
```

---

## 7. Size Budget

| Module          | Estimated bytes | Notes                              |
|-----------------|-----------------|------------------------------------|
| usb_min         | 1400            | EP table init, control transfers   |
| usb_desc        |  300            | device + config + strings          |
| cdc_min         |  400            | line coding, DTR/RTS, bulk pumps   |
| stk500          |  600            | command dispatch + addr/page state |
| nvm             |  250            | erase, word-write, signature read  |
| main + plumbing |  450            | reset, jump, RSTFR, magic, WDT     |
| **Subtotal**    | **3400**        | ~83% of the 4 KB BOOT section      |

Slack: ~700 B for compiler-generated overhead, vector table padding,
and future additions (e.g. EEPROM programming).

If the build overshoots 4 KB, raising `BOOTSIZE` from 0x08 to 0x0C
(6 KB) is the next step; the app's `.text` start moves from 0x1000 to
0x1800. Memory budget allows up to 0x10 (8 KB) before serious
encroachment on app space.

---

## 8. Integration with the runtime CDC stack

Two integration points between bootloader and the runtime application:

### 8.1 1200 bps touch in the runtime

The runtime's `usb_cdc.c` already implements the touch path (DTR drop
while line coding is 1200 baud ↁEset magic, detach, WDT reset). The
only contract addition is the magic address/value:

```c
// usb_cdc.h additions:
#define AVRDU_BL_MAGIC_ADDR 0x7FFE
#define AVRDU_BL_MAGIC_STAY 0xB007
```

### 8.2 Vector table relocation

When the app gains control via the long jump from the bootloader, its
startup code must point the vector table back into APPCODE. The
toolchain's default startup (`crt*`) already does this when the link
script places `.text` at 0x1000 and sets `__vectors` accordingly.
No app source change required.

---

## 9. Open questions / known limitations (v1)

1. **EEPROM** writes via avrdude (`-Ueeprom:w:...`) are not implemented
   in v1. avrdude with `-c arduino` is not commonly used for EEPROM
   anyway; users wanting EEPROM should use UPDI via nEDBG.

2. **FUSE** writes are unsupported. Use UPDI for fuse changes.

3. **USERROW** writes are unsupported. Use UPDI.

4. **48 MHz USB clock**: AVR DU has an internal 48 MHz oscillator
   intended for USB. The bootloader must explicitly select it (via
   `CLKCTRL.MCLKCTRLA`), then unlock the 12/16/20/24 MHz core clock
   via `CLKCTRL.MCLKCTRLB`. This is duplicated in the runtime; the app
   reinits it on startup, but the bootloader must do it independently
   on every cold boot.

5. **Suspend / resume**: ignored. avrdude opens-uses-closes the port
   in well under the 3 ms suspend threshold so this is fine in
   practice.

6. **Self-update of the bootloader**: not supported (silicon-enforced).
   To update the bootloader itself, reprogram via UPDI through nEDBG.

---

## 10. Build & flash

A separate `README.md` in the bootloader directory will spell out the
exact build steps. The high level is:

```
cd hardware/megaavr/1.6.2/bootloaders/usbcdcboot
make TOOLROOT=../../../../tools             # uses DxCore's avr-gcc 7.3.0
# produces usbcdcboot_64du32.hex

# burn via nEDBG:
arduino-cli --burn-bootloader --board "DxCore:megaavr:avrduusb" \
    --programmer "atmel_ice"   # or "nedbg", "serialupdi", etc.
```

After "burn bootloader" succeeds, the AVR64DU32 reboots into the
bootloader, the host enumerates a new "AVRDU CDC Bootloader" COM port,
and avrdude can then push the user's sketch using the standard
Leonardo-style 1200 bps touch dance.
