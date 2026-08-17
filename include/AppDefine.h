#pragma once

#pragma region SOMETHING

#define BYTE_MARKER_PREFIX 0xFF
#define BYTE_STUFFING 0x00
#define SUPPORTED_MARKER_NUMBER 40
#define MAX_DHT_TC_COUNT 2
#define MAX_DHT_TH_COUNT 4

#pragma endregion

#pragma region RETURN VALUES

#define RET_OK 0
#define RET_ERR -1

#pragma endregion

#pragma region MARKER

#define SOI 0xD8
#define EOI 0xD9

#define DHT 0xC4
#define SOF0 0xC0
#define SOF2 0xC2
#define SOF3 0xC3
#define DQT 0xDB
#define DRI 0xDD
#define SOS 0xDA

#define RST0 0xD0
#define RST1 0xD1
#define RST2 0xD2
#define RST3 0xD3
#define RST4 0xD4
#define RST5 0xD5
#define RST6 0xD6
#define RST7 0xD7

#define MARKER_RST_MIN RST0
#define MARKER_RST_MAX RST7

#define APP0 0xE0
#define APP1 0xE1
#define APP2 0xE2
#define APP3 0xE3
#define APP4 0xE4
#define APP5 0xE5
#define APP6 0xE6
#define APP7 0xE7
#define APP8 0xE8
#define APP9 0xE9
#define APP10 0xEA
#define APP11 0xEB
#define APP12 0xEC
#define APP13 0xED
#define APP14 0xEE
#define APP15 0xEF

#pragma endregion
