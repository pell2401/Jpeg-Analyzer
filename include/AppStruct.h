#pragma once

#include <iostream>
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
} _DHT;

typedef struct tagDhtSegment {
  _MARKER_SEGMENT seg;
  std::vector<_DHT> dht;
} _DHT_SEGMENT;

typedef struct tagHuffmanTable {
  uint8_t length; // number of bits (1-16)
  uint16_t code;  // huffman code (b10, b110, etc)
  uint8_t value;  // DC: group number (0-11)
} _HUFFMAN_TABLE;

typedef struct tagDqt {
  uint8_t pq; // 0: 8 bit, 1: 16 bit
  uint8_t tq; // id, range: 0-3
  std::vector<uint16_t>
      qk; // 64 elements (file size: 64 bytes if pq=0, 128 bytes if pq=1)
} _DQT;

typedef struct tagDqtSegment {
  _MARKER_SEGMENT seg;
  std::vector<_DQT> dqt;
} _DQT_SEGMENT;

typedef struct tagStartOfFramePart {
  uint8_t index;  // component id
  uint8_t h_rate; // horizontal rate (4-bit filed)
  uint8_t v_rate; // vertical rate (4-bit field)
  uint8_t dqt_no; // DQT table id
} _SOF_PART;

typedef struct tagStartOfFrame {
  _MARKER_SEGMENT seg;
  uint8_t sample_rate; // 8 (1 sample = {0,...,255} 2^8 = 256)
  uint16_t height;     // image height [pixel]
  uint16_t width;      // image width [pixel]
  uint8_t num;         // component number
  std::vector<_SOF_PART> vt_element;

  void clear()
  {
    seg.clear();
    sample_rate = 0x00;
    height = 0x0000;
    width = 0x0000;
    num = 0x00;
    vt_element.clear();
  }
} _SOF;

typedef struct tagStartOfScanPart {
  uint8_t id;
  uint8_t index;
} _SOS_PART;

typedef struct tagStartOfScan {
  uint8_t marker[2];
  uint8_t size[2];
  uint8_t num;
  std::vector<_SOS_PART> vt_element;
  uint8_t mode;
  uint8_t mode2;
  uint8_t mode3;
} _SOS;

typedef struct tagMarkerHandler {
  uint8_t marker;
  pFuncParseMarker pFunc;
} _MARKER_HANDLER;
