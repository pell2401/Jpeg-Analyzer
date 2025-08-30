#include <iostream>
#include <iomanip>
#include <fstream>
#include "AppImage.h"


CAppImage::CAppImage()
{
	m_pData = nullptr;
	Clear();
}

CAppImage::~CAppImage()
{
	if (m_pData != nullptr)
	{
		delete[] m_pData;
		m_pData = nullptr;
	}
}

int CAppImage::Clear()
{
	if (m_pData != nullptr)
	{
		delete[] m_pData;
		m_pData = nullptr;
	}

	m_nFileSize = 0x00;
	m_FileName = "";

	return (0);
}

int CAppImage::ReadFile(std::string fileName)
{
	std::fstream file;

	file.open(fileName, std::ios_base::in | std::ios_base::binary);

	if (!file.is_open())
	{
		return (-1);
	}

	m_FileName = fileName;

	file.seekg(0, std::ios::end);
	m_nFileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	m_pData = new uint8_t[m_nFileSize];
	file.read((char*)m_pData, m_nFileSize);

	return (0);

}

int CAppImage::Dump()
{
	const uint64_t nCol = 8;
	uint64_t nRow = m_nFileSize / nCol;

	for (uint64_t pos = 0; pos < m_nFileSize; pos++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)m_pData[pos];

		if (pos % 16 == 15)
		{
			std::cout << std::endl;
		}
		else
		{
			std::cout << " ";
		}
	}

	if (m_nFileSize % 16 != 0)
	{
		std::cout << std::endl;
	}

	return (0);
}

