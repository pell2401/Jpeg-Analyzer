#include <iostream>
#include <iomanip>
#include <format>

#include "AppImageJpeg.h"
#include "AppDefine.h"

using namespace std;

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

  const uint64_t origin_pos = pos;

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
      cout << std::vformat("{:08b}", std::make_format_args(m_pData[pos]));
      pos++;
    }
  }
  cout << endl << endl;
  cout << "count: " << (int)(pos - origin_pos) << " [byte]" << endl;

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
       << ") at position 0x" << hex << uppercase << pos;

  cout << dec << setw(0) << setfill(' ');

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

int CAppImageJpeg::ClearMemoryEx()
{
  m_vt_BitData.clear();
  m_sof.clear();

  for (int tc = 0; tc < MAX_DHT_TC_COUNT; tc++) {
    for (int th = 0; th < MAX_DHT_TH_COUNT; th++) {
      m_vt_dht[tc][th].clear();
    }
  }

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
  for (uint16_t nPos = 0; nPos < nLength; nPos++) {
    cout << hex << uppercase << setw(2) << setfill('0') << (int)pData[nPos]
         << " ";

    if (nPos % 16 == 15) {
      cout << endl;
    }
  }

  if (nLength % 16 != 0) {
    cout << endl;
  }

  cout << dec << nouppercase << setw(0) << setfill(' ');

  return (RET_OK);
}

int CAppImageJpeg::ParseDHT(const uint8_t *pData, const uint16_t nLength)
{
  uint64_t pos = 0;
  _DHT_SEGMENT dht_segment;

  dht_segment.seg.marker = ((BYTE_MARKER_PREFIX << 8) | DHT);
  dht_segment.seg.length = *(pData + pos) << 8 | *(pData + pos + 1);
  MoveParseMarkerOffset(pos);

  cout << "========DHT=========" << endl;

  while (pos < nLength) {
    _DHT dht;

    dht.tc = *(pData + pos) >> 4;
    dht.th = (*(pData + pos) & 0x0F);
    pos++;

    for (int i = 0; (i < 16 && pos < nLength); i++) {
      dht.l[i] = *(pData + pos);
      pos++;
    }

    cout << "DHT L[16] = ";
    DumpPtr(dht.l, 16);

    dht.v.clear();

    for (int i = 0; (i < 16 && pos < nLength); i++) {
      for (int j = 0; (j < dht.l[i] && pos < nLength); j++) {
        dht.v.push_back(*(pData + pos));
        pos++;
      }
    }

    MakeHuffmanTree(dht.tc, dht.th, dht.l, dht.v);
    PrintHuffmanTree(dht.tc, dht.th);
    dht_segment.dht.push_back(dht);
  }

  cout << "====================" << endl;

  return (RET_OK);
}

int CAppImageJpeg::ValidateTcTh(const uint8_t tc, const uint8_t th) const
{
  if (tc != 0 && tc != 1) {
    return (RET_ERR);
  }

  if (th != 0 && th != 1 && th != 2 && th != 3) {
    return (RET_ERR);
  }

  return (RET_OK);
}

int CAppImageJpeg::MakeHuffmanTree(const uint8_t tc, const uint8_t th,
                                   const uint8_t l[16],
                                   const vector<uint8_t> &v)
{
  _HUFFMAN_TABLE table;
  uint16_t code = 0x0000;
  uint16_t pos = 0x0000;

  if (ValidateTcTh(tc, th) == RET_ERR) {
    return (RET_ERR);
  }

  vector<_HUFFMAN_TABLE> *pTable = &m_vt_dht[tc][th];

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < l[i]; j++) {
      table.length = i + 1;
      table.code = code++;
      table.value = v[pos++];
      pTable->push_back(table);
    }
    code <<= 1;
  }

  return (RET_OK);
}

int CAppImageJpeg::PrintHuffmanTree(const uint8_t tc, const uint8_t th) const
{
  if (ValidateTcTh(tc, th) == RET_ERR) {
    return (RET_ERR);
  }

  const vector<_HUFFMAN_TABLE> *pTable = &m_vt_dht[tc][th];

  cout << "table class: " << (int)tc << ", ";
  cout << "table identifier: " << (int)th << endl;

  if (tc == 0) {
    cout << "value (decimal) : code (binary)" << endl;
  }
  else if (tc == 1) {
    cout << "value (RRRR/SSSS) : code (binary)" << endl;
  }

  for (int i = 0; i < pTable->size(); i++) {
    _HUFFMAN_TABLE table = pTable->at(i);

    char format[64] = {0x00};
    snprintf(format, 64, "{:0%db}", table.length);

    if (tc == 0) {
      // DC
      cout << (int)table.value;
    }
    else if (tc == 1) {
      // AC
      uint8_t run = (table.value) >> 4;
      uint8_t size = table.value & 0x0F;
      if (table.value == 0x00) {
        cout << "EOB (End Of Block)";
      }
      else if (table.value == 0xF0) {
        cout << "ZRL (Zero RunLength)";
      }
      else {
        cout << (int)run << "/" << (int)size;
      }
    }

    cout << " : ";
    cout << std::vformat(format, std::make_format_args(table.code));
    cout << endl;
  }

  return (RET_OK);
}

int CAppImageJpeg::ParseSOF0(const uint8_t *pData, const uint16_t nLength)
{
  if (nLength < 8) {
    return (RET_ERR);
  }

  m_sof.seg.length = (pData[0] << 8) | pData[1];
  m_sof.sample_rate = pData[2];
  m_sof.width = (pData[3] << 8) | pData[4];
  m_sof.height = (pData[5] << 8) | pData[6];
  m_sof.num = pData[7];

  if ((nLength - 8) % 3 != 0) {
    return (RET_ERR);
  }

  for (int n = 0; n < m_sof.num; n++) {

    _SOF_PART part;

    part.index = pData[8 + 3 * n];
    part.h_rate = pData[8 + 3 * n + 1] >> 4;
    part.v_rate = pData[8 + 3 * n + 1] & 0x0F;
    part.dqt_no = pData[8 + 3 * n + 2];

    m_sof.vt_element.push_back(part);
  }

  cout << hex << uppercase << setw(0) << setfill('0');

  cout << "========SOF0========" << endl;
  cout << "sample rate: 0x" << (int)m_sof.sample_rate << endl;
  cout << "width:  0x" << (int)m_sof.width << endl;
  cout << "height: 0x" << (int)m_sof.height << endl;
  cout << dec;
  cout << "component num: " << (int)m_sof.num << endl;

  for (int i = 0; i < m_sof.vt_element.size(); i++) {
    const _SOF_PART *part = &m_sof.vt_element[i];
    cout << "------------" << endl;
    cout << "index: " << (int)part->index << endl;
    cout << "hn: " << (int)part->h_rate << endl;
    cout << "vn: " << (int)part->v_rate << endl;
    cout << "DQT no: " << (int)part->dqt_no << endl;
  }

  cout << "====================" << endl;

  cout << dec << nouppercase << setw(0) << setfill(' ');

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
