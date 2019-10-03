#ifndef RTCDS1307_H
#define RTCDS1307_H

#include "Wire.h"

#define RTC1307_TIMEADDR 0x00
#define RTC1307_DATEADDR 0x03
#define RTC1307_CONTROLADDR 0x07
#define RTC1307_RAMADDR 0x08

class RTCDS1307 {
  private:
  public:
    uint16_t _offset, _initial;
    uint8_t _address;
    bool _mode, _period;

    uint8_t bcd(uint8_t val) {
      return val + (6 * (val / 10));
    }

    uint8_t decimal(uint8_t val) {
      return val - (6 * (val >> 4));
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

    RTCDS1307(uint8_t address, uint16_t offset = 0, uint16_t initial = 2000):  _address(address), _offset(offset), _initial(initial) {
      _initial = 2000;
    };

    void begin() {
      Wire.begin();
      Wire.beginTransmission(_address);
      return Wire.endTransmission(1);
    }

    void getDate(uint16_t &Y, uint8_t &M, uint8_t &D, uint8_t &WD) {
      uint8_t *buffer;
      read(RTC1307_DATEADDR, buffer, 4);
      WD = decimal(buffer[0]);
      D = decimal(buffer[1]);
      M = decimal(buffer[2]);
      Y = decimal(buffer[3]) + _initial;
    }

    void getTime(uint8_t &h, uint8_t &m, uint8_t &s, bool &mode, bool &period) {
      uint8_t *buffer;
      read(RTC1307_TIMEADDR, buffer, 3);
      s = decimal(buffer[0]);
      m = decimal(buffer[1]);
      mode = bitRead(buffer[2], 6);
      period = mode & bitRead(buffer[2], 5);
      h = decimal(buffer[2] & (mode ? 0x1F : 0x3F));
    }

    void setDate(uint16_t Y, uint8_t M, uint8_t D) {
      uint8_t data[4] = {wday(Y, M, D), bcd(D), bcd(M), bcd(Y - _initial)};
      write(RTC1307_DATEADDR, data, 4);
    }

    void setTime(uint8_t h, uint8_t m, uint8_t s, bool mode = 0, bool period = 0) {
      uint8_t data[3] = {bcd(s), bcd(m), bcd(h) | mode << 6 | (mode & period) << 5};
      write(RTC1307_TIMEADDR, data, 3);
    }

    bool write(uint8_t address, uint8_t *data, uint8_t length) {
      Wire.beginTransmission(_address);
      Wire.write(address);
      for (uint8_t i = 0; i < length; i++) Wire.write(data[i]);
      return Wire.endTransmission(1);
    }

    bool read(uint8_t address, uint8_t *&buffer, uint8_t length) {
      buffer = (uint8_t *) realloc(buffer, length);
      Wire.beginTransmission(_address);
      Wire.write(address);
      Wire.endTransmission(1);
      Wire.requestFrom(_address, length);
      while (Wire.available() < length);
      for (uint8_t i = 0; i < length; i++) buffer[i] = uint8_t(Wire.read());
    }



























    /*    bool midday(bool state) {
          if ((read()) && (state ^ _mode)) {
            if (state) {
              _hour -= (_period = (_hour > 11)) * 12;
              _hour += (_hour == 0) * 12;
            } else _hour = (_hour % 12) + (_period * 12);
            return write(_year, _month, _day, _hour, _minute, _second, state, _period);
          }
          return false;
        }
    */
    bool setTimestamp(unsigned long t) {
      uint8_t s = t % 60;
      uint8_t m = (t /= 60) % 60;
      uint8_t h = (t /= 60) % 24;
      bool p = _mode * (h > 11);
      h += (_mode * !h * 12);

      uint16_t Y;
      for (Y = 1970; t > (365 + isLeapYear(Y)); Y++) t -= (365 + isLeapYear(Y));

      uint8_t M;
      uint8_t n[12] = {31, 28 + isLeapYear(Y), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      for (M = 1; t >= n[M - 1]; M++) t -= n[M - 1];

      if (Y < _initial) return false;
      //      else return write(Y, M, t + 1, h, m, s, _mode, p);
    }
};

#endif
