#include <iostream>
#include <iomanip>

#include "AppImageJpeg.h"
#include "AppDefine.h"

CAppImageJpeg::CAppImageJpeg() : CAppImage()
{
  m_vt_BitData.clear();
}

CAppImageJpeg::~CAppImageJpeg()
{
  m_vt_BitData.clear();
}

int CAppImageJpeg::ParseMarker()
{
  uint64_t pos = 0;
  bool bError = false;
  bool isInScan = false;

  if (IsDataNull()) {
    bError = true;
  }

  while (!bError && IsParsePosValid(pos)) {

    if (isInScan) {
      if (MoveParsePosToScanEnd(pos) == RET_OK) {
        isInScan = false;
      }
      else {
      }
    }
    else {
    }

    if (m_pData[pos] != 0xFF) {
      cout << "Unexpected Format! Data Fragment Happened." << endl << endl;
      bError = true;
      break;
    }

    while (IsParsePosValid(pos) && m_pData[pos] != 0xFF) {
      // Refer to API Specifications, section 1.5.1. Fill Byte.
      pos++;
    }

    if (!IsParsePosValid(pos)) {
      bError = true;
      break;
    }

    const uint8_t markerCode = m_pData[pos + 1];

    PrintMarkerPos(markerCode, pos);
    MoveParseMarkerOffset(pos);

    if (!HasMarkerSegment(markerCode)) {

      // SOI|(EOI)|(RSTn)
      PrintBreakLine(markerCode);
    }
    else {
      // Other markers
      ParseMarkerSegment(markerCode, pos);

      if (markerCode == SOS) {
        isInScan = true;
      }
    }
  }

  int nRet = RET_OK;

  if (bError) {
    nRet = RET_ERR;
  }
  else {
    nRet = RET_OK;
  }

  return (nRet);
}

int CAppImageJpeg::MoveParseMarkerOffset(uint64_t &pos) const
{
  pos += 2;

  return (RET_OK);
}

int CAppImageJpeg::MoveParsePosToScanEnd(uint64_t &pos) const
{
  bool bError = false;
  bool bFound = false;

  bool bFirstRst = true;
  uint8_t nExepectRst = RST0;

  while (!bError && !bFound) {
    if (!IsParsePosValid(pos)) {
      // TODO check if this is a valid logic
      break;
    }

    if (m_pData[pos] == BYTE_MARKER_PREFIX && IsParsePosValid(pos + 1)) {

      switch (m_pData[pos + 1]) {
        case BYTE_STUFFING: {
          MoveParseMarkerOffset(pos);
        } break;

        case RST0:
        case RST1:
        case RST2:
        case RST3:
        case RST4:
        case RST5:
        case RST6:
        case RST7: {
          uint8_t markerCode = m_pData[pos + 1];

          if (bFirstRst) {
            bFirstRst = false;
          }

          if (nExepectRst != markerCode) {
            // TODO: test
            bError = true;
          }

          nExepectRst = GetNextExpectRst(markerCode);

          MoveParseMarkerOffset(pos);
        } break;

        case BYTE_MARKER_PREFIX: {
          // Assume fill byte of RST (0xFFFFDn, 0xFFFFFFDn (n =
          // {0,1,2,3,4,5,6,7}))
          while (m_pData[pos + 1] == BYTE_MARKER_PREFIX) {
            pos++;
            if (!IsParsePosValid(pos)) {
              // TODO: test
              bError = true;
              break;
            }
          }
        } break;

        default: {
          // TODO: test
          bFound = true;
        } break;
      }
    }
    else {
      pos++;
    }
  }

  int nRet = RET_OK;

  if (bError) {
    nRet = RET_ERR;
  }
  else {
    nRet = RET_OK;
  }

  return (nRet);
}

bool CAppImageJpeg::IsParsePosValid(const uint64_t pos) const
{
  bool bRet = false;

  if (pos + 1 <= GetFileSize()) {
    bRet = true;
  }
  else {
    bRet = false;
  }

  return (bRet);
}

///
/// input:
/// markerCode must be within a rage of [MARKER_RST_MIN, MARKER_RST_MAX]
///
///
uint8_t CAppImageJpeg::GetNextExpectRst(uint8_t markerCode) const
{
  uint8_t nNextRst = 0x00;

  switch (markerCode) {

    case RST0: {
      nNextRst = RST1;
    } break;

    case RST1: {
      nNextRst = RST2;
    } break;

    case RST2: {
      nNextRst = RST3;
    } break;

    case RST3: {
      nNextRst = RST4;
    } break;

    case RST4: {
      nNextRst = RST5;
    } break;

    case RST5: {
      nNextRst = RST6;
    } break;

    case RST6: {
      nNextRst = RST7;
    } break;

    case RST7: {
      nNextRst = RST0;
    } break;

    default: {
      // TODO: handling exceptions
      throw out_of_range("Invalid Argument for GetNextExpectRst: " +
                         to_string(markerCode));
    } break;
  }

  return (nNextRst);
}

int CAppImageJpeg::PrintMarkerPos(uint8_t markerCode, uint64_t pos) const
{
  string markerName = "";

  if (m_MarkerMap.contains(markerCode)) {
    markerName = m_MarkerMap.at(markerCode);
  }
  else {
    markerName = "Unknown";
  }

  cout << "Found marker: 0xFF" << hex << uppercase
       << static_cast<int>(markerCode) << " (" << markerName
       << ") at position 0x" << hex << pos;

  return (0);
}

int CAppImageJpeg::PrintBreakLine(uint8_t markerCode) const
{
  switch (markerCode) {
    case 0xD8: {
      cout << endl;
    } break;

    case 0xD0:
    case 0xD1:
    case 0xD2:
    case 0xD3:
    case 0xD4:
    case 0xD5:
    case 0xD6:
    case 0xD7: {
      cout << endl;
    } break;

    case 0xD9: {
      cout << endl << endl;
    } break;
  }

  return (0);
}

int CAppImageJpeg::PrintMarkerLen(uint16_t markerLen) const
{

  cout << ", Length: " << dec << markerLen << endl;

  return (0);
}

///
/// Return Values:
/// true  :   when SOI | EOI | RSTn
/// false :   other markers or invalid markers
///
bool CAppImageJpeg::HasMarkerSegment(uint8_t markerCode) const
{

  bool bRet = true;

  if (markerCode == SOI || markerCode == EOI) {
    bRet = false;
  }
  else if (markerCode >= RST0 && markerCode <= RST7) {
    bRet = false;
  }

  return (bRet);
}

int CAppImageJpeg::ParseMarkerSegment(uint8_t markerCode, uint64_t &pos)
{
  const uint16_t len = (m_pData[pos] << 8) | m_pData[pos + 1];

  PrintMarkerLen(len);

  for (int i = 0; i < SUPPORTED_MARKER_NUMBER; i++) {
    if (markerCode == m_FuncTable[i].marker) {
      (this->*(m_FuncTable[i].pFunc))(m_pData.get() + pos, len);
      break;
    }
  }

  pos += len;

  return (RET_OK);
}

int CAppImageJpeg::ObtainImageData(uint64_t &pos)
{
  return (RET_OK);
}

int CAppImageJpeg::IncrPtrToMarker(byte *pData)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseSOI(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseEOI(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseAPPn(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseDHT(const uint8_t *pData, const uint16_t nLength)
{
  uint64_t pos = 0;
  _DHT_SEGMENT dht_segment;

  dht_segment.seg.marker = ((BYTE_MARKER_PREFIX << 8) | DHT);
  dht_segment.seg.length = *(pData + pos) << 8 | *(pData + pos + 1);
  MoveParseMarkerOffset(pos);

  while (pos < nLength) {
    _DHT dht;

    dht.tc = *(pData + pos) & 0xF0;
    dht.th = *(pData + pos) & 0x0F;
    pos++;

    for (int i = 0; (i < 16 && pos < nLength); i++) {
      dht.l[i] = *(pData + pos);
      pos++;
    }

    cout << "DHT L[16] = ";
    DumpPtr(dht.l, 16);

    dht.v.clear();

    for (int i = 0; (i < 16 && pos < nLength); i++) {
      vector<uint8_t> vt_v;
      for (int j = 0; (j < dht.l[i] && pos < nLength); j++) {
        vt_v.push_back(*(pData + pos));
        pos++;
      }
      dht.v.push_back(vt_v);
    }
  }

  return (RET_OK);
}

int CAppImageJpeg::ParseSOF0(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseSOF2(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseSOF3(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseDQT(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseSOS(const uint8_t *pData, const uint16_t nLength)
{

  return (RET_OK);
}

int CAppImageJpeg::ParseCompressedData(const uint8_t *pData, uint64_t &pos)
{
  // TODO

  return (RET_OK);
}

int CAppImageJpeg::ParseRSTn(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}

int CAppImageJpeg::ParseDRI(const uint8_t *pData, const uint16_t nLength)
{
  return (RET_OK);
}
