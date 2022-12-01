# Makefile for AVR Mega-0 (4809), Tiny-0, and Tiny-1 version of Optiboot
# Bill Westfield, 2019
# $Id$
#
# Edit History
# Sep-2019 refactor from the normal AVR Makefile.
# * Copyright 2013-2019 by Bill Westfield.  Part of Optiboot.
# * This software is licensed under version 2 of the Gnu Public Licence.
# * See optiboot.c for details.

HELPTEXT = "\n"
#----------------------------------------------------------------------
#
# program name should not be changed...
PROGRAM    = optiboot_dx
MF:= $(MAKEFILE_LIST)

# export symbols to recursive makes (for ISP)
export

# defaults
MCU_TARGET = avr128da64

LDSECTIONS  = -Wl,-section-start=.text=0 \
        -Wl,--section-start=.application=0x200 \
        -Wl,--section-start=.spmtarg=0x1fa \
        -Wl,--section-start=.version=0x1fe

BAUD_RATE=115200

# DxCore Notice: I am not an expert on makefiles and toolchains.
# I build on Windows, using a portable arduino 1.0.6 package,
# where everything except hardware/bootloaders and hardware/tools
# have been removed, and the toolchain has been updated to the
# latest version (see the board manager json file). I know
# nothing about building under any other conditions, and I would
# not know how to create a Makefile from scratch.
# I know how to write embedded C. I don't know shit about
# makefiles and build environments. This is all Stack Overflow
# and staring at files I don't understand unti I find an "Oh
# this like looks like it's related to the thing I need to change"
# I realize this may not be terribly useful to those trying to
# build it. If anyone who does know wants to help out with a PR
# that would be great - as long as it still builds from an
# arduino 1.0.6 directory structure with the new toolchain
# copied over the old one... because otherwise I couldn't
# build these.

# If we have a PACKS directory specified, we should use it...
ifeq ($(ENV), arduino)
# For Arduino, we assume that we're connected to the optiboot directory
# included with the arduino distribution, which means that the full set
# of avr-tools are "right up there" in standard places.
# (except that in 1.5.x, there's an additional level of "up")
TESTDIR := $(firstword $(wildcard ../../../tools/*))
ifeq (,$(TESTDIR))
# Arduino 1.5.x tool location compared to optiboot dir
	TOOLROOT = ../../../../tools
else
# Arduino 1.0 (and earlier) tool location
	TOOLROOT = ../../../tools
endif
GCCROOT = $(TOOLROOT)/avr/bin/

ifeq ($(OS), windows)
# On windows, SOME of the tool paths will need to have backslashes instead
# of forward slashes (because they use windows cmd.exe for execution instead
# of a unix/mingw shell?)  We also have to ensure that a consistent shell
# is used even if a unix shell is installed (ie as part of WINAVR)
fixpath = $(subst /,\,$1)
SHELL = cmd.exe
SH = sh
endif

else ifeq ($(ENV), arduinodev)
# Arduino IDE source code environment.  Use the unpacked compilers created
# by the build (you'll need to do "ant build" first.)
ifeq ($(OS), macosx)
TOOLROOT = ../../../../build/macosx/work/Arduino.app/Contents/Resources/Java/hardware/tools
endif
ifeq ($(OS), windows)
TOOLROOT = ../../../../build/windows/work/hardware/tools
endif

GCCROOT = $(TOOLROOT)/avr/bin/
AVRDUDE_CONF = -C$(TOOLROOT)/avr/etc/avrdude.conf

else
GCCROOT =
AVRDUDE_CONF =
endif

#
# End of build environment code.


CC         = $(GCCROOT)avr-gcc
RCC        = $(abspath $(CC))
#$(info wildcard ("$(wildcard $(CC))",""))
ifndef PRODUCTION
$(info Using Compiler at: ${RCC})
endif


# If we have a PACKS directory specified, we should use it...
ifdef PACKS
PACK_OPT= -I $(PACKS)/include/ -B $(PACKS)/gcc/dev/$(TARGET)/
ifndef PRODUCTION
$(info   and Chip-defining PACKS at ${PACKS})
endif
endif


OPTIMIZE = -Os -fno-split-wide-types -mrelax

# Override is only needed by avr-lib build system.

override CFLAGS  = -g -Wall $(OPTIMIZE)
override LDFLAGS = $(LDSECTIONS) -Wl,--relax -nostartfiles -nostdlib

OBJCOPY        = $(GCCROOT)avr-objcopy
OBJDUMP        = "$(GCCROOT)avr-objdump"
SIZE           = $(GCCROOT)avr-size

include parse_options.mk

.PRECIOUS: optiboot_%.elf

ifndef PRODUCTION
LISTING= $(OBJDUMP) -S
else
LISTING= @true
endif



#---------------------------------------------------------------------------
# "Chip-level Platform" targets.
# A "Chip-level Platform" compiles for a particular chip, but probably does
# not have "standard" values for things like clock speed, LED pin, etc.
# Makes for chip-level platforms should usually explicitly define their
# options like: "make atmega4809 UARTTX=A4 LED=D0"
#---------------------------------------------------------------------------
# In order to build DxCore releases, there are recipes defined below for every
#  bootloader binary we ship with.
#
# Note about fuses:
#  The fuses are defined in the source code. They may or may not match what is used
#  in practice. DxCore users are advised to not attempt to do the automatic build
#  and upload - I (Spence) have never tested it and have not the faintest idea
#  how to do it!
#
#---------------------------------------------------------------------------
#

HELPTEXT += "\n-------------\n\n"

optiboot_%.hex: optiboot_%.elf
	$(OBJCOPY) -j .text -j .data -j .version -j .spmtarg --set-section-flags .version=alloc,load --set-section-flags .spmtarg=alloc,load -O ihex $< $@

optiboot_%.elf:	optiboot_dx.c FORCE
	$(CC) $(CFLAGS) $(CPU_OPTIONS) $(LED_OPTIONS) $(UART_OPTIONS) $(POR_CMD) $(COMMON_OPTIONS) $(LDFLAGS) $(PACK_OPT) -mmcu=$(TARGET) -o $@ $<
	$(SIZE) -A $@
	$(LISTING) $@ > optiboot_$*.lst


#---------------------------------------------------------------------------
# "Board-level Platform" targets.
# A "Board-level Platform" implies a manufactured platform with a particular
# AVR_FREQ, LED, and so on.  Parameters are not particularly changeable from
# the "make" command line.
# Most of the board-level platform builds should invoke make recursively
#  appropriate specific options
#---------------------------------------------------------------------------

128dx_ser0_extr: TARGET = avr128da64
128dx_ser0_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins.\n"


128dx_ser0_alt_extr: TARGET = avr128da64
128dx_ser0_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins.\n"


128dx_ser1_extr: TARGET = avr128da64
128dx_ser1_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins.\n"


128dx_ser1_alt_extr: TARGET = avr128da64
128dx_ser1_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_extr.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins.\n"


128dx_ser2_extr: TARGET = avr128da64
128dx_ser2_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_extr.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins.\n"


128dx_ser2_alt_extr: TARGET = avr128da64
128dx_ser2_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_extr.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins.\n"


128dx_ser3_extr: TARGET = avr128da64
128dx_ser3_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_extr.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins.\n"


128dx_ser3_alt_extr: TARGET = avr128da64
128dx_ser3_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_extr.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins.\n"


128dx_ser4_extr: TARGET = avr128da64
128dx_ser4_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_extr.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins.\n"


128dx_ser4_alt_extr: TARGET = avr128da64
128dx_ser4_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_extr.hex UARTTX=E4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins.\n"


128dx_ser5_extr: TARGET = avr128da64
128dx_ser5_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_extr.hex UARTTX=G0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins.\n"


128dx_ser5_alt_extr: TARGET = avr128da64
128dx_ser5_alt_extr:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_extr.hex UARTTX=G4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins.\n"

64dx_ser0_extr: TARGET = avr64da64
64dx_ser0_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins.\n"


64dx_ser0_alt_extr: TARGET = avr64da64
64dx_ser0_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins.\n"


64dx_ser1_extr: TARGET = avr64da64
64dx_ser1_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins.\n"


64dx_ser1_alt_extr: TARGET = avr64da64
64dx_ser1_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_extr.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins.\n"


64dx_ser2_extr: TARGET = avr64da64
64dx_ser2_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_extr.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins.\n"


64dx_ser2_alt_extr: TARGET = avr64da64
64dx_ser2_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_extr.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins.\n"


64dx_ser3_extr: TARGET = avr64da64
64dx_ser3_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_extr.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins.\n"


64dx_ser3_alt_extr: TARGET = avr64da64
64dx_ser3_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_extr.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins.\n"


64dx_ser4_extr: TARGET = avr64da64
64dx_ser4_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_extr.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins.\n"


64dx_ser4_alt_extr: TARGET = avr64da64
64dx_ser4_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_extr.hex UARTTX=E4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins.\n"


64dx_ser5_extr: TARGET = avr64da64
64dx_ser5_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_extr.hex UARTTX=G0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins.\n"


64dx_ser5_alt_extr: TARGET = avr64da64
64dx_ser5_alt_extr:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_extr.hex UARTTX=G4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins.\n"

32dx_ser0_extr: TARGET = avr32da48
32dx_ser0_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins.\n"


32dx_ser0_alt_extr: TARGET = avr32da48
32dx_ser0_alt_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins.\n"


32dx_ser1_extr: TARGET = avr32da48
32dx_ser1_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins.\n"


32dx_ser1_alt_extr: TARGET = avr32da48
32dx_ser1_alt_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_extr.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins.\n"


32dx_ser2_extr: TARGET = avr32da48
32dx_ser2_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_extr.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins.\n"


32dx_ser2_alt_extr: TARGET = avr32da48
32dx_ser2_alt_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_extr.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins.\n"


32dx_ser3_extr: TARGET = avr32da48
32dx_ser3_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_extr.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins.\n"


32dx_ser3_alt_extr: TARGET = avr32da48
32dx_ser3_alt_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_extr.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins.\n"


32dx_ser4_extr: TARGET = avr32da48
32dx_ser4_extr:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_extr.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins.\n"

128dx_ser0_all_8sec: TARGET = avr128da64
128dx_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


128dx_ser0_alt_all_8sec: TARGET = avr128da64
128dx_ser0_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_all_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


128dx_ser1_all_8sec: TARGET = avr128da64
128dx_ser1_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_all_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


128dx_ser1_alt_all_8sec: TARGET = avr128da64
128dx_ser1_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_all_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


128dx_ser2_all_8sec: TARGET = avr128da64
128dx_ser2_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_all_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


128dx_ser2_alt_all_8sec: TARGET = avr128da64
128dx_ser2_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_all_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


128dx_ser3_all_8sec: TARGET = avr128da64
128dx_ser3_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_all_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


128dx_ser3_alt_all_8sec: TARGET = avr128da64
128dx_ser3_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_all_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


128dx_ser4_all_8sec: TARGET = avr128da64
128dx_ser4_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_all_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. 8 sec timeout for manual reset.\n"


128dx_ser4_alt_all_8sec: TARGET = avr128da64
128dx_ser4_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_all_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. 8 sec timeout for manual reset.\n"


128dx_ser5_all_8sec: TARGET = avr128da64
128dx_ser5_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_all_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. 8 sec timeout for manual reset.\n"


128dx_ser5_alt_all_8sec: TARGET = avr128da64
128dx_ser5_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_all_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. 8 sec timeout for manual reset.\n"

64dx_ser0_all_8sec: TARGET = avr64da64
64dx_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


64dx_ser0_alt_all_8sec: TARGET = avr64da64
64dx_ser0_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_all_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


64dx_ser1_all_8sec: TARGET = avr64da64
64dx_ser1_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_all_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


64dx_ser1_alt_all_8sec: TARGET = avr64da64
64dx_ser1_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_all_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


64dx_ser2_all_8sec: TARGET = avr64da64
64dx_ser2_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_all_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


64dx_ser2_alt_all_8sec: TARGET = avr64da64
64dx_ser2_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_all_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


64dx_ser3_all_8sec: TARGET = avr64da64
64dx_ser3_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_all_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


64dx_ser3_alt_all_8sec: TARGET = avr64da64
64dx_ser3_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_all_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


64dx_ser4_all_8sec: TARGET = avr64da64
64dx_ser4_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_all_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. 8 sec timeout for manual reset.\n"


64dx_ser4_alt_all_8sec: TARGET = avr64da64
64dx_ser4_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_all_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. 8 sec timeout for manual reset.\n"


64dx_ser5_all_8sec: TARGET = avr64da64
64dx_ser5_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_all_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. 8 sec timeout for manual reset.\n"


64dx_ser5_alt_all_8sec: TARGET = avr64da64
64dx_ser5_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_all_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. 8 sec timeout for manual reset.\n"

32dx_ser0_all_8sec: TARGET = avr32da48
32dx_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


32dx_ser0_alt_all_8sec: TARGET = avr32da48
32dx_ser0_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_all_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


32dx_ser1_all_8sec: TARGET = avr32da48
32dx_ser1_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_all_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


32dx_ser1_alt_all_8sec: TARGET = avr32da48
32dx_ser1_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_all_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


32dx_ser2_all_8sec: TARGET = avr32da48
32dx_ser2_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_all_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


32dx_ser2_alt_all_8sec: TARGET = avr32da48
32dx_ser2_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_all_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


32dx_ser3_all_8sec: TARGET = avr32da48
32dx_ser3_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_all_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


32dx_ser3_alt_all_8sec: TARGET = avr32da48
32dx_ser3_alt_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_all_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


32dx_ser4_all_8sec: TARGET = avr32da48
32dx_ser4_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_all_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. 8 sec timeout for manual reset.\n"

128dx_ser0_poronly_8sec: TARGET = avr128da64
128dx_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser0_alt_poronly_8sec: TARGET = avr128da64
128dx_ser0_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_poronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser1_poronly_8sec: TARGET = avr128da64
128dx_ser1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_poronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser1_alt_poronly_8sec: TARGET = avr128da64
128dx_ser1_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_poronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser2_poronly_8sec: TARGET = avr128da64
128dx_ser2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_poronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser2_alt_poronly_8sec: TARGET = avr128da64
128dx_ser2_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_poronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser3_poronly_8sec: TARGET = avr128da64
128dx_ser3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_poronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser3_alt_poronly_8sec: TARGET = avr128da64
128dx_ser3_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_poronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser4_poronly_8sec: TARGET = avr128da64
128dx_ser4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_poronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser4_alt_poronly_8sec: TARGET = avr128da64
128dx_ser4_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_poronly_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser5_poronly_8sec: TARGET = avr128da64
128dx_ser5_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_poronly_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser5_alt_poronly_8sec: TARGET = avr128da64
128dx_ser5_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_poronly_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

64dx_ser0_poronly_8sec: TARGET = avr64da64
64dx_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser0_alt_poronly_8sec: TARGET = avr64da64
64dx_ser0_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_poronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser1_poronly_8sec: TARGET = avr64da64
64dx_ser1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_poronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser1_alt_poronly_8sec: TARGET = avr64da64
64dx_ser1_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_poronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser2_poronly_8sec: TARGET = avr64da64
64dx_ser2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_poronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser2_alt_poronly_8sec: TARGET = avr64da64
64dx_ser2_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_poronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser3_poronly_8sec: TARGET = avr64da64
64dx_ser3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_poronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser3_alt_poronly_8sec: TARGET = avr64da64
64dx_ser3_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_poronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser4_poronly_8sec: TARGET = avr64da64
64dx_ser4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_poronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser4_alt_poronly_8sec: TARGET = avr64da64
64dx_ser4_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_poronly_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser5_poronly_8sec: TARGET = avr64da64
64dx_ser5_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_poronly_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser5_alt_poronly_8sec: TARGET = avr64da64
64dx_ser5_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_poronly_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

32dx_ser0_poronly_8sec: TARGET = avr32da48
32dx_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser0_alt_poronly_8sec: TARGET = avr32da48
32dx_ser0_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_poronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser1_poronly_8sec: TARGET = avr32da48
32dx_ser1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_poronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser1_alt_poronly_8sec: TARGET = avr32da48
32dx_ser1_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_poronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser2_poronly_8sec: TARGET = avr32da48
32dx_ser2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_poronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser2_alt_poronly_8sec: TARGET = avr32da48
32dx_ser2_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_poronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser3_poronly_8sec: TARGET = avr32da48
32dx_ser3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_poronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser3_alt_poronly_8sec: TARGET = avr32da48
32dx_ser3_alt_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_poronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser4_poronly_8sec: TARGET = avr32da48
32dx_ser4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_poronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser0_extronly: TARGET = avr128da64
128dx_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser0_alt_extronly: TARGET = avr128da64
128dx_ser0_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser1_extronly: TARGET = avr128da64
128dx_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser1_alt_extronly: TARGET = avr128da64
128dx_ser1_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_extronly.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser2_extronly: TARGET = avr128da64
128dx_ser2_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_extronly.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser2_alt_extronly: TARGET = avr128da64
128dx_ser2_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_extronly.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser3_extronly: TARGET = avr128da64
128dx_ser3_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_extronly.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser3_alt_extronly: TARGET = avr128da64
128dx_ser3_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_extronly.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser4_extronly: TARGET = avr128da64
128dx_ser4_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_extronly.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser4_alt_extronly: TARGET = avr128da64
128dx_ser4_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_extronly.hex UARTTX=E4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser5_extronly: TARGET = avr128da64
128dx_ser5_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_extronly.hex UARTTX=G0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser5_alt_extronly: TARGET = avr128da64
128dx_ser5_alt_extronly:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_extronly.hex UARTTX=G4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"

64dx_ser0_extronly: TARGET = avr64da64
64dx_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser0_alt_extronly: TARGET = avr64da64
64dx_ser0_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser1_extronly: TARGET = avr64da64
64dx_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser1_alt_extronly: TARGET = avr64da64
64dx_ser1_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_extronly.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser2_extronly: TARGET = avr64da64
64dx_ser2_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_extronly.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser2_alt_extronly: TARGET = avr64da64
64dx_ser2_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_extronly.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser3_extronly: TARGET = avr64da64
64dx_ser3_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_extronly.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser3_alt_extronly: TARGET = avr64da64
64dx_ser3_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_extronly.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser4_extronly: TARGET = avr64da64
64dx_ser4_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_extronly.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser4_alt_extronly: TARGET = avr64da64
64dx_ser4_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_extronly.hex UARTTX=E4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser5_extronly: TARGET = avr64da64
64dx_ser5_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_extronly.hex UARTTX=G0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser5_alt_extronly: TARGET = avr64da64
64dx_ser5_alt_extronly:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_extronly.hex UARTTX=G4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"

32dx_ser0_extronly: TARGET = avr32da48
32dx_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser0_alt_extronly: TARGET = avr32da48
32dx_ser0_alt_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser1_extronly: TARGET = avr32da48
32dx_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser1_alt_extronly: TARGET = avr32da48
32dx_ser1_alt_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_extronly.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser2_extronly: TARGET = avr32da48
32dx_ser2_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_extronly.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser2_alt_extronly: TARGET = avr32da48
32dx_ser2_alt_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_extronly.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser3_extronly: TARGET = avr32da48
32dx_ser3_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_extronly.hex UARTTX=B0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser3_alt_extronly: TARGET = avr32da48
32dx_ser3_alt_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_extronly.hex UARTTX=B4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser4_extronly: TARGET = avr32da48
32dx_ser4_extronly:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_extronly.hex UARTTX=E0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"

128dx_ser0_swronly_8sec: TARGET = avr128da64
128dx_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser0_alt_swronly_8sec: TARGET = avr128da64
128dx_ser0_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_swronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser1_swronly_8sec: TARGET = avr128da64
128dx_ser1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_swronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser1_alt_swronly_8sec: TARGET = avr128da64
128dx_ser1_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_swronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser2_swronly_8sec: TARGET = avr128da64
128dx_ser2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_swronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser2_alt_swronly_8sec: TARGET = avr128da64
128dx_ser2_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_swronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser3_swronly_8sec: TARGET = avr128da64
128dx_ser3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_swronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser3_alt_swronly_8sec: TARGET = avr128da64
128dx_ser3_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_swronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser4_swronly_8sec: TARGET = avr128da64
128dx_ser4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_swronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser4_alt_swronly_8sec: TARGET = avr128da64
128dx_ser4_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_swronly_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser5_swronly_8sec: TARGET = avr128da64
128dx_ser5_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_swronly_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


128dx_ser5_alt_swronly_8sec: TARGET = avr128da64
128dx_ser5_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_swronly_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

64dx_ser0_swronly_8sec: TARGET = avr64da64
64dx_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser0_alt_swronly_8sec: TARGET = avr64da64
64dx_ser0_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_swronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser1_swronly_8sec: TARGET = avr64da64
64dx_ser1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_swronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser1_alt_swronly_8sec: TARGET = avr64da64
64dx_ser1_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_swronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser2_swronly_8sec: TARGET = avr64da64
64dx_ser2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_swronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser2_alt_swronly_8sec: TARGET = avr64da64
64dx_ser2_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_swronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser3_swronly_8sec: TARGET = avr64da64
64dx_ser3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_swronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser3_alt_swronly_8sec: TARGET = avr64da64
64dx_ser3_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_swronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser4_swronly_8sec: TARGET = avr64da64
64dx_ser4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_swronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser4_alt_swronly_8sec: TARGET = avr64da64
64dx_ser4_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_swronly_8sec.hex UARTTX=E4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser5_swronly_8sec: TARGET = avr64da64
64dx_ser5_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_swronly_8sec.hex UARTTX=G0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dx_ser5_alt_swronly_8sec: TARGET = avr64da64
64dx_ser5_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_swronly_8sec.hex UARTTX=G4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

32dx_ser0_swronly_8sec: TARGET = avr32da48
32dx_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser0_alt_swronly_8sec: TARGET = avr32da48
32dx_ser0_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_swronly_8sec.hex UARTTX=A4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser1_swronly_8sec: TARGET = avr32da48
32dx_ser1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_swronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser1_alt_swronly_8sec: TARGET = avr32da48
32dx_ser1_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_swronly_8sec.hex UARTTX=C4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser2_swronly_8sec: TARGET = avr32da48
32dx_ser2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_swronly_8sec.hex UARTTX=F0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser2_alt_swronly_8sec: TARGET = avr32da48
32dx_ser2_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_swronly_8sec.hex UARTTX=F4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser3_swronly_8sec: TARGET = avr32da48
32dx_ser3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_swronly_8sec.hex UARTTX=B0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser3_alt_swronly_8sec: TARGET = avr32da48
32dx_ser3_alt_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_swronly_8sec.hex UARTTX=B4 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dx_ser4_swronly_8sec: TARGET = avr32da48
32dx_ser4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_swronly_8sec.hex UARTTX=E0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

128dx_ser0_extr_8sec: TARGET = avr128da64
128dx_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser0_alt_extr_8sec: TARGET = avr128da64
128dx_ser0_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser0_alt_extr_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser1_extr_8sec: TARGET = avr128da64
128dx_ser1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_extr_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser1_alt_extr_8sec: TARGET = avr128da64
128dx_ser1_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser1_alt_extr_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser2_extr_8sec: TARGET = avr128da64
128dx_ser2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_extr_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser2_alt_extr_8sec: TARGET = avr128da64
128dx_ser2_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser2_alt_extr_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser3_extr_8sec: TARGET = avr128da64
128dx_ser3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_extr_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser3_alt_extr_8sec: TARGET = avr128da64
128dx_ser3_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser3_alt_extr_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser4_extr_8sec: TARGET = avr128da64
128dx_ser4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_extr_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser4_alt_extr_8sec: TARGET = avr128da64
128dx_ser4_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser4_alt_extr_8sec.hex UARTTX=E4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser5_extr_8sec: TARGET = avr128da64
128dx_ser5_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_extr_8sec.hex UARTTX=G0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. Reset Pin Reset Only, 1 second timeout.\n"


128dx_ser5_alt_extr_8sec: TARGET = avr128da64
128dx_ser5_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_128dx_ser5_alt_extr_8sec.hex UARTTX=G4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"

64dx_ser0_extr_8sec: TARGET = avr64da64
64dx_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser0_alt_extr_8sec: TARGET = avr64da64
64dx_ser0_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser0_alt_extr_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser1_extr_8sec: TARGET = avr64da64
64dx_ser1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_extr_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser1_alt_extr_8sec: TARGET = avr64da64
64dx_ser1_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser1_alt_extr_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser2_extr_8sec: TARGET = avr64da64
64dx_ser2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_extr_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser2_alt_extr_8sec: TARGET = avr64da64
64dx_ser2_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser2_alt_extr_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser3_extr_8sec: TARGET = avr64da64
64dx_ser3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_extr_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser3_alt_extr_8sec: TARGET = avr64da64
64dx_ser3_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser3_alt_extr_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser4_extr_8sec: TARGET = avr64da64
64dx_ser4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_extr_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser4_alt_extr_8sec: TARGET = avr64da64
64dx_ser4_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser4_alt_extr_8sec.hex UARTTX=E4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser5_extr_8sec: TARGET = avr64da64
64dx_ser5_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_extr_8sec.hex UARTTX=G0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dx_ser5_alt_extr_8sec: TARGET = avr64da64
64dx_ser5_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dx_ser5_alt_extr_8sec.hex UARTTX=G4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"

32dx_ser0_extr_8sec: TARGET = avr32da48
32dx_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser0_alt_extr_8sec: TARGET = avr32da48
32dx_ser0_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser0_alt_extr_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser1_extr_8sec: TARGET = avr32da48
32dx_ser1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_extr_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser1_alt_extr_8sec: TARGET = avr32da48
32dx_ser1_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser1_alt_extr_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser2_extr_8sec: TARGET = avr32da48
32dx_ser2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_extr_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser2_alt_extr_8sec: TARGET = avr32da48
32dx_ser2_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser2_alt_extr_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser3_extr_8sec: TARGET = avr32da48
32dx_ser3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_extr_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser3_alt_extr_8sec: TARGET = avr32da48
32dx_ser3_alt_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser3_alt_extr_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. Reset Pin Reset Only, 1 second timeout.\n"


32dx_ser4_extr_8sec: TARGET = avr32da48
32dx_ser4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dx_ser4_extr_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_extr: TARGET = avr64dd32
64dd_ser0_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins.\n"


64dd_ser0_alt1_extr: TARGET = avr64dd32
64dd_ser0_alt1_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt2_extr: TARGET = avr64dd32
64dd_ser0_alt2_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_extr.hex UARTTX=A2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt3_extr: TARGET = avr64dd32
64dd_ser0_alt3_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_extr.hex UARTTX=D4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset3. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt4_extr: TARGET = avr64dd32
64dd_ser0_alt4_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_extr.hex UARTTX=C1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset4. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser1_extr: TARGET = avr64dd32
64dd_ser1_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins.\n"


64dd_ser1_alt2_extr: TARGET = avr64dd32
64dd_ser1_alt2_extr:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_extr.hex UARTTX=D6 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2.\n"


32dd_ser0_extr: TARGET = avr32dd32
32dd_ser0_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins.\n"


32dd_ser0_alt1_extr: TARGET = avr32dd32
32dd_ser0_alt1_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1.\n"


32dd_ser0_alt2_extr: TARGET = avr32dd32
32dd_ser0_alt2_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_extr.hex UARTTX=A2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt3_extr: TARGET = avr32dd32
32dd_ser0_alt3_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_extr.hex UARTTX=D4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset3. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt4_extr: TARGET = avr32dd32
32dd_ser0_alt4_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_extr.hex UARTTX=C1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset4. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser1_extr: TARGET = avr32dd32
32dd_ser1_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins.\n"


32dd_ser1_alt2_extr: TARGET = avr32dd32
32dd_ser1_alt2_extr:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_extr.hex UARTTX=D6 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2.\n"


64dd_ser0_all_8sec: TARGET = avr64dd32
64dd_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7


HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins. Always enter bootloader, 8 second timeout.\n"


64dd_ser0_alt1_all_8sec: TARGET = avr64dd32
64dd_ser0_alt1_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_all_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd_ser0_alt2_all_8sec: TARGET = avr64dd32
64dd_ser0_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_all_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd_ser0_alt3_all_8sec: TARGET = avr64dd32
64dd_ser0_alt3_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_all_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd_ser0_alt4_all_8sec: TARGET = avr64dd32
64dd_ser0_alt4_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_all_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd_ser1_all_8sec: TARGET = avr64dd32
64dd_ser1_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_all_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins. Always enter bootloader, 8 second timeout.\n"


64dd_ser1_alt2_all_8sec: TARGET = avr64dd32
64dd_ser1_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_all_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2. Always enter bootloader, 8 second timeout.\n"


32dd_ser0_all_8sec: TARGET = avr32dd32
32dd_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins. Always enter bootloader, 8 second timeout.\n"


32dd_ser0_alt1_all_8sec: TARGET = avr32dd32
32dd_ser0_alt1_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_all_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd_ser0_alt2_all_8sec: TARGET = avr32dd32
32dd_ser0_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_all_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd_ser0_alt3_all_8sec: TARGET = avr32dd32
32dd_ser0_alt3_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_all_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd_ser0_alt4_all_8sec: TARGET = avr32dd32
32dd_ser0_alt4_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_all_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd_ser1_all_8sec: TARGET = avr32dd32
32dd_ser1_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_all_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins. Always enter bootloader, 8 second timeout.\n"


32dd_ser1_alt2_all_8sec: TARGET = avr32dd32
32dd_ser1_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_all_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2. Always enter bootloader, 8 second timeout.\n"


64dd_ser0_poronly_8sec: TARGET = avr64dd32
64dd_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt1_poronly_8sec: TARGET = avr64dd32
64dd_ser0_alt1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_poronly_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt2_poronly_8sec: TARGET = avr64dd32
64dd_ser0_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_poronly_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt3_poronly_8sec: TARGET = avr64dd32
64dd_ser0_alt3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_poronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt4_poronly_8sec: TARGET = avr64dd32
64dd_ser0_alt4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_poronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser1_poronly_8sec: TARGET = avr64dd32
64dd_ser1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_poronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser1_alt2_poronly_8sec: TARGET = avr64dd32
64dd_ser1_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_poronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_poronly_8sec: TARGET = avr32dd32
32dd_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt1_poronly_8sec: TARGET = avr32dd32
32dd_ser0_alt1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_poronly_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt2_poronly_8sec: TARGET = avr32dd32
32dd_ser0_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_poronly_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt3_poronly_8sec: TARGET = avr32dd32
32dd_ser0_alt3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_poronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt4_poronly_8sec: TARGET = avr32dd32
32dd_ser0_alt4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_poronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser1_poronly_8sec: TARGET = avr32dd32
32dd_ser1_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_poronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser1_alt2_poronly_8sec: TARGET = avr32dd32
32dd_ser1_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_poronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"

64dd_ser0_extronly: TARGET = avr64dd32
64dd_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt1_extronly: TARGET = avr64dd32
64dd_ser0_alt1_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt2_extronly: TARGET = avr64dd32
64dd_ser0_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_extronly.hex UARTTX=A2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt3_extronly: TARGET = avr64dd32
64dd_ser0_alt3_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_extronly.hex UARTTX=D4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt4_extronly: TARGET = avr64dd32
64dd_ser0_alt4_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_extronly.hex UARTTX=C1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser1_extronly: TARGET = avr64dd32
64dd_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser1_alt2_extronly: TARGET = avr64dd32
64dd_ser1_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_extronly.hex UARTTX=D6 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_extronly: TARGET = avr32dd32
32dd_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt1_extronly: TARGET = avr32dd32
32dd_ser0_alt1_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt2_extronly: TARGET = avr32dd32
32dd_ser0_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_extronly.hex UARTTX=A2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt3_extronly: TARGET = avr32dd32
32dd_ser0_alt3_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_extronly.hex UARTTX=D4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt4_extronly: TARGET = avr32dd32
32dd_ser0_alt4_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_extronly.hex UARTTX=C1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser1_extronly: TARGET = avr32dd32
32dd_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser1_alt2_extronly: TARGET = avr32dd32
32dd_ser1_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_extronly.hex UARTTX=D6 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_swronly_8sec: TARGET = avr64dd32
64dd_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt1_swronly_8sec: TARGET = avr64dd32
64dd_ser0_alt1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_swronly_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt2_swronly_8sec: TARGET = avr64dd32
64dd_ser0_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_swronly_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt3_swronly_8sec: TARGET = avr64dd32
64dd_ser0_alt3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_swronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_alt4_swronly_8sec: TARGET = avr64dd32
64dd_ser0_alt4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_swronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser1_swronly_8sec: TARGET = avr64dd32
64dd_ser1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_swronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser1_alt2_swronly_8sec: TARGET = avr64dd32
64dd_ser1_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_swronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_swronly_8sec: TARGET = avr32dd32
32dd_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt1_swronly_8sec: TARGET = avr32dd32
32dd_ser0_alt1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_swronly_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt2_swronly_8sec: TARGET = avr32dd32
32dd_ser0_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_swronly_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt3_swronly_8sec: TARGET = avr32dd32
32dd_ser0_alt3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_swronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser0_alt4_swronly_8sec: TARGET = avr32dd32
32dd_ser0_alt4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_swronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser1_swronly_8sec: TARGET = avr32dd32
32dd_ser1_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_swronly_8sec.hex UARTTX=C0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd_ser1_alt2_swronly_8sec: TARGET = avr32dd32
32dd_ser1_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_swronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=A7

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd_ser0_extr_8sec: TARGET = avr64dd32
64dd_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt1_extr_8sec: TARGET = avr64dd32
64dd_ser0_alt1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt1_extr_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt2_extr_8sec: TARGET = avr64dd32
64dd_ser0_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt2_extr_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt3_extr_8sec: TARGET = avr64dd32
64dd_ser0_alt3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt3_extr_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser0_alt4_extr_8sec: TARGET = avr64dd32
64dd_ser0_alt4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser0_alt4_extr_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser1_extr_8sec: TARGET = avr64dd32
64dd_ser1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_extr_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd_ser1_alt2_extr_8sec: TARGET = avr64dd32
64dd_ser1_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd_ser1_alt2_extr_8sec.hex UARTTX=D6 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 64DD - any AVR64DD device, USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_extr_8sec: TARGET = avr32dd32
32dd_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt1_extr_8sec: TARGET = avr32dd32
32dd_ser0_alt1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt1_extr_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt2_extr_8sec: TARGET = avr32dd32
32dd_ser0_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt2_extr_8sec.hex UARTTX=A2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt3_extr_8sec: TARGET = avr32dd32
32dd_ser0_alt3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt3_extr_8sec.hex UARTTX=D4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser0_alt4_extr_8sec: TARGET = avr32dd32
32dd_ser0_alt4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser0_alt4_extr_8sec.hex UARTTX=C1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser1_extr_8sec: TARGET = avr32dd32
32dd_ser1_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_extr_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd_ser1_alt2_extr_8sec: TARGET = avr32dd32
32dd_ser1_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd_ser1_alt2_extr_8sec.hex UARTTX=D6 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD - any AVR32DD or AVR16DD device, USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_extr: TARGET = avr64dd14
64dd14_ser0_extr:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins.\n"


64dd14_ser0_alt3_extr: TARGET = avr64dd14
64dd14_ser0_alt3_extr:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_extr.hex UARTTX=D4 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset3. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_alt4_extr: TARGET = avr64dd14
64dd14_ser0_alt4_extr:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_extr.hex UARTTX=C1 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset4. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser1_alt2_extr: TARGET = avr64dd14
64dd14_ser1_alt2_extr:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_extr.hex UARTTX=D6 TIMEOUT=1 LED=D4 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2.\n"


32dd14_ser0_extr: TARGET = avr32dd14
32dd14_ser0_extr:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins.\n"


32dd14_ser0_alt3_extr: TARGET = avr32dd14
32dd14_ser0_alt3_extr:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_extr.hex UARTTX=D4 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset3. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_alt4_extr: TARGET = avr32dd14
32dd14_ser0_alt4_extr:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_extr.hex UARTTX=C1 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset4. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser1_alt2_extr: TARGET = avr32dd14
32dd14_ser1_alt2_extr:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_extr.hex UARTTX=D6 TIMEOUT=1 LED=D4 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2.\n"


64dd14_ser0_all_8sec: TARGET = avr64dd14
64dd14_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=D6


HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins. Always enter bootloader, 8 second timeout.\n"


64dd14_ser0_alt3_all_8sec: TARGET = avr64dd14
64dd14_ser0_alt3_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_all_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd14_ser0_alt4_all_8sec: TARGET = avr64dd14
64dd14_ser0_alt4_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_all_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


64dd14_ser1_alt2_all_8sec: TARGET = avr64dd14
64dd14_ser1_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_all_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=D4

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2. Always enter bootloader, 8 second timeout.\n"


32dd14_ser0_all_8sec: TARGET = avr32dd14
32dd14_ser0_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_all_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=D6

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins. Always enter bootloader, 8 second timeout.\n"


32dd14_ser0_alt3_all_8sec: TARGET = avr32dd14
32dd14_ser0_alt3_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_all_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd14_ser0_alt4_all_8sec: TARGET = avr32dd14
32dd14_ser0_alt4_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_all_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Always enter bootloader, 8 second timeout.\n"


32dd14_ser1_alt2_all_8sec: TARGET = avr32dd14
32dd14_ser1_alt2_all_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_all_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x35 LED=D4

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2. Always enter bootloader, 8 second timeout.\n"


64dd14_ser0_poronly_8sec: TARGET = avr64dd14
64dd14_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=D6

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_alt3_poronly_8sec: TARGET = avr64dd14
64dd14_ser0_alt3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_poronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_alt4_poronly_8sec: TARGET = avr64dd14
64dd14_ser0_alt4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_poronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser1_alt2_poronly_8sec: TARGET = avr64dd14
64dd14_ser1_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_poronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=D4

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_poronly_8sec: TARGET = avr32dd14
32dd14_ser0_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_poronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=D6

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_alt3_poronly_8sec: TARGET = avr32dd14
32dd14_ser0_alt3_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_poronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_alt4_poronly_8sec: TARGET = avr32dd14
32dd14_ser0_alt4_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_poronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x01

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser1_alt2_poronly_8sec: TARGET = avr32dd14
32dd14_ser1_alt2_poronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_poronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x01 LED=D4

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_extronly: TARGET = avr64dd14
64dd14_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_alt3_extronly: TARGET = avr64dd14
64dd14_ser0_alt3_extronly:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_extronly.hex UARTTX=D4 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_alt4_extronly: TARGET = avr64dd14
64dd14_ser0_alt4_extronly:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_extronly.hex UARTTX=C1 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser1_alt2_extronly: TARGET = avr64dd14
64dd14_ser1_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_extronly.hex UARTTX=D6 TIMEOUT=1 LED=D4 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_extronly: TARGET = avr32dd14
32dd14_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_alt3_extronly: TARGET = avr32dd14
32dd14_ser0_alt3_extronly:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_extronly.hex UARTTX=D4 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_alt4_extronly: TARGET = avr32dd14
32dd14_ser0_alt4_extronly:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_extronly.hex UARTTX=C1 TIMEOUT=1 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser1_alt2_extronly: TARGET = avr32dd14
32dd14_ser1_alt2_extronly:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_extronly.hex UARTTX=D6 TIMEOUT=1 LED=D4 ENTRYCOND_REQUIRE=0x04

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_swronly_8sec: TARGET = avr64dd14
64dd14_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=D6

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_alt3_swronly_8sec: TARGET = avr64dd14
64dd14_ser0_alt3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_swronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_alt4_swronly_8sec: TARGET = avr64dd14
64dd14_ser0_alt4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_swronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser1_alt2_swronly_8sec: TARGET = avr64dd14
64dd14_ser1_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_swronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=D4

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_swronly_8sec: TARGET = avr32dd14
32dd14_ser0_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_swronly_8sec.hex UARTTX=A0 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=D6

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_alt3_swronly_8sec: TARGET = avr32dd14
32dd14_ser0_alt3_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_swronly_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser0_alt4_swronly_8sec: TARGET = avr32dd14
32dd14_ser0_alt4_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_swronly_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x10

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


32dd14_ser1_alt2_swronly_8sec: TARGET = avr32dd14
32dd14_ser1_alt2_swronly_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_swronly_8sec.hex UARTTX=D6 TIMEOUT=8 ENTRYCOND_REQUIRE=0x10 LED=D4

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2. 8 sec timeout for manual power-cycle, POR only entry condition.\n"


64dd14_ser0_extr_8sec: TARGET = avr64dd14
64dd14_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 default pins. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_alt3_extr_8sec: TARGET = avr64dd14
64dd14_ser0_alt3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt3_extr_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser0_alt4_extr_8sec: TARGET = avr64dd14
64dd14_ser0_alt4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser0_alt4_extr_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


64dd14_ser1_alt2_extr_8sec: TARGET = avr64dd14
64dd14_ser1_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_64dd14_ser1_alt2_extr_8sec.hex UARTTX=D6 TIMEOUT=8 LED=D4 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target AVR64DD14 (LED on PD4), USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_extr_8sec: TARGET = avr32dd14
32dd14_ser0_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_extr_8sec.hex UARTTX=A0 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0default pins. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_alt3_extr_8sec: TARGET = avr32dd14
32dd14_ser0_alt3_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt3_extr_8sec.hex UARTTX=D4 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser0_alt4_extr_8sec: TARGET = avr32dd14
32dd14_ser0_alt4_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser0_alt4_extr_8sec.hex UARTTX=C1 TIMEOUT=8 LED=D6 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD6), USART0 alternate pinset1. Reset Pin Reset Only, 1 second timeout.\n"


32dd14_ser1_alt2_extr_8sec: TARGET = avr32dd14
32dd14_ser1_alt2_extr_8sec:
	$(MAKE) -f $(MF) optiboot_32dd14_ser1_alt2_extr_8sec.hex UARTTX=D6 TIMEOUT=8 LED=D4 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target 32DD14 - AVR32DD14 or AVR16DD14 (LED on PD4), USART1 alternate pinset2. Reset Pin Reset Only, 1 second timeout.\n"





#---------------------------------------------------------------------------
#
# Generic build instructions
#

FORCE:

# The build+upload functionality has not been tested and almost certainly doesn't work.

isp: $(TARGET) FORCE
	"$(MAKE)" -f Makefile.isp isp TARGET=$(TARGET)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

#windows "rm" is dumb and objects to wildcards that don't exist
clean:
	@touch  __temp_.o __temp_.elf __temp_.lst __temp_.map
	@touch  __temp_.sym __temp_.lss __temp_.eep __temp_.srec
	@touch __temp_.bin __temp_.hex __temp_.tmp.sh
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex *.tmp.sh

clean_asm:
	rm -rf *.lst

%.lst: %.elf FORCE
	$(OBJDUMP) -h -S $< > $@

%.srec: %.elf FORCE
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O srec $< $@

%.bin: %.elf FORCE
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O binary $< $@

help:
	@echo -e $(HELPTEXT)
