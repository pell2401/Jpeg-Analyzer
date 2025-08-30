#pragma once

#include "AppImage.h"
#include <string>
#include <map>
#include <vector>
#include <bitset>

class CAppImageJpeg : public CAppImage
{
public:
	CAppImageJpeg();
	virtual ~CAppImageJpeg();
	int ParseMarker();

private:
	std::vector<uint8_t> m_vt_BitData;

private:
	std::map<uint8_t, std::string> m_MarkerMap = {
	{0xD8, "SOI"},	{0xD9, "EOI"},	{0xDA, "SOS"},
	{0xC0, "SOF0"},	{0xC4, "DHT"},	{0xDB, "DQT"},
	{0xDD, "DRI"},
	{0xE0, "APP0"}, {0xE1, "APP1"}, {0xE2, "APP2"}, {0xE3, "APP3"},
	{0xE4, "APP4"}, {0xE5, "APP5"}, {0xE6, "APP6"}, {0xE7, "APP7"},
	{0xE8, "APP8"}, {0xE9, "APP9"}, {0xEA, "APP10"}, {0xEB, "APP11"},
	{0xEC, "APP12"}, {0xED, "APP13"}, {0xEE, "APP14"}, {0xEF, "APP15"}
	};

};

