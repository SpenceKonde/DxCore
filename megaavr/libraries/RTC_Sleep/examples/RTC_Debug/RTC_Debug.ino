#include <RTC_Sleep.h>

extern char __rtc_intflags;
extern void rtc_sleep_setup();

void setup()
{
    Serial.begin(115200);
    delay(512);
    rtc_sleep_setup();
    Serial.printf("start\n");
    Serial.flush();
    while(millis() < 3) ;
    set_millis(0);
}

void loop()
{
    rtc_sleep(311);

}
