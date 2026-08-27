# AVRDU CDC Bootloader — Provenance Statement

This file documents the clean-room development of the AVRDU CDC
Bootloader.  It exists so that downstream maintainers can verify the
absence of accidental code reuse from incompatibly-licensed projects.

## Summary

The bootloader was written from public protocol and silicon
specifications **without consulting source code from any other USB
or AVR bootloader project**.  In particular, no source was viewed
from:

- **Optiboot** (the existing UART bootloader in DxCore - GPL 2+)
- **LUFA** (Lightweight USB Framework for AVRs - MIT-with-exception)
- **TinyUSB** (cross-platform USB stack - MIT)
- **V-USB** (software USB for AVRs - GPL 2 or commercial)
- **Arduino USB Host Shield 2.0**
- **Arduino's own ArduinoCore-avr** USB code (for 32u4 / Leonardo)
- **CircuitPython** USB stack
- **NicoHood/HID** library
- **MHeironimus/ArduinoJoystickLibrary**

Code organisation, naming, and comments are original to this work.

## Reference documents (all publicly available)

### USB protocol level

| Document | Use |
|---|---|
| USB 2.0 Specification, chapter 9 (Device Framework) | Standard requests, descriptor types, control transfer state machine |
| USB CDC 1.20 Specification | Class identifiers, functional descriptor framework |
| USB CDC PSTN Subclass 1.20 Specification | ACM class request codes (Table 13), Line Coding format (Table 17) |

### AVR DU silicon level

| Document | Use |
|---|---|
| Microchip DS40002676 (AVR64DU32 datasheet) | NVMCTRL section 11, USB peripheral section 28, RSTCTRL section 12, CLKCTRL section 11.5, fuse layout |
| Microchip DS40002683 (AVR DU family datasheet) | Cross-reference for shared peripherals |
| Atmel AVR061 - STK500 Communication Protocol Application Note | STK500v1 command set (Section 4), used by avrdude's "arduino" programmer mode |
| Microchip ATPACK device headers (`avr/io.h`, `ioavr64du32.h`) | Register and bit-field names |

### avrdude protocol behaviour

The exact byte sequences avrdude emits when invoked as `-c arduino`
were inferred from the AVR061 application note and verified against
the protocol diagrams in the same note.  No source code from avrdude
itself was read.

## Author's statement

I declare that, to the best of my knowledge, every source file in
`src/` is original work derived solely from the reference documents
listed above, and contains no code copied or paraphrased from any of
the projects in the "absence of reuse" list above.

Where the runtime AVRDU CDC stack (the sibling project in this same
repository) shares architectural patterns with this bootloader, the
shared lineage is **the same single clean-room development effort**
that produced both - not a transfer from any external source.

## Per-file provenance header

Every `.c` and `.h` file in `src/` opens with a comment block
explicitly restating that it is a clean-room implementation and listing
which specifications it consulted.  Maintainers introducing new files
to this directory should follow the same convention.

## Verification by review

This bootloader is offered for upstream contribution to DxCore.  If
the DxCore maintainers (or any third party) wish to verify the
clean-room provenance independently, the following are acceptable
review approaches:

1. **Side-by-side comparison** with Optiboot's `optiboot_dx.c` -
   structural and naming differences should be obvious throughout.
2. **Search for any verbatim phrase** longer than ~15 characters in
   the comments or identifiers, against the candidate prior-art
   codebases - none should match.
3. **Audit of the per-file headers** to confirm each file's
   provenance statement is consistent with this document.

If a discrepancy is found, please open an issue.
