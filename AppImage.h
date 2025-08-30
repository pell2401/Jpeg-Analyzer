#pragma once

#include <iostream>
#include <string>

class CAppImage
{
public:
	CAppImage();
	virtual ~CAppImage();

	int Clear();
	int ReadFile(std::string fileName);
	int Dump();

protected:
	uint8_t* m_pData;
	std::string m_FileName;
	uint64_t m_nFileSize;


};

