# avrdu_cdc_bl/Makefile
# --------------------------------------------------------------------
#   Build rules for the AVRDU CDC bootloader.
#   Produces usbcdcboot_64du.hex, .elf, .lst, .map (one hex per build
#   class; see build_all_du.bat for the 16du/16du14/32du/32du14/64du set).
#
#   Usage from inside this directory:
#
#       make TOOLROOT=../../../../tools           # use DxCore's toolchain
#       make clean
#       make all                                  # default target
#       make size                                 # report .text size vs 4 KB budget
#
#   The DxCore toolchain ships avr-gcc 7.3.0 / atmel3.6.1 / azduino8 under
#   tools/avr-gcc/<ver>/bin/avr-gcc.  If TOOLROOT is not set we look for it
#   on $PATH.  License: LGPL 2.1.
# --------------------------------------------------------------------

MCU      := avr64du32
F_CPU    := 24000000UL
TARGET   := usbcdcboot_64du
BOOT_BASE := 0x0000
BOOTSIZE  := 4096           # bytes reserved for the bootloader (BOOTSIZE fuse * 512)

# --- Toolchain ------------------------------------------------------
ifeq ($(strip $(TOOLROOT)),)
CC := avr-gcc
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
SIZE    := avr-size
else
AVR_GCC_DIR := $(firstword $(wildcard $(TOOLROOT)/avr-gcc/*))
CC      := $(AVR_GCC_DIR)/bin/avr-gcc
OBJCOPY := $(AVR_GCC_DIR)/bin/avr-objcopy
OBJDUMP := $(AVR_GCC_DIR)/bin/avr-objdump
SIZE    := $(AVR_GCC_DIR)/bin/avr-size
endif

# --- Sources --------------------------------------------------------
SRCS := src/main.c \
        src/spm_entry.c \
        src/usb_min.c \
        src/usb_desc.c \
        src/cdc_min.c \
        src/stk500.c \
        src/nvm.c

OBJS := $(SRCS:.c=.o)

# --- Compile / link flags -------------------------------------------
# --- .rodata containment --------------------------------------------
# The hex must contain nothing outside the boot section: uploading an
# application would erase whatever lies beyond it.
#
#  * avr64du* (avrxmega2): flash is not fully mapped, so .rodata has no
#    natural home in flash. avr-gcc <= 14 - including the
#    7.3.0-atmel3.6.1-azduino* toolchain that DxCore ships - folds it
#    into the .data init image (LMA right after .text), which is exactly
#    what a bootloader needs, and REJECTS -mrodata-in-ram as an unknown
#    option. avr-gcc >= 15 instead defaults to putting .rodata in the
#    FLMAP window at flash 0x8000, i.e. OUTSIDE the boot section, so
#    there we have to ask for the old behaviour explicitly.
#  * avr16du*/avr32du* (avrxmega3): all of flash is mapped; .rodata is
#    loaded right after .text and is contained on every compiler.
#
# Detected automatically; override with e.g. make RODATA_FLAGS=-mrodata-in-ram
CC_MAJOR := $(firstword $(subst ., ,$(shell $(CC) -dumpversion)))
ifneq ($(filter avr64du%,$(MCU)),)
  ifneq ($(filter 15 16 17 18 19 20 21 22 23 24 25,$(CC_MAJOR)),)
    RODATA_FLAGS ?= -mrodata-in-ram
  endif
endif
RODATA_FLAGS ?=

CFLAGS := -mmcu=$(MCU) \
          -DF_CPU=$(F_CPU) \
          -DBOOTLOADER_BUILD=1 \
          $(RODATA_FLAGS) \
          \
          -Os \
          -ffunction-sections -fdata-sections \
          -fno-common \
          -mrelax \
          -Wall -Wextra -Wstrict-prototypes \
          -std=gnu11 \
          -Isrc

# --- LED pin override (per-build) ------------------------------------
# main.c defaults the DFU LED to PA7 (present on 20/28/32-pin DU).  The
# 14-pin DU has no PA7, so its build classes pass LED_PORT/LED_PIN to move
# the LED to PD6, e.g.:  make ... LED_PORT=PORTD LED_PIN=6
# (Kept as plain tokens -- no spaces/parens -- so Windows cmd passes them
# through cleanly.)
ifneq ($(strip $(LED_PORT)),)
CFLAGS += -DBL_LED_PORT=$(LED_PORT)
endif
ifneq ($(strip $(LED_PIN)),)
CFLAGS += -DBL_LED_PIN=$(LED_PIN)
endif

# --- VUSB regulator -------------------------------------------------
# VREG=1 (default): enable the internal 5V->3.3V USB voltage regulator at
#                   startup (5V VDD systems; power configurations 5b/5s).
# VREG=0:           leave the regulator off; the board feeds 3.3V into the
#                   VUSB pin externally (configuration 3s). Built as the
#                   *_novreg hex variants, e.g.:  make ... VREG=0
VREG ?= 1
ifeq ($(VREG),1)
CFLAGS += -DUSB_VREG_INTERNAL
endif

# The toolchain's specs-avr64du32 file handles the avrxmega2 paths
# automatically when -mmcu=avr64du32 is set, so we don't need any
# explicit -B options here.

# The SPM entry stub sits in the last 6 bytes of the boot section.
SPM_ADDR := 0x0FFA
VER_ADDR := 0x0FFE

LDFLAGS := -mmcu=$(MCU) \
           $(RODATA_FLAGS) \
           -Wl,--section-start=.spmtarg=$(SPM_ADDR) \
           -Wl,--section-start=.blversion=$(VER_ADDR) \
           -Wl,--undefined=bl_spm_entry \
           -Wl,--undefined=bl_version \
           -Wl,--gc-sections \
           -Wl,--section-start=.text=$(BOOT_BASE) \
           -Wl,-Map,$(TARGET).map \
           -mrelax
# NOTE: we intentionally do NOT pass -nostartfiles.  The standard
# crt provides _start which calls main, and is what makes
# --gc-sections see main as reachable.  Crt overhead is ~50 bytes,
# more than compensated by the section-level dead-code elimination.

# Optional: a custom linker script could replace --section-start above
# if we wanted finer control over .data init image placement.  For a
# 4 KB bootloader the default link with .text=0 is sufficient.

# --- Build rules ----------------------------------------------------
.PHONY: all clean size

all: $(TARGET).hex $(TARGET).lst

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) --mcu=$(MCU) -A $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .signature -R .user_signatures \
	           --set-section-flags .spmtarg=alloc,load \
	           --set-section-flags .blversion=alloc,load $< $@

$(TARGET).lst: $(TARGET).elf
	$(OBJDUMP) -h -S $< > $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

size: $(TARGET).elf
	@echo ""
	@echo "=== Bootloader size vs $(BOOTSIZE)-byte budget ==="
	@$(SIZE) --mcu=$(MCU) -A $< \
	  | awk '/^.text/  { t=$$2 } \
	         /^.data/  { d=$$2 } \
	         END { printf "  .text = %u\n  .data = %u (init image)\n  total = %u bytes\n  free  = %d bytes\n", \
	                       t, d, t+d, $(BOOTSIZE)-(t+d) }'

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(TARGET).lst $(TARGET).map
