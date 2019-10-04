#include "RTCDS1307.h"

RTCDS1307::RTCDS1307(uint8_t address): _address(address) {};

bool RTCDS1307::begin() {
  Wire.begin();
  Wire.beginTransmission(_address);
  return !Wire.endTransmission(1);
}

uint8_t RTCDS1307::bcd(uint8_t val) {
  return val + (6 * (val / 10));
}

uint8_t RTCDS1307::decimal(uint8_t val) {
  return val - (6 * (val >> 4));
}

void RTCDS1307::getDate(uint8_t &Y, uint8_t &M, uint8_t &D, uint8_t &WD) {
  read(RTCDS1307_DATE, 4);
  WD = _buffer[0];
  D = decimal(_buffer[1]);
  M = decimal(_buffer[2]);
  Y = decimal(_buffer[3]);
}

bool RTCDS1307::getMode() {
  read(RTCDS1307_MODE, 1);
  return bitRead(_buffer[0], 6);
}

void RTCDS1307::getTime(uint8_t &h, uint8_t &m, uint8_t &s) {
  bool period;
  getTime(h, m, s, period);
}

void RTCDS1307::getTime(uint8_t &h, uint8_t &m, uint8_t &s, bool &period) {
  read(RTCDS1307_TIME, 3);
  s = decimal(_buffer[0]);
  m = decimal(_buffer[1]);
  h = decimal(_buffer[2] & (bitRead(_buffer[2], 6) ? 0x1F : 0x3F));
  period = bitRead(_buffer[2], 6) & bitRead(_buffer[2], 5);
}

bool RTCDS1307::isLeapYear(uint16_t Y) {
  return !((Y % 4) * (!(Y % 100) + (Y % 400)));
}

void RTCDS1307::read(uint8_t address, uint8_t length) {
  _buffer = (uint8_t *) malloc(length);
  write(address, 0);
  Wire.requestFrom(_address, length);
  while (Wire.available() < length);
  for (uint8_t i = 0; i < length; i++) _buffer[i] = uint8_t(Wire.read());
}

void RTCDS1307::read(uint8_t address, uint8_t *&buffer, uint8_t length) {
  read(RTCDS1307_RAM | address, length);
  buffer = (uint8_t *) malloc(length);
  memcpy(buffer, _buffer, length);
}

void RTCDS1307::setDate(uint8_t Y, uint8_t M, uint8_t D) {
  _buffer[0] = wday(Y, M, D);
  _buffer[1] = bcd(D);
  _buffer[2] = bcd(M);
  _buffer[3] = bcd(Y);
  write(RTCDS1307_DATE, 4);
}

void RTCDS1307::setMode(bool state) {
  read(RTCDS1307_MODE, 1);

  if (state ^ bitRead(_buffer[0], 6)) {
    bool period = bitRead(_buffer[0], 5);
    _buffer[0] = decimal(_buffer[0] & (bitRead(_buffer[0], 6) ? 0x1F : 0x3F));

    if (state) _buffer[0] += ((_buffer[0] == 12) - (period = (_buffer[0] > 11))) * 12;
    else _buffer[0] = (_buffer[0] % 12) + (period * 12);

    _buffer[0] = bcd(_buffer[0]) | state << 6 | (state & period) << 5;
    write(RTCDS1307_MODE, 1);
  }
}

void RTCDS1307::setTime(uint8_t h, uint8_t m, uint8_t s, bool mode, bool period) {
  _buffer[0] = bcd(s);
  _buffer[1] = bcd(m);
  _buffer[2] = bcd(h) | mode << 6 | (mode & period) << 5;
  write(RTCDS1307_TIME, 3);
}

uint8_t RTCDS1307::wday(uint16_t Y, uint8_t M, uint8_t D) {
  uint8_t n[12] = {31, 28 + isLeapYear(Y), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  Y = (D + (((Y - 1) * 365UL) + ((Y - 1) / 4) - ((Y - 1) / 100) + ((Y - 1) / 400))) % 7;
  while (M > 1) Y += n[--M - 1];
  return (Y % 7) + 1;
}

void RTCDS1307::write(uint8_t address, uint8_t length) {
  Wire.beginTransmission(_address);
  Wire.write(address);
  for (uint8_t i = 0; i < length; i++) Wire.write(_buffer[i]);
  Wire.endTransmission(1);
}

void RTCDS1307::write(uint8_t address, uint8_t *buffer, uint8_t length) {
  memcpy(_buffer, buffer, length);
  write(RTCDS1307_RAM | address, length);
}
