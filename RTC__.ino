#include "RTCDS1307.h"

RTCDS1307 rtc(0x68);

uint16_t year;
uint8_t month, weekday, day, hour, minute, second;
bool mode, period;

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  rtc.setDate(2019, 7, 16);
  rtc.setTime(11, 59, 50);
}

void loop()
{
  rtc.getDate(year, month, day, weekday);
  rtc.getTime(hour, minute, second, mode, period);

  //if (!(rtc.second % 4)) rtc.midday(1 - rtc.mode_);
  //Serial.print(weekday, DEC);
  Serial.print("  ");
  Serial.print(day, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print("  ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  // Serial.println(rtc.mode_ ? (rtc.period ? " PM" : " AM") : "");
  Serial.println();
  delay(1000);
}
