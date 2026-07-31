#pragma once

#include <cstdint>

class CAppImageJpeg;

typedef int (CAppImageJpeg::*pFuncParseMarker)(const uint8_t *, const uint16_t);
