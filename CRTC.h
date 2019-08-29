#ifndef CRTC_H
#define CRTC_H

#include "Wire.h"

#define DS1307 0x68

class CRTC {
  public:
    CRTC(uint8_t model = DS1307, uint16_t start = 2000): _model(model), _start(start) {
      Wire.begin();
    };

    uint8_t read(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second, bool &split, bool &period) {
      Wire.beginTransmission(_model);
      Wire.write(0);
      Wire.endTransmission();

      Wire.requestFrom((int)_model, 7);

      second = decimal(Wire.read());
      minute = decimal(Wire.read());

      uint8_t data = Wire.read();
      split = bitRead(data, 6);
      period = split & bitRead(data, 5);
      hour = decimal(data & (split ? 0x1F : 0x3F));

      uint8_t weekday = decimal(Wire.read());
      day = decimal(Wire.read());
      month = decimal(Wire.read());
      year = decimal(Wire.read()) + _start;
      return weekday;
    }

    uint8_t write(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool split = false, bool period = false) {
      if (second == min(uint8_t(59), second))
        if (minute == min(uint8_t(59), minute)) {
          if (hour == constrain(hour, split, uint8_t(split ? 12 : 23))) {

            uint8_t n[12] = {31, 28 + isLeapYear(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            if (day == constrain(day, 1, n[month - 1]))
              if (month == constrain(month, 1, uint8_t(12))) {
                uint8_t weekday = wday(year, month, day);
                year = year - _start;

                if (year == min(uint8_t(99), uint8_t(year))) {
                  Wire.beginTransmission(_model);
                  Wire.write(0x00);
                  Wire.write(bcd(second));
                  Wire.write(bcd(minute));
                  Wire.write(bcd(hour) | (split << 6) | ((split & period) << 5));
                  Wire.write(bcd(weekday));
                  Wire.write(bcd(day));
                  Wire.write(bcd(month));
                  Wire.write(bcd(year));
                  Wire.endTransmission();
                  return weekday;
                }
              }
          }
        }
      return false;
    }

    uint8_t split(bool state) {
      uint16_t year;
      uint8_t month, weekday, day, hour, minute, second;
      bool split, period;

      if (read(year, month, day, hour, minute, second, split, period)) {

        if (state ^ split) {

          if (state) {
            period = hour > 11;
            hour -= period * 12;
            hour += (hour == 0) * 12;

          } else {
            hour = (hour % 12) + (period * 12);
          }
        }

        return write(year, month, day, hour, minute, second, state, period);
      }
      return false;
    }

  private:
    uint8_t _model;
    uint16_t _start;

    uint8_t bcd(uint8_t data) {
      return data + 6 * (data / 10);;
    }

    uint8_t decimal(uint8_t data) {
      return data - 6 * (data >> 4);
    }

    bool isLeapYear(uint16_t year) {
      return !((year % 4) * (!(year % 100) + (year % 400)));
    }

    uint8_t wday(uint16_t year, uint8_t month, uint8_t day) {
      uint8_t n[12] = {31, 28 + isLeapYear(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      year = (day + (((year - 1) * 365UL) + ((year - 1) / 4) - ((year - 1) / 100) + ((year - 1) / 400))) % 7;
      while (month > 1) year += n[--month - 1];
      return (year % 7) + 1;
    }
};


#endif
