#pragma once

#include <cstdint>
#include <vector>
#include "AppFuncType.h"

typedef struct tagMarkerSegment {
  uint16_t marker;
  uint16_t length;

  void clear()
  {
    marker = 0x0000;
    length = 0x0000;
  }
} _MARKER_SEGMENT;

typedef struct tagDht {
  uint8_t tc;             // 0: DC, 1: AC (4-bit field in a file)
  uint8_t th;             // id, range: 0-3 (4-bit field in a file)
  uint8_t l[16];          // bit number per byte
  std::vector<uint8_t> v; // table

  tagDht()
  {
    clear();
  }

  void clear()
  {
    tc = 0x00;
    th = 0x00;
    memset(l, 0x00, sizeof(l));
  }
} _DHT;

typedef struct tagHuffmanCode {
  uint8_t length; // number of bits (1-16)
  uint16_t code;  // huffman code (b10, b110, etc)
  uint8_t value;  // DC: group number (0-11)

  tagHuffmanCode()
  {
    clear();
  }

  void clear()
  {
    length = 0x00;
    code = 0x00;
    value = 0x00;
  }
} _HUFFMAN_CODE;

typedef struct tagQuantizationTable {
  uint8_t pq; // 0: 8 bit, 1: 16 bit
  uint8_t tq; // id, range: 0-3
  std::vector<uint16_t>
      qk; // 64 elements (file size: 64 bytes if pq=0, 128 bytes if pq=1)

  tagQuantizationTable()
  {
    clear();
  }

  void clear()
  {
    pq = 0;
    tq = 0;
    qk.clear();
  }
} _QUANTIZATION_TABLE;

typedef struct tagStartOfFramePart {
  uint8_t index;  // component id
  uint8_t h_rate; // horizontal rate (4-bit filed)
  uint8_t v_rate; // vertical rate (4-bit field)
  uint8_t dqt_no; // DQT table id
  tagStartOfFramePart()
  {
    clear();
  }

  void clear()
  {
    index = 0x00;
    h_rate = 0x00;
    v_rate = 0x00;
    dqt_no = 0x00;
  }
} _SOF_PART;

typedef struct tagStartOfFrame {
  _MARKER_SEGMENT seg;
  uint8_t sample_rate;      // 8 (1 sample = {0,...,255} 2^8 = 256)
  uint16_t height;          // image height [pixel]
  uint16_t width;           // image width [pixel]
  uint8_t num;              // the number of component
  std::vector<_SOF_PART> v; // components

  void clear()
  {
    seg.clear();
    sample_rate = 0x00;
    height = 0x0000;
    width = 0x0000;
    num = 0x00;
    v.clear();
  }
} _SOF;

typedef struct tagStartOfScanPart {
  uint8_t id; // 1 byte (component selector)
  uint8_t td; // upper nibble (DC table destination)
  uint8_t ta; // lower nibble (AC table destination)

  tagStartOfScanPart()
  {
    clear();
  }
  void clear()
  {
    id = 0x00;
    td = 0x00;
    ta = 0x00;
  }
} _SOS_PART;

typedef struct tagStartOfScan {
  _MARKER_SEGMENT seg;
  uint8_t ns;                // number of image components in scan
  std::vector<_SOS_PART> cs; // component-specification parameters
  uint8_t ss;                // 1 byte (start of spectral selection)
  uint8_t se;                // 1 byte (end of spectral selection)
  uint8_t ah; // upper nibble (successive approximation bit pos high)
  uint8_t al; // lower nibble (successive approximation bit pos low)

  tagStartOfScan()
  {
    clear();
  }

  void clear()
  {
    seg.clear();
    ns = 0x00;
    cs.clear();
    ss = 0x00;
    ss = 0x00;
    ah = 0x00;
    al = 0x00;
  }
} _SOS;

typedef struct tagMarkerHandler {
  uint8_t marker;
  pFuncParseMarker pFunc;
} _MARKER_HANDLER;
