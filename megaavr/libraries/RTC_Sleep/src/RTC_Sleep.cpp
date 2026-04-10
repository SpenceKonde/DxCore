/*
  Low power hibernation for AVR DxCore, AVR xmega.
  These chips have an RTC clock running at 32kHz.
  When the RTC is used as Arduino millis() timer, this code
  uses the running RTC to implement a delay() equivalent that
  sends the chip into low power Sleep_Standby_Mode during delay.

  This relies on the RTC interrupt routine of DxCore/../cores/.../wiring.c
  which checks the RTC_CMP interrupt. If enabled, it will resume sleep.
*/

#include <RTC_Sleep.h>
#include <avr/sleep.h>

#define RTC_MAX 0xffff

void rtc_sleep(unsigned long dly)
{
  volatile unsigned int cntr, cnt;

  dly = dly + (dly>>6) + (dly>>7) + (dly>>11) + (dly>>14); // ms -> 1024Hz
  do {
    cnt = (dly > RTC_MAX) ? RTC_MAX : dly;
    dly =  dly - cnt;
    while (RTC.STATUS && RTC_CNTBUSY_bm); // wait for RTC.CNT sync
    cntr = RTC.CNT;
    while (RTC.STATUS && RTC_CMPBUSY_bm); // wait for RTC.CNT sync
    RTC.CMP = (cntr+cnt-1)&RTC_MAX;
    RTC.INTCTRL |= RTC_CMP_bm; //enable CMP Interrupt RTC_CMP

    set_sleep_mode(SLEEP_MODE_STANDBY);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    while (RTC.STATUS && RTC_CNTBUSY_bm);   // required for millis()

    #ifdef DEBUG
    {
    volatile int j,l,m;
    j = RTC.CNT&RTC_MAX;
    m = (j==0) ? 200:100;
    digitalWrite(DEBUG_PIN,0);
    for(l=0; l < m ; l++);
    digitalWrite(DEBUG_PIN,1);
    }
    #endif

    RTC.INTCTRL &= ~RTC_CMP_bm;
  } while ( dly );
}
