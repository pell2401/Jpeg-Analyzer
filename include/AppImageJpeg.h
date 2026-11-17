#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <fstream>
#include <sstream>

#include "AppImage.h"
#include "AppFuncType.h"
#include "AppDefine.h"
#include "AppStruct.h"
#include "AppDataType.h"

class CAppImageJpeg : public CAppImage {
public:
  CAppImageJpeg();
  virtual ~CAppImageJpeg();

  // Public API for parsing JPEG
  int ParseMarker();

  // TODO: Clean up the design and redesign
  int MoveParseMarkerOffset(uint64_t &pos) const;
  int MoveParsePosToScanEnd(uint64_t &pos);
  bool IsParsePosValid(const uint64_t pos) const;
  uint8_t GetNextExpectRst(uint8_t markerCode) const;
  int PrintMarkerPos(uint8_t markerCode, uint64_t pos) const;
  int PrintBreakLine(uint8_t markerCode) const;
  int PrintMarkerLen(uint16_t markerLen) const;
  bool HasMarkerSegment(uint8_t markerCode) const;
  int ParseMarkerSegment(uint8_t markerCode, uint64_t &pos);
  int ObtainImageData(uint64_t &pos);

protected:
  // Override
  int ClearMemoryEx() override;

  // Private Methods
private:
  int DumpPtrFile(const uint8_t *ptr, const uint64_t nByte,
                  const uint64_t nOffset, std::ostream &os);

#pragma region I/O Helper
  int WriteOutputStream(std::ofstream &ofs, const std::stringstream &ss) const;
  int WriteFileAPPn(const std::stringstream &ss);
  int WriteFileCOM(const std::stringstream &ss);
  int WriteFileDQT(const std::stringstream &ss);
  int WriteFileDRI(const std::stringstream &ss);
  int WriteFileSOF(const std::stringstream &ss);
  int WriteFileDHT(const std::stringstream &ss);
  int WriteFileSOS(const std::stringstream &ss);
  int WriteFileRAW(const std::stringstream &ss);
#pragma endregion

private:
#pragma region TODO
  int ParseCOM(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int ParseDRI(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int ParseSOF2(const uint8_t marker, const uint8_t *pData,
                const uint16_t nLength);
  int ParseSOF3(const uint8_t marker, const uint8_t *pData,
                const uint16_t nLength);
  int ParseCompressedData(const uint8_t *pData, uint64_t &pos);
#pragma endregion

#pragma region APP Parser
  int ParseAPPn(const uint8_t marker, const uint8_t *pData,
                const uint16_t nLength);
  int PrintAPP(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int PrintAPPFormatHex(const uint8_t *pData, const uint16_t nLength);
  int PrintAPPFormatAscii(const uint8_t *pData, const uint16_t nLength);
  int PrintAPPFormatBoth(const uint8_t *pData, const uint16_t nLength);
  int PrintAPPFormatHexdump(const uint8_t *pData, const uint16_t nLength);
#pragma endregion

#pragma region DQT Parser
  int ParseDQT(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int PrintQuantizationTable(const uint8_t q);
#pragma endregion

#pragma region DHT Parser
  int ParseDHT(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int MakeHuffmanTree(const uint8_t tc, const uint8_t th, const uint8_t l[16],
                      const vector<uint8_t> &v);
  int PrintHuffmanTree(const uint8_t tc, const uint8_t th,
                       std::ostream &ss) const;
  int ValidateTcTh(const uint8_t tc, const uint8_t th) const;
#pragma endregion

#pragma region SOF Parser
  int ParseSOF0(const uint8_t marker, const uint8_t *pData,
                const uint16_t nLength);
  int PrintSOF();
#pragma endregion

#pragma region SOS Parser
  int ParseSOS(const uint8_t marker, const uint8_t *pData,
               const uint16_t nLength);
  int PrintSOS();
#pragma endregion

private:
  _MARKER_HANDLER m_FuncTable[SUPPORTED_MARKER_SEGMENT_NUMBER] = {

      {APP0, &CAppImageJpeg::ParseAPPn},
      {APP1, &CAppImageJpeg::ParseAPPn},
      {APP2, &CAppImageJpeg::ParseAPPn},
      {APP3, &CAppImageJpeg::ParseAPPn},
      {APP4, &CAppImageJpeg::ParseAPPn},
      {APP5, &CAppImageJpeg::ParseAPPn},
      {APP6, &CAppImageJpeg::ParseAPPn},
      {APP7, &CAppImageJpeg::ParseAPPn},
      {APP8, &CAppImageJpeg::ParseAPPn},
      {APP9, &CAppImageJpeg::ParseAPPn},
      {APP10, &CAppImageJpeg::ParseAPPn},
      {APP11, &CAppImageJpeg::ParseAPPn},
      {APP12, &CAppImageJpeg::ParseAPPn},
      {APP13, &CAppImageJpeg::ParseAPPn},
      {APP14, &CAppImageJpeg::ParseAPPn},
      {APP15, &CAppImageJpeg::ParseAPPn},
      {DHT, &CAppImageJpeg::ParseDHT},
      {DQT, &CAppImageJpeg::ParseDQT},
      {SOF0, &CAppImageJpeg::ParseSOF0},
      //{SOF2, &CAppImageJpeg::ParseSOF2},
      //{SOF3, &CAppImageJpeg::ParseSOF3},
      {SOS, &CAppImageJpeg::ParseSOS},
      {DRI, &CAppImageJpeg::ParseDRI},
      {COM, &CAppImageJpeg::ParseCOM},
  };

private:
  std::vector<uint8_t> m_vt_BitData;

private:
  std::map<uint8_t, string> m_MarkerMap = {
      {SOF0, "SOF0"},   {SOF2, "SOF2"},   {SOF3, "SOF3"},   {DHT, "DHT"},
      {RST0, "RST0"},   {RST1, "RST1"},   {RST2, "RST2"},   {RST3, "RST3"},
      {RST4, "RST4"},   {RST5, "RST5"},   {RST6, "RST6"},   {RST7, "RST7"},
      {SOI, "SOI"},     {EOI, "EOI"},     {SOS, "SOS"},     {DQT, "DQT"},
      {DRI, "DRI"},     {APP0, "APP0"},   {APP1, "APP1"},   {APP2, "APP2"},
      {APP3, "APP3"},   {APP4, "APP4"},   {APP5, "APP5"},   {APP6, "APP6"},
      {APP7, "APP7"},   {APP8, "APP8"},   {APP9, "APP9"},   {APP10, "APP10"},
      {APP11, "APP11"}, {APP12, "APP12"}, {APP13, "APP13"}, {APP14, "APP14"},
      {APP15, "APP15"}, {COM, "COM"}};

private:
  vt_DHT m_vt_dht;
  vt_DQT m_dqt;
  uint8_t m_count_dqt_table;
  _SOF m_sof;
  _SOS m_sos;
  _DRI m_dri;

  // Output Stream
  std::ofstream m_ofs_app;
  std::ofstream m_ofs_com;
  std::ofstream m_ofs_dqt;
  std::ofstream m_ofs_sof;
  std::ofstream m_ofs_dht;
  std::ofstream m_ofs_dri;
  std::ofstream m_ofs_sos;
  std::ofstream m_ofs_raw;
};
