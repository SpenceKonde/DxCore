#include "DxCore.h"


TwoWire::TwoWire()
{
}

uint8_t enableAutoTune(uint16_t verifytime=0)
{
	_PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA,CLKCTRL.OSCHFCTRLA|0x01);
	if (verifytime!=0) {
		uint32_t startedAt=millis();
		while ((millis()-startedAt < verifytime)&&(CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm));
		if (CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm){
			return 1;
		}
	}
	return 0;
}

// settings should be one of
//  (CLKCTRL_CSUT_1K_gc), (CLKCTRL_CSUT_16K_gc), (CLKCTRL_CSUT_32K_gc) or (CLKCTRL_CSUT_64K_gc), optionally or'ed with (CLKCTRL_LPMODE_bm) and/or (CLKCTRL_SEL_bm)
uint8_t enableXOSC32K(uint8_t settings)
{
	_PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x23);
}


