#include <iostream>
#include <iomanip>
#include <format>
#include <fstream>

#include <ostream>
#include <sstream>

#include "AppImageJpeg.h"
#include "AppDefine.h"
#include "AppData.h"
#include "AppIosFlagGuard.h"

using namespace std;

static const uint8_t zigzag_order[DQT_QK_MAX_COUNT] = {
    0,  1,  5,  6,  14, 15, 27, 28, 2,  4,  7,  13, 16, 26, 29, 42,
    3,  8,  12, 17, 25, 30, 41, 43, 9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63,
};

CAppImageJpeg::CAppImageJpeg() : CAppImage()
{
  m_vt_BitData.clear();

  if (CAppData::IsPrintAPP()) {
    m_ofs_app.open(APP_FILENAME, ios::out);
  }
  if (CAppData::IsPrintCOM()) {
    m_ofs_com.open(COM_FILENAME, ios::out);
  }
  if (CAppData::IsPrintDQT()) {
    m_ofs_dqt.open(DQT_FILENAME, ios::out);
  }
  if (CAppData::IsPrintSOF()) {
    m_ofs_sof.open(SOF_FILENAME, ios::out);
  }
  if (CAppData::IsPrintDHT()) {
    m_ofs_dht.open(DHT_FILENAME, ios::out);
  }
  if (CAppData::IsPrintDRI()) {
    m_ofs_dri.open(DRI_FILENAME, ios::out);
  }
  if (CAppData::IsPrintSOS()) {
    m_ofs_sos.open(SOS_FILENAME, ios::out);
  }
  if (CAppData::IsPrintRAW()) {
    m_ofs_raw.open(RAW_FILENAME, ios::out);
  }

  m_count_dqt_table = 0;
}

CAppImageJpeg::~CAppImageJpeg()
{
  m_vt_BitData.clear();

  if (m_ofs_dht.is_open()) {
    m_ofs_dht.close();
  }
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

      if (markerCode == EOI) {
        ClearMemoryEx();
      }
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

int CAppImageJpeg::MoveParsePosToScanEnd(uint64_t &pos)
{
  bool bError = false; // predictable errors flag
  bool bFound = false; // unexpected value encounter flag

  bool bFirstRst = true;
  uint8_t nExepectRst = RST0;
  const uint64_t origin_pos = pos;

  stringstream ss;

  while (!bError && !bFound) {
    if (!IsParsePosValid(pos)) {
      // TODO: Check if this is a valid logic
      break;
    }

    // Check parse position validity
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
          //
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
      if (CAppData::IsPrintRAW()) {
        ss << std::vformat("{:08b}", std::make_format_args(m_pData[pos]));
      }
      pos++;
    }
  }

  if (CAppData::IsPrintRAW()) {
    ss << endl << endl;
    ss << "count: " << (int)(pos - origin_pos) << " [byte]" << endl;
  }

  int nRet = RET_OK;

  if (bError) {
    nRet = RET_ERR;
  }
  else {
    nRet = RET_OK;
  }

  if (CAppData::IsPrintRAW()) {
    WriteFileRAW(ss);
  }

  return (nRet);
}

///
/// Return Values:
///
/// true  : Parse position is valid
/// false : Parse position is invalid
///
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

  // RAII (Resource Acquisition Is Initialization)
  CAppIosFlagGuard lock(cout);
  cout << hex << uppercase;
  cout << "Found marker: 0xFF";
  cout << static_cast<int>(markerCode) << " ";
  cout << "(" << markerName << ") ";
  cout << "at position 0x" << pos;

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

      // APPn (Print)
      if (markerCode >= APP0 && markerCode <= APP15) {
        PrintAPPMarker(markerCode);
      }

      // Call Marker Parase Process
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

  for (int i = 0; i < DQT_MAX_TABLE_COUNT; i++) {
    m_dqt[i].clear();
  }

  m_count_dqt_table = 0;

  for (int tc = 0; tc < DHT_MAX_TC_COUNT; tc++) {
    for (int th = 0; th < DHT_MAX_TH_COUNT; th++) {
      m_vt_dht[tc][th].clear();
    }
  }

  return (RET_OK);
}

int CAppImageJpeg::IncrPtrToMarker(byte *pData)
{
  return (RET_OK);
}

///
/// Parameters
///
/// *ptr      : A pointer to the head position of a buffer
/// nByte     : Total size in bytes to be displayed
/// nOffset   : An offset at which the display starts
/// os        : An output stream to which the dump result is written
///
int CAppImageJpeg::DumpPtrFile(const uint8_t *ptr, const uint64_t nByte,
                               const uint64_t nOffset, std::ostream &os)
{
  for (uint64_t i = 0; i < nByte; i++) {

    os << hex << uppercase << setfill('0') << setw(2)
       << static_cast<int>(ptr[nOffset + i]);

    if (i % 16 == 15) {
      os << endl;
    }
    else {
      os << " ";
    }
  }

  if (nByte % 16 != 0) {
    os << endl;
  }

  os << dec << nouppercase << setfill(' ');

  return (RET_OK);
}

///
/// Parameters
///
/// ofs       : An ofstream to which a stringstream is written
/// ss        : A stringstream that will be written to ofs
///
int CAppImageJpeg::WriteOutputStream(std::ofstream &ofs,
                                     const std::stringstream &ss) const
{
  if (ofs.is_open() == false) {
    // The output stream is not opened
    return (RET_ERR);
  }

  // Write strings to output stream
  ofs << ss.str();

  return (RET_OK);
}

int CAppImageJpeg::WriteFileAPPn(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_app, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileCOM(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_com, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileDHT(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_dht, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileDRI(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_dri, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileDQT(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_dqt, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileSOS(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_sos, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileSOF(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_sof, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
}

int CAppImageJpeg::WriteFileRAW(const std::stringstream &ss)
{
  if (WriteOutputStream(m_ofs_raw, ss) == RET_OK) {
    return (RET_OK);
  }

  return (RET_ERR);
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
  // TODO: redesign
  stringstream ss;
  stringstream ss_ascii;

  // Set format flags (std::ios_base::fmtflags)
  ss << hex << uppercase << setfill('0');
  ss_ascii << setfill('.');

  for (uint16_t nPos = 0; nPos < nLength; nPos++) {

    ss << setw(2) << (int)pData[nPos] << " ";

    if (IsPrintable(pData[nPos])) {

      if (pData[nPos] == ' ') {
        ss_ascii << setw(2) << "" << " ";
      }
      else {
        ss_ascii << setw(2) << pData[nPos] << " ";
      }
    }
    else {
      ss_ascii << setw(2) << "" << " ";
    }

    if (nPos % DUMP_MAX_LINE_LENGTH == (DUMP_MAX_LINE_LENGTH - 1)) {
      ss << endl;
      ss_ascii << endl;
    }
  }

  if (nLength % DUMP_MAX_LINE_LENGTH != 0) {
    ss << endl << endl;
    ss_ascii << endl << endl;
  }

  if (CAppData::IsPrintAPP()) {
    WriteFileAPPn(ss_ascii);
    // WriteFileAPPn(ss);
  }

  return (RET_OK);
}

int CAppImageJpeg::PrintAPPMarker(const uint8_t marker)
{
  if (marker < APP0 || marker > APP15) {
    // TODO: test
    return (RET_ERR);
  }

  if (CAppData::IsPrintAPP() == false) {
    return (RET_ERR);
  }

  stringstream ss;
  ss << GetFileName() << ", ";

  uint8_t diff = marker - APP0;

  string str = "APP" + to_string(diff);
  ss << str << endl;

  WriteFileAPPn(ss);

  return (RET_OK);
}

int CAppImageJpeg::PrintAPP()
{
  return (RET_OK);
}

int CAppImageJpeg::PrintSOF()
{
  if (CAppData::IsPrintSOF() == false) {
    return (RET_ERR);
  }

  stringstream ss;

  ss << GetFileName() << ", SOF0" << endl;
  ss << hex << uppercase << setfill('0');
  ss << "sample rate: 0x" << (int)m_sof.sample_rate << endl;
  ss << "width:  0x" << hex << (int)m_sof.width;
  ss << " (" << dec << (int)m_sof.width << ") " << endl;
  ss << "height: 0x" << hex << (int)m_sof.height;
  ss << " (" << dec << (int)m_sof.height << ") " << endl;
  ss << "component number: " << (int)m_sof.num << endl;

  for (int i = 0; i < m_sof.v.size(); i++) {
    const _SOF_PART *part = &m_sof.v[i];
    ss << "------------" << endl;
    ss << "index: " << (int)part->index << endl;
    ss << "hn: " << (int)part->h_rate << endl;
    ss << "vn: " << (int)part->v_rate << endl;
    ss << "DQT no: " << (int)part->dqt_no << endl;
  }
  ss << endl;

  WriteFileSOF(ss);

  return (RET_OK);
}

int CAppImageJpeg::PrintSOS()
{
  if (m_sos.cs.size() != m_sos.ns) {
    return (RET_ERR);
  }

  stringstream ss;

  ss << GetFileName() << ", SOS, ";
  ss << "marker len : " << (int)m_sos.seg.length << endl;

  ss << "The number of components: " << (int)m_sos.ns << endl;

  for (int i = 0; i < m_sos.ns; i++) {
    ss << "-----------------------------------" << endl;
    ss << "cs: " << (int)m_sos.cs[i].id << endl;
    ss << "huffman table id (DC): " << (int)m_sos.cs[i].td << endl;
    ss << "huffman table id (AC): " << (int)m_sos.cs[i].ta << endl;
  }

  ss << "-----------------------------------" << endl;
  ss << "Start of spectral selection: " << (int)m_sos.ss << endl;
  ss << "End of spectral selection: " << (int)m_sos.se << endl;

  ss << "Approximation bit high: " << (int)m_sos.ah << endl;
  ss << "Approximation bit low: " << (int)m_sos.al << endl;
  ss << endl;

  WriteFileSOS(ss);

  return (RET_OK);
}

int CAppImageJpeg::ParseDHT(const uint8_t *pData, const uint16_t nLength)
{
  uint64_t pos = 0;

  MoveParseMarkerOffset(pos);

  stringstream ss;
  ss << GetFileName() << endl;

  while (pos < nLength) {
    _DHT dht;

    dht.tc = *(pData + pos) >> 4;
    dht.th = (*(pData + pos) & 0x0F);
    pos++;

    for (int i = 0; i < DHT_MAX_CODE_LENGTH && pos < nLength; i++) {
      dht.l[i] = *(pData + pos);
      pos++;
    }

    ss << "DHT L[16] = ";
    DumpPtrFile(dht.l, DHT_MAX_CODE_LENGTH, 0, ss);

    dht.v.clear();

    for (int i = 0; (i < 16 && pos < nLength); i++) {
      for (int j = 0; (j < dht.l[i] && pos < nLength); j++) {
        dht.v.push_back(pData[pos]);
        pos++;
      }
    }

    MakeHuffmanTree(dht.tc, dht.th, dht.l, dht.v);
    PrintHuffmanTree(dht.tc, dht.th, ss);
  }

  ss << endl;

  WriteFileDHT(ss);

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
  _HUFFMAN_CODE hcode;
  uint16_t code = 0x0000;
  uint16_t pos = 0x0000;

  if (ValidateTcTh(tc, th) == RET_ERR) {
    return (RET_ERR);
  }

  _HUFFMAN_TABLE *pTable = &m_vt_dht[tc][th];

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < l[i]; j++) {
      hcode.length = i + 1;
      hcode.code = code++;
      hcode.value = v[pos++];
      pTable->push_back(hcode);
    }
    code <<= 1;
  }

  return (RET_OK);
}

int CAppImageJpeg::PrintHuffmanTree(const uint8_t tc, const uint8_t th,
                                    std::ostream &os) const
{
  // RAII (Resource Acquisition Is Initialization)
  CAppIosFlagGuard lock(os);

  // Check whether to print
  if (CAppData::IsPrintDHT() == false) {
    return (RET_ERR);
  }

  // Validate parameters
  if (ValidateTcTh(tc, th) == RET_ERR) {
    return (RET_ERR);
  }

  // Specify a DHT buffer
  const _HUFFMAN_TABLE *pTable = &m_vt_dht[tc][th];

  // Output tc (Table Class)
  switch (tc) {
    case DHT_TC_VALUE_FOR_DC: {
      os << "table class: " << DHT_TC_CODE_FOR_DC << ", ";
    } break;

    case DHT_TC_VALUE_FOR_AC: {
      os << "table class: " << DHT_TC_CODE_FOR_AC << ", ";
    } break;

    default:
      return (RET_ERR);
  }

  // Output th (table identifier)
  os << "table identifier: " << (int)th << endl;

  // Output DHT table header
  if (tc == DHT_TC_VALUE_FOR_DC) {
    os << "value (decimal) : code (binary)" << endl;
  }
  else if (tc == DHT_TC_VALUE_FOR_AC) {
    os << "value (RRRR/SSSS) : code (binary)" << endl;
  }

  for (int i = 0; i < pTable->size(); i++) {

    // copy
    _HUFFMAN_CODE hcode = pTable->at(i);

    char format[64] = {0x00};
    snprintf(format, 64, "{:0%db}", hcode.length);

    if (tc == DHT_TC_VALUE_FOR_DC) {
      // DC
      os << dec << (int)hcode.value;
    }
    else if (tc == DHT_TC_VALUE_FOR_AC) {
      // AC
      uint8_t run = (hcode.value) >> 4;
      uint8_t size = hcode.value & 0x0F;

      if (hcode.value == 0x00) {
        // End Of Block
        os << "EOB";
      }
      else if (hcode.value == 0xF0) {
        // Zero Run Length
        os << "ZRL";
      }
      else {
        os << dec << (int)run << "/" << (int)size;
      }

      os << hex << uppercase << setfill('0');
      os << " (0x" << setw(2) << (int)hcode.value << ") ";
    }

    // Ouput code in binary with specified length
    os << " : ";
    os << std::vformat(format, std::make_format_args(hcode.code));
    os << endl;
  }

  return (RET_OK);
}

int CAppImageJpeg::PrintQuantizationTable(const uint8_t q)
{
  stringstream ss;

  if (q < 0 || q >= DQT_MAX_TABLE_COUNT) {
    // TODO: test
    return (RET_ERR);
  }

  const _QUANTIZATION_TABLE *pTable = &m_dqt[q];

  if (pTable->qk.size() != DQT_QK_MAX_COUNT) {
    // TODO: test
    return (RET_ERR);
  }

  ss << GetFileName() << ", ";
  ss << "Pq: " << (int)pTable->pq << ", ";
  ss << "Tq: " << (int)pTable->tq << endl;
  ss << dec << setfill(' ') << nouppercase;

  if (pTable->pq == DQT_PQ_08_BITS) {
    for (int i = 0; i < DQT_QK_MAX_COUNT; i++) {
      ss << setw(3) << (int)pTable->qk[zigzag_order[i]];
      if (i % MCU_COUNT == (MCU_COUNT - 1)) {
        ss << endl;
      }
      else {
        ss << " ";
      }
    }
  }
  else if (pTable->pq == DQT_PQ_16_BITS) {
    for (int i = 0; i < DQT_QK_MAX_COUNT; i++) {
      ss << setw(5) << (int)pTable->qk[zigzag_order[i]] << " ";
      if (i % MCU_COUNT == (MCU_COUNT - 1)) {
        ss << endl;
      }
      else {
        ss << " ";
      }
    }
  }

  ss << endl;
  WriteFileDQT(ss);

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

    m_sof.v.push_back(part);
  }

  if (CAppData::IsPrintSOF()) {
    PrintSOF();
  }

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
  uint64_t pos = 0x00;

  MoveParseMarkerOffset(pos);

  // One DQT segment may contain multiple quantization tables.
  while (pos < nLength) {

    if (m_count_dqt_table >= DQT_MAX_TABLE_COUNT) {
      // TODO: test & design
      return (RET_ERR);
    }

    _QUANTIZATION_TABLE *pDqt = &m_dqt[m_count_dqt_table];

    pDqt->pq = (pData[pos] >> 4);
    pDqt->tq = (pData[pos] & 0x0F);
    pDqt->qk.assign(64, 0);

    pos += 1;

    if (pDqt->pq == DQT_PQ_08_BITS) {
      for (int i = 0; i < DQT_QK_MAX_COUNT; i++) {
        pDqt->qk[i] = pData[pos + i];
      }
      pos += DQT_QK_MAX_COUNT;
    }
    else if (pDqt->pq == DQT_PQ_16_BITS) {
      for (int i = 0; i < DQT_QK_MAX_COUNT; i++) {
        pDqt->qk[i] = (pData[pos + i + 1] << 8) + pData[pos + i];
        i++;
      }
      pos += DQT_QK_MAX_COUNT * 2;
    }

    PrintQuantizationTable(m_count_dqt_table);
    m_count_dqt_table++;
  }
  return (RET_OK);
}

int CAppImageJpeg::ParseSOS(const uint8_t *pData, const uint16_t nLength)
{
  if (nLength % 2 != 0) {
    // TODO: test
    return (RET_ERR);
  }

  m_sos.clear();
  m_sos.seg.marker = SOS;
  m_sos.seg.length = (pData[0] << 8) | pData[1];
  m_sos.ns = pData[2];

  if (nLength != (m_sos.ns * 2 + 6)) {
    // TODO: test
    return (RET_ERR);
  }

  for (int i = 0; i < m_sos.ns; i++) {
    _SOS_PART part;

    part.id = pData[2 * i + 3];
    part.td = pData[2 * i + 4] >> 4;
    part.ta = pData[2 * i + 4] & 0x0F;
    m_sos.cs.push_back(part);
  }

  m_sos.ss = pData[m_sos.ns * 2 + 3];
  m_sos.se = pData[m_sos.ns * 2 + 4];
  m_sos.ah = pData[m_sos.ns * 2 + 5] >> 4;
  m_sos.al = pData[m_sos.ns * 2 + 5] & 0x0F;

  if (CAppData::IsPrintSOS()) {
    PrintSOS();
  }

  return (RET_OK);
}

int CAppImageJpeg::ParseCOM(const uint8_t *pData, const uint16_t nLength)
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
