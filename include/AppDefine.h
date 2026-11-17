#pragma once

#pragma region SOMETHING
#define BYTE_MARKER_PREFIX 0xFF
#define BYTE_STUFFING 0x00
#define MCU_COUNT 0x08
#define DUMP_MAX_LINE_LENGTH 32
#define SUPPORTED_MARKER_SEGMENT_NUMBER 24
#define APP_FILENAME "APPn.txt"
#define COM_FILENAME "COM.txt"
#define DQT_FILENAME "DQT.txt"
#define SOF_FILENAME "SOFn.txt"
#define DHT_FILENAME "DHT.txt"
#define DRI_FILENAME "DRI.txt"
#define SOS_FILENAME "SOS.txt"
#define RAW_FILENAME "RAW.txt"
#define INIFILE_NAME "Setting.ini"
#pragma endregion

#pragma region Settings in AppData
#define PRINT_APP_FORMAT_HEX 0x01
#define PRINT_APP_FORMAT_ASCII 0x02
#define PRINT_APP_FORMAT_BOTH 0x04
#define PRINT_APP_FORMAT_HEXDUMP 0x08
#pragma endregion

#pragma region things in DHT
#define DHT_MAX_TC_COUNT 2
#define DHT_MAX_TH_COUNT 4
#define DHT_TC_CODE_FOR_DC "DC"
#define DHT_TC_CODE_FOR_AC "AC"
#define DHT_MAX_CODE_LENGTH 16
#define DHT_TC_VALUE_FOR_DC 0 // TC: (Table Class)
#define DHT_TC_VALUE_FOR_AC 1 // TC: (Table Class)
#pragma endregion

#pragma region things in DQT
#define DQT_PQ_08_BITS 0x00
#define DQT_PQ_16_BITS 0x01
#define DQT_MAX_TABLE_COUNT 0x04
#define DQT_QK_MAX_COUNT 64
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

#define COM 0xFE

#pragma endregion
