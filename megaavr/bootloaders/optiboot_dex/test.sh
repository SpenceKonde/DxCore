#!/bin/sh
# make bootloader hexfile for all MCUs avr[0-9]*[d,e][a-t] 

#avrtools=~/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino7b1/
avrtools=~/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino8a/

rm logfile
rm config_avr*

for name in $(ls $avrtools/avr/include/avr* | sed -n -e 's/.*io\(avr[0-9]*[d,e][a-t].*\).h/\1/p' ); do
  ./configure $name PA6 7 PC0 115200 > /dev/null
  if [ $? -eq 1 ]; then ./configure $name PA6 7 PC1 115200 > /dev/null ; fi
  make -f Makefile ${name}.hex >>logfile 2>&1 
  if [ $? -ne 0 ] ; then echo error: $name; fi
done


