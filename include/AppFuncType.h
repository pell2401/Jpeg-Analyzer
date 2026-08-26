#pragma once

#include <cstdint>
#include <sstream>

// Forward-declaration is required
class CAppImageJpeg;

typedef int (CAppImageJpeg::*pFuncParseMarker)(const uint8_t *, const uint16_t);
typedef int (CAppImageJpeg::*pFuncDumpPtrFile)(const std::stringstream&);
