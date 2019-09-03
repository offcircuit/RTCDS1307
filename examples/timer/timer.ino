#include <RTCDS1307.h>

RTCDS1307 rtc;

void setup()
{
  Serial.begin(9600);
  rtc.write(2019, 8, 29, 23, 59, 54);
}

void loop()
{
  if (rtc.read()) {
    if (!(rtc.second % 4)) rtc.cycle(1 - rtc.split);
    Serial.print("  Day_of_week:");
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
    Serial.print(" ");
    Serial.print(rtc.split ? (rtc.period ? "PM" : "AM"));
  }
  delay(1000);
}
