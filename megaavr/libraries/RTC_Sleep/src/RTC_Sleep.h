#ifndef RTC_SLEEP_H
#define RTC_SLEEP_H

#include <Arduino.h>
#ifndef MILLIS_USE_TIMERRTC
    #error RTC_Sleep requires RTC TIMER
#endif
void rtc_sleep(unsigned long time);

#endif
