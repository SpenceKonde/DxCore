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
PROGRAM    = optiboot_x
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
PACK_OPT= -I $(PACKS)/include/ -B $(PACKS)/gcc/dev/$*
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
#
# Mega0, tiny0, tiny1 don't really have any chip-specific requirements.
#
# Note about fuses:
#  The fuses are defined in the source code.  There are 9!
#  Be sure to use a programmer that will program the fuses from the object file.
#
#---------------------------------------------------------------------------
#

HELPTEXT += "\n-------------\n\n"


optiboot_%.hex: optiboot_%.elf
	$(OBJCOPY) -j .text -j .data -j .version -j .spmtarg --set-section-flags .version=alloc,load --set-section-flags .spmtarg=alloc,load -O ihex $< $@

optiboot_%.elf:	optiboot_x.c FORCE
	$(CC) $(CFLAGS) $(CPU_OPTIONS) $(LED_OPTIONS) $(UART_OPTIONS) $(POR_CMD) $(COMMON_OPTIONS) $(LDFLAGS) $(PACK_OPT) -mmcu=$(TARGET) -o $@ $<
	$(SIZE) $@
	$(LISTING) $@ > optiboot_$*.lst


#---------------------------------------------------------------------------
# "Board-level Platform" targets.
# A "Board-level Platform" implies a manufactured platform with a particular
# AVR_FREQ, LED, and so on.  Parameters are not particularly changeable from
# the "make" command line.
# Most of the board-level platform builds should envoke make recursively
#  appropriate specific options
#---------------------------------------------------------------------------

dx128_ser0: TARGET = avr128da64
dx128_ser0:
	$(MAKE) -f $(MF) optiboot_dx128_ser0.hex UARTTX=A0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins.\n"


dx128_ser0_alt: TARGET = avr128da64
dx128_ser0_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser0_alt.hex UARTTX=A4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins.\n"


dx128_ser1: TARGET = avr128da64
dx128_ser1:
	$(MAKE) -f $(MF) optiboot_dx128_ser1.hex UARTTX=C0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins.\n"


dx128_ser1_alt: TARGET = avr128da64
dx128_ser1_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser1_alt.hex UARTTX=C4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins.\n"


dx128_ser2: TARGET = avr128da64
dx128_ser2:
	$(MAKE) -f $(MF) optiboot_dx128_ser2.hex UARTTX=F0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins.\n"


dx128_ser2_alt: TARGET = avr128da64
dx128_ser2_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser2_alt.hex UARTTX=F4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins.\n"


dx128_ser3: TARGET = avr128da64
dx128_ser3:
	$(MAKE) -f $(MF) optiboot_dx128_ser3.hex UARTTX=B0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins.\n"


dx128_ser3_alt: TARGET = avr128da64
dx128_ser3_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser3_alt.hex UARTTX=B4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins.\n"


dx128_ser4: TARGET = avr128da64
dx128_ser4:
	$(MAKE) -f $(MF) optiboot_dx128_ser4.hex UARTTX=E0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins.\n"


dx128_ser4_alt: TARGET = avr128da64
dx128_ser4_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser4_alt.hex UARTTX=E4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins.\n"


dx128_ser5: TARGET = avr128da64
dx128_ser5:
	$(MAKE) -f $(MF) optiboot_dx128_ser5.hex UARTTX=G0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins.\n"


dx128_ser5_alt: TARGET = avr128da64
dx128_ser5_alt:
	$(MAKE) -f $(MF) optiboot_dx128_ser5_alt.hex UARTTX=G4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins.\n"

dx64_ser0: TARGET = avr64da64
dx64_ser0:
	$(MAKE) -f $(MF) optiboot_dx64_ser0.hex UARTTX=A0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins.\n"


dx64_ser0_alt: TARGET = avr64da64
dx64_ser0_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser0_alt.hex UARTTX=A4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins.\n"


dx64_ser1: TARGET = avr64da64
dx64_ser1:
	$(MAKE) -f $(MF) optiboot_dx64_ser1.hex UARTTX=C0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins.\n"


dx64_ser1_alt: TARGET = avr64da64
dx64_ser1_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser1_alt.hex UARTTX=C4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins.\n"


dx64_ser2: TARGET = avr64da64
dx64_ser2:
	$(MAKE) -f $(MF) optiboot_dx64_ser2.hex UARTTX=F0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins.\n"


dx64_ser2_alt: TARGET = avr64da64
dx64_ser2_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser2_alt.hex UARTTX=F4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins.\n"


dx64_ser3: TARGET = avr64da64
dx64_ser3:
	$(MAKE) -f $(MF) optiboot_dx64_ser3.hex UARTTX=B0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins.\n"


dx64_ser3_alt: TARGET = avr64da64
dx64_ser3_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser3_alt.hex UARTTX=B4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins.\n"


dx64_ser4: TARGET = avr64da64
dx64_ser4:
	$(MAKE) -f $(MF) optiboot_dx64_ser4.hex UARTTX=E0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins.\n"


dx64_ser4_alt: TARGET = avr64da64
dx64_ser4_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser4_alt.hex UARTTX=E4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins.\n"


dx64_ser5: TARGET = avr64da64
dx64_ser5:
	$(MAKE) -f $(MF) optiboot_dx64_ser5.hex UARTTX=G0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins.\n"


dx64_ser5_alt: TARGET = avr64da64
dx64_ser5_alt:
	$(MAKE) -f $(MF) optiboot_dx64_ser5_alt.hex UARTTX=G4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins.\n"

dx32_ser0: TARGET = avr32da48
dx32_ser0:
	$(MAKE) -f $(MF) optiboot_dx32_ser0.hex UARTTX=A0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins.\n"


dx32_ser0_alt: TARGET = avr32da48
dx32_ser0_alt:
	$(MAKE) -f $(MF) optiboot_dx32_ser0_alt.hex UARTTX=A4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins.\n"


dx32_ser1: TARGET = avr32da48
dx32_ser1:
	$(MAKE) -f $(MF) optiboot_dx32_ser1.hex UARTTX=C0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins.\n"


dx32_ser1_alt: TARGET = avr32da48
dx32_ser1_alt:
	$(MAKE) -f $(MF) optiboot_dx32_ser1_alt.hex UARTTX=C4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins.\n"


dx32_ser2: TARGET = avr32da48
dx32_ser2:
	$(MAKE) -f $(MF) optiboot_dx32_ser2.hex UARTTX=F0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins.\n"


dx32_ser2_alt: TARGET = avr32da48
dx32_ser2_alt:
	$(MAKE) -f $(MF) optiboot_dx32_ser2_alt.hex UARTTX=F4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins.\n"


dx32_ser3: TARGET = avr32da48
dx32_ser3:
	$(MAKE) -f $(MF) optiboot_dx32_ser3.hex UARTTX=B0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins.\n"


dx32_ser3_alt: TARGET = avr32da48
dx32_ser3_alt:
	$(MAKE) -f $(MF) optiboot_dx32_ser3_alt.hex UARTTX=B4 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins.\n"


dx32_ser4: TARGET = avr32da48
dx32_ser4:
	$(MAKE) -f $(MF) optiboot_dx32_ser4.hex UARTTX=E0 TIMEOUT=1 LED=A7 START_APP_ON_POR=1

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins.\n"

dx128_ser0_8sec: TARGET = avr128da64
dx128_ser0_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser0_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


dx128_ser0_alt_8sec: TARGET = avr128da64
dx128_ser0_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser0_alt_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


dx128_ser1_8sec: TARGET = avr128da64
dx128_ser1_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser1_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


dx128_ser1_alt_8sec: TARGET = avr128da64
dx128_ser1_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser1_alt_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


dx128_ser2_8sec: TARGET = avr128da64
dx128_ser2_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser2_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


dx128_ser2_alt_8sec: TARGET = avr128da64
dx128_ser2_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser2_alt_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


dx128_ser3_8sec: TARGET = avr128da64
dx128_ser3_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser3_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


dx128_ser3_alt_8sec: TARGET = avr128da64
dx128_ser3_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser3_alt_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


dx128_ser4_8sec: TARGET = avr128da64
dx128_ser4_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser4_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 default pins. 8 sec timeout for manual reset.\n"


dx128_ser4_alt_8sec: TARGET = avr128da64
dx128_ser4_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser4_alt_8sec.hex UARTTX=E4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART4 alternate pins. 8 sec timeout for manual reset.\n"


dx128_ser5_8sec: TARGET = avr128da64
dx128_ser5_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser5_8sec.hex UARTTX=G0 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 default pins. 8 sec timeout for manual reset.\n"


dx128_ser5_alt_8sec: TARGET = avr128da64
dx128_ser5_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx128_ser4_alt_8sec.hex UARTTX=G4 TIMEOUT=8 LED=A7

HELPTEXT += "target 128Dx - any AVR128DA or AVR128DB device, USART5 alternate pins. 8 sec timeout for manual reset.\n"

dx64_ser0_8sec: TARGET = avr64da64
dx64_ser0_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser0_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


dx64_ser0_alt_8sec: TARGET = avr64da64
dx64_ser0_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser0_alt_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


dx64_ser1_8sec: TARGET = avr64da64
dx64_ser1_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser1_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


dx64_ser1_alt_8sec: TARGET = avr64da64
dx64_ser1_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser1_alt_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


dx64_ser2_8sec: TARGET = avr64da64
dx64_ser2_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser2_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


dx64_ser2_alt_8sec: TARGET = avr64da64
dx64_ser2_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser2_alt_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


dx64_ser3_8sec: TARGET = avr64da64
dx64_ser3_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser3_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


dx64_ser3_alt_8sec: TARGET = avr64da64
dx64_ser3_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser3_alt_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


dx64_ser4_8sec: TARGET = avr64da64
dx64_ser4_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser4_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 default pins. 8 sec timeout for manual reset.\n"


dx64_ser4_alt_8sec: TARGET = avr64da64
dx64_ser4_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser4_alt_8sec.hex UARTTX=E4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART4 alternate pins. 8 sec timeout for manual reset.\n"


dx64_ser5_8sec: TARGET = avr64da64
dx64_ser5_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser5_8sec.hex UARTTX=G0 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 default pins. 8 sec timeout for manual reset.\n"


dx64_ser5_alt_8sec: TARGET = avr64da64
dx64_ser5_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx64_ser4_alt_8sec.hex UARTTX=G4 TIMEOUT=8 LED=A7

HELPTEXT += "target 64Dx - any AVR64DA or AVR64DB device, USART5 alternate pins. 8 sec timeout for manual reset.\n"

dx32_ser0_8sec: TARGET = avr32da48
dx32_ser0_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser0_8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 default pins. 8 sec timeout for manual reset.\n"


dx32_ser0_alt_8sec: TARGET = avr32da48
dx32_ser0_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser0_alt_8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART0 alternate pins. 8 sec timeout for manual reset.\n"


dx32_ser1_8sec: TARGET = avr32da48
dx32_ser1_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser1_8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 default pins. 8 sec timeout for manual reset.\n"


dx32_ser1_alt_8sec: TARGET = avr32da48
dx32_ser1_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser1_alt_8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART1 alternate pins. 8 sec timeout for manual reset.\n"


dx32_ser2_8sec: TARGET = avr32da48
dx32_ser2_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser2_8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 default pins. 8 sec timeout for manual reset.\n"


dx32_ser2_alt_8sec: TARGET = avr32da48
dx32_ser2_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser2_alt_8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART2 alternate pins. 8 sec timeout for manual reset.\n"


dx32_ser3_8sec: TARGET = avr32da48
dx32_ser3_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser3_8sec.hex UARTTX=B0 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 default pins. 8 sec timeout for manual reset.\n"


dx32_ser3_alt_8sec: TARGET = avr32da48
dx32_ser3_alt_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser3_alt_8sec.hex UARTTX=B4 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART3 alternate pins. 8 sec timeout for manual reset.\n"


dx32_ser4_8sec: TARGET = avr32da48
dx32_ser4_8sec:
	$(MAKE) -f $(MF) optiboot_dx32_ser4_8sec.hex UARTTX=E0 TIMEOUT=8 LED=A7

HELPTEXT += "target 32Dx - any AVR32DA or AVR32DB device, USART4 default pins. 8 sec timeout for manual reset.\n"


#---------------------------------------------------------------------------
#
# Generic build instructions
#

FORCE:

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
