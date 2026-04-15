#include <RTC_Sleep.h>


static uint32_t timer_overflow_count;

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.printf("start\n");
    Serial.flush();
    rtc_sleep(1);
    while(millis() < 3) ; // wait for XTAL start-up
    set_millis(0);
}

long t=8192, time=0;
long delta=8192;
void loop()
{
    t=2000;// 8192;
    rtc_sleep(t);
    t=millis();
    delta=t-time;
    Serial.printf("time %ld  %ld  0x%2.2x\n", t, delta, SLPCTRL.CTRLA);
    time=t;
    Serial.flush();
}
