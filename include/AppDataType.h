#pragma once

#include <cstdint>
#include <vector>
#include "AppStruct.h"
#include "AppDefine.h"

typedef std::vector<_HUFFMAN_CODE> _HUFFMAN_TABLE;
typedef _HUFFMAN_TABLE vt_DHT[DHT_MAX_TC_COUNT][DHT_MAX_TH_COUNT];
typedef _QUANTIZATION_TABLE vt_DQT[DQT_MAX_TABLE_COUNT];
