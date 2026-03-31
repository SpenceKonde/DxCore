# It is required to use the azduino avr-gcc, other compilers will bail out.
# The flash target assumes boot pagesize is 512 for all Dx Cores and 256 for all Ex Cores.
# avrdude >= 8.1 required

GCCROOT=~/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino8a
AVRDUDE=~/.arduino15/packages/DxCore/tools/avrdude/avrdude_v8.1_Linux_64bit/avrdude_Linux_64bit/bin/avrdude

CC=$(GCCROOT)/bin/avr-gcc
SIZE=$(GCCROOT)/bin/avr-size
OBJDUMP=$(GCCROOT)/bin/avr-objdump
OBJCOPY=$(GCCROOT)/bin/avr-objcopy

MCU?=$(basename $@)
ttyUPDI?=/dev/ttyUPDI
ttyUSB?=/dev/ttyUSB0

SOURCES=optiboot_dx.c

CFLAGS=-g -Os -Wall -mrelax -I $(GCCROOT)/avr/include/
LDFLAGS=-Wl,--relax
BLDFLAGS=-Wl,--relax -nostartfiles -nostdlib -Wl,-section-start=.text=0x000 -Wl,-section-start=.nvmc=0x1f8 -Wl,-section-start=.spmtarg=0x1fa -Wl,-section-start=.version=0x1fe

.PRECIOUS: %.hex

%.hex:	config_%.h $(SOURCES)
	$(CC) -include $(addsuffix .h,config_$(MCU)) -mmcu=$(MCU) $(CFLAGS) $(BLDFLAGS) $(SOURCES)
	@$(SIZE)
	@$(OBJDUMP)  -j .text -j .nvmc -j .spmtarg -j .version -D -S a.out > $(@:%.hex=%.lst)
	@$(OBJCOPY)  -j .text -j .nvmc -j .spmtarg -j .version -O ihex a.out $@
	@rm a.out

%.flash_ex:	%.hex
	$(AVRDUDE) -c serialupdi -p $(MCU) -P $(ttyUPDI) -b 115200 -v -e -U bootsize:w:0x02:m -U bodcfg:w:0x56:m -U syscfg0:w:0xd9:m -U flash:w:$<

%.flash_dx:	%.hex
	$(AVRDUDE) -c serialupdi -p $(MCU) -P $(ttyUPDI) -b 115200 -v -e -U bootsize:w:0x01:m -U bodcfg:w:0x56:m -U syscfg0:w:0xd9:m -U flash:w:$<

clean:
	rm -f *.lst *.hex a.out*
