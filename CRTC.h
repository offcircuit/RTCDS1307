#ifndef CRTC_H
#define CRTC_H

#include "Wire.h"

#define DS1307 0x68

#define RTCAM 0
#define RTCPM 1

struct chronos {
  uint16_t year;
  uint8_t month, weekday, day, hour, minute, second;
  bool split, period;
};

class CRTC {
  public:
    CRTC(uint8_t model, uint16_t start = 2000): _model(model), _start(start) {
      Wire.begin();
    };

    chronos read() {
      Wire.beginTransmission(_model);
      Wire.write(0);
      Wire.endTransmission();

      Wire.requestFrom((int)_model, 7);

      chronos current;
      current.second = decimal(Wire.read() & 0x7F);
      current.minute = decimal(Wire.read() & 0x7F);

      uint8_t data = Wire.read();
      current.split = bitRead(data, 6);
      current.period = current.split & bitRead(data, 5);
      current.hour = current.split ? decimal(data & 0x1F) % 12 : decimal(data & 0x3F);

      current.weekday = decimal(Wire.read());
      current.day = decimal(Wire.read());
      current.month = decimal(Wire.read());
      current.year = decimal(Wire.read()) + _start;

      return current;
    }

    bool write(chronos current) {
      if (current.second == min(uint8_t(59), current.second))
        if (current.minute == min(uint8_t(59), current.minute))
          if (current.hour == min(uint8_t(23), current.hour)) {

            current.period = current.split * (current.hour > 11);
            current.hour -= current.period * 12;
            current.hour += current.split * (current.hour == 0) * 12;
            uint8_t n[12] = {31, 28 + isLeapYear(current.year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            if (current.day == constrain(current.day, 1, n[current.month - 1]))
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
      return false;
    }

    bool set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
      chronos current = read();
      current.year = year;
      current.month = month;
      current.day = day;
      current.hour = hour;
      current.minute = minute;
      current.second = second;
      return write(current);
    }

    bool split(bool state) {
      chronos current = read();
      current.hour += !state * current.split * current.period * 12;
      current.split = state;
      return write(current);
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
