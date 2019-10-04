#ifndef RTCDS1307_H
#define RTCDS1307_H

#include "Wire.h"
#include "Arduino.h"

#define RTCDS1307_TIME 0x00
#define RTCDS1307_MODE 0x02
#define RTCDS1307_DATE 0x03
#define RTCDS1307_CONTROL 0x07
#define RTCDS1307_RAM 0x08

class RTCDS1307 {
  private:
    uint8_t _address;
  public:

    RTCDS1307(uint8_t address);
    bool begin();
    uint8_t bcd(uint8_t val);
    uint8_t decimal(uint8_t val);
    void getDate(uint8_t &Y, uint8_t &M, uint8_t &D, uint8_t &WD);
    bool getMode();
    void getTime(uint8_t &h, uint8_t &m, uint8_t &s);
    void getTime(uint8_t &h, uint8_t &m, uint8_t &s, bool &period);
    bool isLeapYear(uint16_t Y);
    void setDate(uint8_t Y, uint8_t M, uint8_t D);
    void setMode(bool state);
    void setTime(uint8_t h, uint8_t m, uint8_t s, bool mode = 0, bool period = 0);
    uint8_t wday(uint16_t Y, uint8_t M, uint8_t D);



















};

#endif
