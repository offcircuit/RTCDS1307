#include "RTCDS1307.h"

RTCDS1307 rtc(2000);

void setup()
{
  Serial.begin(115200);
  rtc.write(2019, 8, 29, 23, 59, 54);
}

void loop()
{
  if (rtc.read()) {
    if (!(rtc.second % 4)) rtc.midday(1 - rtc.mode);
    Serial.print(rtc.weekday, DEC);
    Serial.print("  ");
    Serial.print(rtc.day, DEC);
    Serial.print("/");
    Serial.print(rtc.month, DEC);
    Serial.print("/");
    Serial.print(rtc.year, DEC);
    Serial.print("  ");
    Serial.print(rtc.hour, DEC);
    Serial.print(":");
    Serial.print(rtc.minute, DEC);
    Serial.print(":");
    Serial.print(rtc.second, DEC);
    Serial.println(rtc.mode ? (rtc.period ? " PM" : " AM") : "");
  }
  delay(1000);
}
