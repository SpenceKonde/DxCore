#!/bin/sh
#
# This script generates bootloader hex files for programming via the Arduino IDE and some helpers for the board.txt file.
#
# As ioavrxxYYzz.h appears to be generated, the comment lines of the USART PORTMUX are a reliable source for USART pin mapping.
# The typical USART PORTMUX line looks like this:
# PORTMUX_USART0_ALT1_gc = (0x01<<0),  /* TxD: PA4, RxD: PA5, XCK: PA6, XDIR: PA7 */
#
# The optiboot_dx.c relies on the azduino avr-gcc compiler, other compiler versions always require using the frame Pointer and won't compile.

INCROOT=${INCROOT:-~/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino8}
INCLUDES=${INCROOT}/avr/include/avr

if [ ! -d ${INCLUDES} ]; then echo path to include files: ${INCLUDES} not found. export INCROOT=path in environment; exit 1; fi

# Default values, LED connected to VDD
LED=${LED:-PA7}
TGCNT=${TGCNT:-6}
BAUD=${BAUD:-115200}

# Memory-Class & Entry-Condition iterators
CLASS="8 16 32 64"
ENTRYCOND="_extr _extronly _poronly_8sec _all_8sec _swronly_8sec _extr_8sec"
CHIPS="ea eb"

# init logs
echo >generate.log
echo >board.txt

for chip in $CHIPS; do
  echo "avr${chip}opti.bootloader.file={runtime.platform.path}/bootloaders/optiboot_dex/hex/{bootloader.class}{bootloader.port}{bootloader.entrycond}.hex" >>board.txt
  for class in $CLASS; do
    MCU=avr${class}${chip}28
    HEADER=io${MCU}.h
    if [ ! -f ${INCLUDES}/${HEADER} ] ; then
      echo ${INCLUDES}/${HEADER} not found; continue;
    fi
    SERIALS=$( sed -n -e '/PORTMUX_USART.*TxD:/s/.*USART\([0-9]\)_\([A-Z]*\)\([0-9]*\)_.* \/\* TxD: \(.*\), RxD: \(.*\), XCK.*/\4\5\1\2\3 /p' ${INCLUDES}/${HEADER})
    echo ${HEADER}

    for serial in $SERIALS; do
      txpin=$(echo $serial | cut -c -3 )
      rxpin=$(echo $serial | cut -c 4-6 )
      uart=$(echo $serial | cut -c 7-7 )
      alt=$(echo $serial | cut -c 8- )
      if [ $alt = DEFAULT ]; then alt= ; portswap=0; else
        portswap=$(echo $alt | cut -c 4- )
        alt=_alt${portswap}
      fi
      echo $txpin $rxpin USART$uart \"$alt\" ALT$portswap

      for ec in $ENTRYCOND; do
        case "$ec" in
          *_8sec) WDTTIME=8;;
          *) WDTTIME=1
        esac

        case "$ec" in
          _extr)         entrycond=0x34;;
          _extronly)     entrycond=0x04;;
          _all_8sec)     entrycond=0x35;;
          _poronly_8sec) entrycond=0x01;;
          _swronly_8sec) entrycond=0x10;;
          *) entrycond=0x35;;
        esac

        #echo configure $MCU $LED $TGCNT $txpin $BAUD $WDTTIME $entrycond

        if [ $ec = _extr ]; then
          if [ $class = 32 ]; then
           echo >>board.txt
           echo avr${chip}opti.menu.bootloaderusart.ser${uart}${alt}=USART${uart}: TX $txpin, RX $rxpin >>board.txt
           echo avr${chip}opti.menu.bootloaderusart.ser${uart}${alt}.bootloader.port=_ser${uart}${alt} >>board.txt
           if [ $uart = 0 ]; then uartx=; else uartx=$uart; fi
           echo avr${chip}opti.menu.bootloaderusart.ser${uart}${alt}.bootloader.portname=Serial${uartx} >>board.txt
           echo avr${chip}opti.menu.bootloaderusart.ser${uart}${alt}.bootloader.portswap=${portswap} >>board.txt
          fi
        fi

        ./configure $MCU $LED $TGCNT $txpin $BAUD $WDTTIME $entrycond >/dev/null
        make ${MCU}.hex >>generate.log
        mv ${MCU}.hex hex/optiboot_${class}${chip}_ser${uart}${alt}${ec}.hex
      done
    done
  done
done
