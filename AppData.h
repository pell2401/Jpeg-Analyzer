#pragma once

#include <iostream>
#include <vector>

typedef struct tagDefineHuffmanTable
{
	uint8_t marker[2];
	uint8_t size[2];
	uint8_t type;
	uint8_t bitArray[16];
	std::vector<uint8_t> vt_bitData;
} _DHT;

typedef struct tagDefineQuantumTable
{
	uint8_t marker[2];
	uint8_t size[2];
	uint8_t type;
	std::vector<uint8_t> vt_data8;
	std::vector<uint16_t> vt_data16;
} _DQT;

typedef struct tagStartOfFramePart
{
	uint8_t index;
	uint8_t ratio;
	uint8_t dqt;
} _SOF_PART;


typedef struct tagStartOfFrame
{
	uint8_t marker[2];
	uint8_t size[2];
	uint8_t rate;
	uint8_t height[2];
	uint8_t width[2];
	uint8_t num;
	std::vector<_SOF_PART> vt_element;
} _SOF;


typedef struct tagStartOfScanPart
{
	uint8_t id;
	uint8_t index;
} _SOS_PART;

typedef struct tagStartOfScan
{
	uint8_t marker[2];
	uint8_t size[2];
	uint8_t num;
	std::vector<_SOS_PART> vt_element;
	uint8_t mode;
	uint8_t mode2;
	uint8_t mode3;
} _SOS;

class CAppData
{
private:
	CAppData();
	virtual ~CAppData();
	static CAppData* m_pObject;

public:




public:
	static CAppData* GetInstance();

};

