#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include "AppImage.h"
#include "AppDataType.h"
#include "AppStruct.h"
#include "AppFuncType.h"
#include "AppDefine.h"

using namespace std;

class CAppImageJpeg : public CAppImage {
public:
  CAppImageJpeg();
  virtual ~CAppImageJpeg();

  int ParseMarker();

  int MoveParseMarkerOffset(uint64_t &pos) const;
  int MoveParsePosToScanEnd(uint64_t &pos) const;
  bool IsParsePosValid(const uint64_t pos) const;

  uint8_t GetNextExpectRst(uint8_t markerCode) const;

  int PrintMarkerPos(uint8_t markerCode, uint64_t pos) const;
  int PrintBreakLine(uint8_t markerCode) const;
  int PrintMarkerLen(uint16_t markerLen) const;

  bool HasMarkerSegment(uint8_t markerCode) const;
  int ParseMarkerSegment(uint8_t markerCode, uint64_t &pos);
  int ObtainImageData(uint64_t &pos);

private:
  int IncrPtrToMarker(byte *pData);

private:
  int ParseSOI(const uint8_t *pData, const uint16_t nLength);
  int ParseEOI(const uint8_t *pData, const uint16_t nLength);
  int ParseAPPn(const uint8_t *pData, const uint16_t nLength);

  int ParseDHT(const uint8_t *pData, const uint16_t nLength);
  int ParseSOF0(const uint8_t *pData, const uint16_t nLength);
  int ParseSOF2(const uint8_t *pData, const uint16_t nLength);
  int ParseSOF3(const uint8_t *pData, const uint16_t nLength);
  int ParseDRI(const uint8_t *pData, const uint16_t nLength);
  int ParseDQT(const uint8_t *pData, const uint16_t nLength);
  int ParseSOS(const uint8_t *pData, const uint16_t nLength);
  int ParseCompressedData(const uint8_t *pData, uint64_t &pos);
  int ParseRSTn(const uint8_t *pData, const uint16_t nLength);

private:
  _MARKER_HANDLER m_FuncTable[SUPPORTED_MARKER_NUMBER] = {

      {SOI, &CAppImageJpeg::ParseSOI},
      {EOI, &CAppImageJpeg::ParseEOI},

      //{APP0, &CAppImageJpeg::ParseAPPn},  {APP1, &CAppImageJpeg::ParseAPPn},
      //{APP2, &CAppImageJpeg::ParseAPPn},  {APP3, &CAppImageJpeg::ParseAPPn},
      //{APP4, &CAppImageJpeg::ParseAPPn},  {APP5, &CAppImageJpeg::ParseAPPn},
      //{APP6, &CAppImageJpeg::ParseAPPn},  {APP7, &CAppImageJpeg::ParseAPPn},
      //{APP8, &CAppImageJpeg::ParseAPPn},  {APP9, &CAppImageJpeg::ParseAPPn},
      //{APP10, &CAppImageJpeg::ParseAPPn}, {APP11, &CAppImageJpeg::ParseAPPn},
      //{APP12, &CAppImageJpeg::ParseAPPn}, {APP13, &CAppImageJpeg::ParseAPPn},
      //{APP14, &CAppImageJpeg::ParseAPPn}, {APP15, &CAppImageJpeg::ParseAPPn},

      //{RST0, &CAppImageJpeg::ParseRSTn},  {RST1, &CAppImageJpeg::ParseRSTn},
      //{RST2, &CAppImageJpeg::ParseRSTn},  {RST3, &CAppImageJpeg::ParseRSTn},
      //{RST4, &CAppImageJpeg::ParseRSTn},  {RST5, &CAppImageJpeg::ParseRSTn},
      //{RST6, &CAppImageJpeg::ParseRSTn},  {RST7, &CAppImageJpeg::ParseRSTn},

      {DHT, &CAppImageJpeg::ParseDHT},
      {DQT, &CAppImageJpeg::ParseDQT},

      {SOF0, &CAppImageJpeg::ParseSOF0},
      {SOF2, &CAppImageJpeg::ParseSOF2},
      {SOF3, &CAppImageJpeg::ParseSOF3},

      {SOS, &CAppImageJpeg::ParseSOS},
      {DRI, &CAppImageJpeg::ParseDRI},

  };

private:
  vector<uint8_t> m_vt_BitData;

private:
  map<uint8_t, string> m_MarkerMap = {
      {0xC0, "SOF0"},  {0xC2, "SOF2"},  {0xC3, "SOF3"},  {0xC4, "DHT"},

      {0xD0, "RST0"},  {0xD1, "RST1"},  {0xD2, "RST2"},  {0xD3, "RST3"},
      {0xD4, "RST4"},  {0xD5, "RST5"},  {0xD6, "RST6"},  {0xD7, "RST7"},

      {0xD8, "SOI"},   {0xD9, "EOI"},   {0xDA, "SOS"},   {0xDB, "DQT"},
      {0xDD, "DRI"},

      {0xE0, "APP0"},  {0xE1, "APP1"},  {0xE2, "APP2"},  {0xE3, "APP3"},
      {0xE4, "APP4"},  {0xE5, "APP5"},  {0xE6, "APP6"},  {0xE7, "APP7"},
      {0xE8, "APP8"},  {0xE9, "APP9"},  {0xEA, "APP10"}, {0xEB, "APP11"},
      {0xEC, "APP12"}, {0xED, "APP13"}, {0xEE, "APP14"}, {0xEF, "APP15"}};

private:
  vt_dht m_vt_dht;
  vt_dqt m_vt_dqt;
};
