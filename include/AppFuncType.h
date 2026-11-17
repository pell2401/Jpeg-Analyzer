#pragma once

#include <cstdint>
#include <sstream>

// Forward-declaration is required
class CAppImageJpeg;

typedef int (CAppImageJpeg::*pFuncParseMarker)(const uint8_t marker, const uint8_t *pData, const uint16_t nLength);
