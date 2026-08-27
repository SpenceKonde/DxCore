# AVRDU CDC Bootloader

USB CDC bootloader for AVR DU-series microcontrollers, primarily the
**AVR64DU32 Curiosity Nano (EV59F82A)**.  Lets `avrdude -c arduino` push
sketches over the on-target USB port using the same 1200 bps touch reset
convention as Arduino Leonardo / Pro Micro.

## Provenance and license

Clean-room implementation.  No source from Optiboot, LUFA, TinyUSB,
V-USB, or any other USB or AVR bootloader project was consulted while
writing this code.  References are limited to:

- USB 2.0 specification
- USB CDC 1.20 / PSTN 1.20 specifications
- AVR64DU32 datasheet (Microchip DS40002676)
- Atmel AVR061 - STK500 Communication Protocol Application Note
- Microchip ATPACK device headers (`avr/io.h`, group/group-mask defines)

License: LGPL 2.1 (to match the host DxCore repository).  See `LICENSE.md`.

## Building

The DxCore distribution bundles the exact toolchain we developed and
test against (`tools/avr-gcc/7.3.0-atmel3.6.1-azduino8`).  From inside
this directory:

```
make TOOLROOT=../../../../tools
```

This produces:

```
usbcdcboot_64du32.elf
usbcdcboot_64du32.hex
usbcdcboot_64du32.lst
usbcdcboot_64du32.map
```

To check that the build fits in the 4 KB BOOT section:

```
make TOOLROOT=../../../../tools size
```

Sample output (target):

```
=== Bootloader size vs 4096-byte budget ===
  .text = 3380
  .data = 36 (init image)
  total = 3416 bytes
  free  = 680 bytes
```

## Burning to a Curiosity Nano

The Curiosity Nano has an on-board nEDBG that exposes UPDI.  From
Arduino IDE:

1. **Tools > Board > DxCore > AVR DU-series Curiosity Nano (USB CDC Bootloader)**
2. **Tools > Board > AVR64DU32 Curiosity Nano (EV59F82A)** (sub-menu)
3. **Tools > Programmer > Atmel mEDBG** (or "Curiosity Nano" /
   "nEDBG" depending on your avrdude.conf version)
4. **Tools > Burn Bootloader**

Burn-bootloader writes both the fuse set (`BOOTSIZE=0x08` is the
critical one - it reserves the 4 KB BOOT section) and the
`usbcdcboot_64du32.hex` payload.

After burn, the AVR64DU32 reboots into the bootloader.  Windows / macOS
will enumerate a new "AVRDU CDC Bootloader" COM port (separate from the
nEDBG's own CDC port and separate from the runtime's CDC port).

## Uploading a sketch

With the bootloader resident and the board selected as above, plain
**Sketch > Upload** works.  Arduino IDE's upload flow:

1. Detects current CDC port for the board.
2. Opens it at 1200 bps and drops DTR (the "touch").
3. Waits for the port to reappear (it does, courtesy of `cdc_min`'s
   re-attach after the WDT reset hits the bootloader).
4. Calls `avrdude -c arduino -P <newport> -b 115200 -U flash:w:sketch.hex`.

The runtime application's `usb_cdc.c` participates in step 2 - if the
host opens the COM port at 1200 baud and then drops DTR, it sets the
`AVRDU_BL_MAGIC_STAY = 0xB007` magic word at SRAM address `0x7FFE` and
triggers an 8 ms watchdog reset.  On reboot the bootloader sees the
magic word and stays resident.

## What this bootloader supports

| Feature                             | Supported?      |
|-------------------------------------|-----------------|
| Flash write via avrdude `-c arduino` | yes            |
| Flash read-back verify              | yes             |
| Signature read (`-Usignature:r`)    | yes             |
| 1200 bps touch from runtime         | yes             |
| EXTRF (RESET button) -> stay in BL  | yes             |
| Blank-app guard                     | yes             |
| EEPROM write/read                   | **no** (use UPDI)|
| FUSE write                          | **no** (use UPDI)|
| USERROW write                       | **no** (use UPDI)|
| Self-update of bootloader           | **no** (silicon block)|
| Suspend / Resume / Remote wakeup    | **no**          |

## Source tree

```
src/
    main.c       reset handler, entry decision, jump-to-app
    usb_min.c    USB peripheral setup, EP table, standard requests
    usb_desc.c   device / config / string descriptors
    cdc_min.c    CDC ACM class requests, bulk pipe pumps
    stk500.c     STK500v1 protocol state machine
    nvm.c        NVMCTRL self-program (erase + word write + read)
Makefile         build rules; see top of file for variables
DESIGN.md        architecture write-up
LICENSE.md       LGPL 2.1
PROVENANCE.md    clean-room provenance statement
```

The `.h` headers describe the public API of each module; `.c` files
hold the implementations.

## Known limitations / future work

- v1 targets AVR64DU32 only.  Adding 32DU32 / 16DU32 needs the
  signature byte table (DEVICEID2) updated and the upload size shrunk
  to match smaller flash.
- The 4 KB BOOT section is a comfortable size budget for AVR64DU32 but
  marginal for smaller-flash siblings.  A 28-pin variant would need a
  rebuild against `28pin-duseries` plus a re-check of the size budget.
- Suspend / Remote Wakeup are unimplemented.  This means hosts that
  aggressively suspend idle CDC devices will see the port go away
  while the bootloader is waiting at startup; avrdude's port-reappear
  loop normally retries fast enough that this is invisible.
