#include <RTC_Sleep.h>

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.printf("start\n");
    Serial.flush();
    while(millis() < 3) ; // wait for XTAL start-up
    set_millis(0);
}

long t=8192, time=0;
long delta=8192;
void loop()
{
    t=8192;
    rtc_sleep(t);
    t=millis();
    delta=t-time;
    Serial.printf("time %ld  %ld\n", t, delta);
    time=t;
    Serial.flush();
}
