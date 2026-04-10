#include <RTC_Sleep.h>

void setup()
{
    Serial.begin(115200);
    Serial.printf("start")
}

void loop()
{
    rtc_sleep(5000000);
    Serial.printf("time %ld\n" millis());
}
