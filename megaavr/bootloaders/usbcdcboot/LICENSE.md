# AVRDU CDC Bootloader — License

This bootloader is distributed under the terms of the **GNU Lesser
General Public License version 2.1** (LGPL 2.1), the same license
that covers the host DxCore project.

A copy of the LGPL 2.1 license text is included in DxCore's top-level
`LICENSE.md`.  In short, the LGPL 2.1 permits the following:

- You may use, copy, modify and distribute this bootloader.
- You may link applications against it without those applications
  having to be LGPL-licensed.
- Modifications to the bootloader itself must remain LGPL-licensed.
- The full source of the bootloader must be available to anyone
  receiving a binary copy.

You are encouraged to upstream improvements via pull request to the
DxCore repository.

## Why LGPL 2.1 and not GPL 2

GPL 2 is the license that covers Optiboot (the existing UART
bootloader in DxCore).  Because this bootloader does **not** copy or
derive from Optiboot code, it is free to choose its own compatible
license.  LGPL 2.1 was chosen so that user sketches running on top of
the bootloader (which technically constitute "linking" via the reset
vector hand-off) are not automatically forced under GPL.

## Author and contact

Clean-room implementation prepared for the DxCore AVR DU integration
effort.  See `PROVENANCE.md` in this directory for the full list of
reference documents consulted while writing this code.

For issues, bug reports, or contributions: open an issue or pull
request against the DxCore repository on GitHub.
