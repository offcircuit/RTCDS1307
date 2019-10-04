#include "RTCDS1307.h"

RTCDS1307 rtc(0x68);

uint8_t year;
uint8_t month, weekday, day, hour, minute, second;
bool mode = 0, period = 0;

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  rtc.setDate(19, 2, 28);
  rtc.setTime(23, 59, 50);
}

void loop()
{
  rtc.getDate(year, month, day, weekday);
  rtc.getTime(hour, minute, second, period);
  if (!(second % 3)) rtc.setMode(1 - rtc.getMode());
  rtc.getTime(hour, minute, second, period);

  Serial.print(weekday, DEC);
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
  Serial.print(rtc.getMode() ? (period ? " PM" : " AM") : "");
  Serial.println();
  delay(1000);
}
