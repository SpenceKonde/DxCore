#ifndef DXCORE_H

typedef enum X32K_TYPE
{
    X32K_LOWPWR_START31MS = (CLKCTRL_CSUT_1K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START500MS = (CLKCTRL_CSUT_16K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START1S = (CLKCTRL_CSUT_32K_gc|CLKCTRL_LPMODE_bm),
    X32K_LOWPWR_START2S = (CLKCTRL_CSUT_64K_gc|CLKCTRL_LPMODE_bm),
    X32K_HIGHPWR_START31MS = (CLKCTRL_CSUT_1K_gc),
    X32K_HIGHPWR_START500MS = (CLKCTRL_CSUT_16K_gc),
    X32K_HIGHPWR_START1S = (CLKCTRL_CSUT_32K_gc),
    X32K_HIGHPWR_START2S = (CLKCTRL_CSUT_64K_gc),
    X32K_EXTCLK = (CLKCTRL_SEL_bm)
} X32K_TYPE_t;

typedef enum X32K_ENABLE
{
    X32K_DISABLED = ( 0x00),
    X32K_ENABLED  = ( 0x01),
    X32K_ALWAYSON = ( 0x81),
} X32K_ENABLE_t;

// void configXOSC32K(X32K_TYPE_t, X32K_ENABLE_t)
// attempts to configure the external crystal or oscillator.
// see above for valid values of these two arguments. This handles the enable-locking of many of these bits.
// which means it may disable this clock source (CSUT is long enough that this likely matters!)
void configXOSC32K(X32K_TYPE_t settings, X32K_ENABLE_t enable)
{
  uint8_t newval=settings|enable;
  //if any of the bits are "enable protected" we need to turn off the external crystal/clock.
  if ((CLKCTRL.XOSC32KCTRLA ^ newval)&(CLKCTRL_SEL_bm|CLKCTRL_CSUT_gm)) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,CLKCTRL.XOSC32KCTRLA&0xFE); //disable external crystal
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm); //unclear if this is immediately cleared or not...
  }
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,newval);
}

// void disableXOSC32K()
// disables the external 32.768 kHz oscillator
void disableXOSC32K() {
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,(CLKCTRL.XOSC32KCTRLA&0xFE));
}


// since CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune in order to check the status...

// uint8_t enableAutoTune()
// if configXOSC32K() was previously called, those settings will be retained, otherwise external oscillator is enabled with
// 1 second startu time and normal (not low power) crystal.
// Returns 1 if autotune was not successfully enabled - waited for CSUT + 0.5 seconds, and status still reported XOSC32K as not running/stable!
// Returns 255 (-1) if the current main clock source isn't the internal HF oscillator (presumably it's either external clock/crystal)
// under this circumstance, autotune will not impact the main clock - and the main clock, in fact, is likely more accurate than autotune would achieve.
// Returns 0 if autotune is successfully enabled within the time permitted by XOSC32KCTRLA's currently configured CSUT bits.

uint8_t enableAutoTune()
{
  if ((CLKCTRL.MCLKCTRLA&0x0F)!=0) return 0xFF;
  if (CLKCTRL.XOSC32KCTRLA&0x01) {
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,X32K_HIGHPWR_START1S|X32K_ENABLED);
  }
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA,CLKCTRL.OSCHFCTRLA|0x01);
  uint8_t csut=(CLKCTRL.XOSC32KCTRLA&CLKCTRL_CSUT_gm)>>4;
  uint32_t verifytime=500+(csut==3?2000:500*csut);
  uint32_t startedAt=millis();
  while ((millis()-startedAt < verifytime)&&(!(CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm)));
  if (CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm){
    return 0;
  }
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA,CLKCTRL.OSCHFCTRLA&0xFE);// turn it back off - crystal not working
  return 1;
}


// uint8_t disableAutoTune()
// Returns 255 (-1) if autotune was not enabled.
// Returns 0 if autotune is successfully disabled.
int8_t disableAutoTune()
{
  if (CLKCTRL.OSCHFCTRLA&0x01) {
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA,CLKCTRL.OSCHFCTRLA&0xFE);
    return 0x00;
  } else {
    return 0xFF;
  }
}
#endif


#define MVIO_DISABLED (-128)
#define MVIO_BAD_FUSE (-64)
#define MVIO_UNDERVOLTAGE (1)
#define MVIO_OKAY (0)
#define MVIO_UNSUPPORTED (-1)
#ifdef MVIO
  #define getMVIOStatus() ((FUSE.SYSCFG1&0x18)==0x10?MVIO_DISABLED:((FUSE.SYSCFG1&0x18)==0x08?(MVIO.STATUS?MVIO_OKAY:MVIO_UNDERVOLTAGE):MVIO_BAD_FUSE))
#else
  #define getMVIOStatus() MVIO_UNSUPPORTED
#endif
