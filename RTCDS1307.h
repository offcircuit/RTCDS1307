#ifndef RTCDS1307_H
#define RTCDS1307_H

#include "Wire.h"

#define RTCDS1307_TIME 0x00
#define RTCDS1307_MODE 0x02
#define RTCDS1307_DATE 0x03
#define RTCDS1307_CONTROL 0x07
#define RTCDS1307_RAM 0x08


class RTCDS1307 {
  private:
  public:
    uint8_t _address;

    RTCDS1307(uint8_t address): _address(address) {};

    bool begin() {
      Wire.begin();
      Wire.beginTransmission(_address);
      return Wire.endTransmission(1);
    }

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

    void setDate(uint8_t Y, uint8_t M, uint8_t D) {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_DATE);
      Wire.write(wday(Y, M, D));
      Wire.write(bcd(D));
      Wire.write(bcd(M));
      Wire.write(bcd(Y));
      Wire.endTransmission(1);
    }

    void getDate(uint8_t &Y, uint8_t &M, uint8_t &D, uint8_t &WD) {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_DATE);
      Wire.endTransmission(0);

      Wire.requestFrom(_address, uint8_t(4), uint8_t(1));
      while (Wire.available() < 4);
      WD = Wire.read();
      D = decimal(Wire.read());
      M = decimal(Wire.read());
      Y = decimal(Wire.read());
    }

    void setTime(uint8_t h, uint8_t m, uint8_t s, bool mode = 0, bool period = 0) {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_TIME);
      Wire.write(bcd(s));
      Wire.write(bcd(m));
      Wire.write(bcd(h) | mode << 6 | (mode & period) << 5);
      Wire.endTransmission(1);
    }

    void getTime(uint8_t &h, uint8_t &m, uint8_t &s, bool &period) {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_TIME);
      Wire.endTransmission(0);

      Wire.requestFrom(_address, uint8_t(3), uint8_t(1));
      while (Wire.available() < 3);
      s = decimal(Wire.read());
      m = decimal(Wire.read());

      h = Wire.read();
      period = bitRead(h, 6) & bitRead(h, 5);
      h = decimal(h & (bitRead(h, 6) ? 0x1F : 0x3F));
    }

    void getTime(uint8_t &h, uint8_t &m, uint8_t &s) {
      bool period;
      getTime(h, m, s, period);
    }

    void setMode(bool state) {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_MODE);
      Wire.endTransmission(0);

      Wire.requestFrom(_address, uint8_t(1), uint8_t(1));
      while (Wire.available() < 1);
      uint8_t h = Wire.read();

      if (state ^ bitRead(h, 6)) {
        bool period = bitRead(h, 5);
        h = decimal(h & (bitRead(h, 6) ? 0x1F : 0x3F));

        if (state) h += ((h == 12) - (period = (h > 11))) * 12;
        else h = (h % 12) + (period * 12);

        Wire.beginTransmission(_address);
        Wire.write(RTCDS1307_MODE);
        Wire.write(bcd(h) | state << 6 | (state & period) << 5);
        Wire.endTransmission(0);
      }
    }

    bool getMode() {
      Wire.beginTransmission(_address);
      Wire.write(RTCDS1307_MODE);
      Wire.endTransmission(0);

      Wire.requestFrom(_address, uint8_t(1), uint8_t(1));
      while (Wire.available() < 1);
      return bitRead(Wire.read(), 6);
    }





















};

#endif
