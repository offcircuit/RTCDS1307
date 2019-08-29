#ifndef RTC_H
#define RTC_H

#include "Wire.h"

#define DS1307 0x68

#define RTCAM 0
#define RTCPM 1

struct chronos {
  uint16_t year;
  uint8_t month, weekday, day, hour, minute, second;
  bool split, period;
};

class RTC {
  public:
    RTC(uint8_t model, uint16_t start = 2000): _model(model), _start(start) {
      Wire.begin();
    };

    void split(bool turn) {
      chronos current;
      read(current);
      current.hour += !turn * current.split * current.period * 12;
      current.split = turn;
      write(current);
    }

    void read(chronos &current) {
      Wire.beginTransmission(_model);
      Wire.write(0);
      Wire.endTransmission();

      Wire.requestFrom((int)_model, 7);

      current.second = decimal(Wire.read() & 0x7F);
      current.minute = decimal(Wire.read() & 0x7F);

      uint8_t data = Wire.read();
      current.split = bitRead(data, 6);
      current.period = current.split & bitRead(data, 5);

      if (current.split) {
        current.hour = decimal(data & 0x1F) % 12;

      } else {
        current.hour = decimal(data & 0x3F);
      }

      current.weekday = decimal(Wire.read());
      current.day = decimal(Wire.read());
      current.month = decimal(Wire.read());
      current.year = decimal(Wire.read()) + _start;
    }

    bool set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
      chronos current;
      read(current);
      current.year = year;
      current.month = month;
      current.day = day;
      current.hour = hour;
      current.minute = minute;
      current.second = second;
      return write(current);

    }

    bool write(chronos current) {
      if (current.second == min(uint8_t(59), current.second)) {

        if (current.minute == min(uint8_t(59), current.minute)) {

          if (current.hour == min(uint8_t(23), current.hour)) {

            current.period = current.split * (current.hour > 11);
            current.hour += current.split * (current.hour == 0) * 12;
            uint8_t n[12] = {31, 28 + isLeapYear(current.year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            if (current.day == constrain(current.day, 1, n[current.month - 1])) {

              if (current.month == constrain(current.month, 1, uint8_t(12))) {

                current.weekday = wday(current.year, current.month, current.day);
                current.year = current.year - _start;

                if (current.year == min(uint8_t(99), uint8_t(current.year))) {

                  Wire.beginTransmission(_model);
                  Wire.write(0x00);
                  Wire.write(bcd(current.second));
                  Wire.write(bcd(current.minute));
                  Wire.write(bcd(current.hour) | current.split << 6 | current.period << 5);
                  Wire.write(bcd(current.weekday));
                  Wire.write(bcd(current.day));
                  Wire.write(bcd(current.month));
                  Wire.write(bcd(current.year));
                  Wire.endTransmission();
                  
                  return true;
                }
              }
            }
          }
        }
      }
      return false;
    }









    chronos fixChronos(chronos current) {
      current.second = min(uint8_t(59), current.second);
      current.minute = min(uint8_t(59), current.minute);

      current.hour = min(uint8_t(23), current.hour);

      if (current.split) {
        if (current.hour > 11) current.period = 1;
        if (current.hour > 12) current.hour -= 12;
        if (current.hour == 0) current.hour = 12;
      }

      current.weekday = wday(current.year, current.month, current.day);

      uint8_t n[12] = {31, 28 + isLeapYear(current.year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      current.day = constrain(current.day, 1, n[current.month - 1]);

      current.month = constrain(current.month, 1, uint8_t(12));
      current.year = min(uint8_t(99), uint8_t(current.year - _start));

      return current;
    }


  private:

    uint8_t _model;
    uint16_t _start;

    uint8_t bcd(uint8_t val)
    {
      return val + 6 * (val / 10);;
    }

    uint8_t decimal(uint8_t val)
    {
      return val - 6 * (val >> 4);
    }

    bool isLeapYear(uint16_t Y) {
      return !((Y % 4) * (!(Y % 100) + (Y % 400)));
    }

    uint8_t wday(uint16_t Y, uint8_t M, uint8_t D) {
      uint8_t n[12] = {31, 28 + isLeapYear(Y), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      Y = (D + (((Y - 1) * 365UL) + ((Y - 1) / 4) - ((Y - 1) / 100) + ((Y - 1) / 400))) % 7;
      while (M > 1) Y += n[--M - 1];
      return (Y % 7) + 1;
    }


};


#endif
