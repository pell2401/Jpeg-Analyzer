#pragma once

#include <iostream>
#include <vector>
#include "AppFuncType.h"

typedef struct tagMarkerSegment {
  uint16_t marker;
  uint16_t length;
} _MARKER_SEGMENT;

typedef struct tagDht {
  uint8_t tc;                // 0: DC, 1: AC (4bit field in a file)
  uint8_t th;                // id, range: 0-3 (4bit field in a file)
  uint8_t l[16];             // bit number per byte
  std::vector<std::vector<uint8_t>> v; // table
} _DHT;

typedef struct tagDhtSegment {
  _MARKER_SEGMENT seg;
  std::vector<_DHT> dht;
} _DHT_SEGMENT;

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
  uint8_t index;
  uint8_t ratio;
  uint8_t dqt;
} _SOF_PART;

typedef struct tagStartOfFrame {
  uint8_t marker[2];
  uint8_t size[2];
  uint8_t rate;
  uint8_t height[2];
  uint8_t width[2];
  uint8_t num;
  std::vector<_SOF_PART> vt_element;
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
