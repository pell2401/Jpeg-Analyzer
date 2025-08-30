#include "AppImageJpeg.h"
#include <map>
#include <bitset>



CAppImageJpeg::CAppImageJpeg() : CAppImage()
{
	m_vt_BitData.clear();
}

CAppImageJpeg::~CAppImageJpeg()
{
	m_vt_BitData.clear();
}

int CAppImageJpeg::ParseMarker()
{
	uint64_t pos = 0;

	while (pos < m_nFileSize - 1)
	{
		if (m_pData[pos] == 0xff)
		{
			while (pos + 1 < m_nFileSize && m_pData[pos + 1] == 0xff)
			{
				pos++;
			}

			if (pos + 1 >= m_nFileSize) break;

			uint8_t markerCode = m_pData[pos + 1];

			std::string markerName = m_MarkerMap.count(markerCode) ? m_MarkerMap[markerCode] : "Unknown";

			std::cout << "Found marker: 0xFF"
				<< std::hex << std::uppercase << static_cast<int>(markerCode)
				<< " (" << markerName << ") at position " << std::dec << pos << std::endl;

			// SOI/EOI/SOS are exceptions
			
			if (markerCode == 0xd8 || markerCode == 0xd9)
			{
				pos += 2;
			}
			else if (markerCode == 0xda)
			{
				pos += 2;
				if (pos + 1 >= m_nFileSize) break;

				uint16_t len = (m_pData[pos] << 8) | m_pData[pos + 1];

				std::cout << " -> Length: " << len << std::endl;

				pos += len;

				m_vt_BitData.clear();

				while (pos + 1 < m_nFileSize)
				{
					if (m_pData[pos] == 0xff && m_pData[pos + 1] != 0x00)
					{
						break;
					}

					for (int n = 0; n < 8; n++)
					{
						uint8_t bit = (m_pData[pos] & (1 << n)) ? 1 : 0;
						m_vt_BitData.push_back(bit);
					}

					if (m_pData[pos] == 0xff && m_pData[pos + 1] == 0x00)
					{
						pos++;
					}

					pos++;
				}
			}
			else
			{
				if (pos + 3 >= m_nFileSize)
					break;

				uint16_t len = (m_pData[pos + 2] << 8) | m_pData[pos + 3];
				std::cout << " -> Length: " << len << std::endl;
				pos += (2 + len);
			}

		}
	}


	return (0);
}

