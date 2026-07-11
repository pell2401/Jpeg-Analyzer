#include "AppImageJpeg.h"
#include <bitset>
#include <map>

CAppImageJpeg::CAppImageJpeg() : CAppImage() {
  m_vt_BitData.clear();
  m_pData = nullptr;
}

CAppImageJpeg::~CAppImageJpeg() {
  m_vt_BitData.clear();

  if (m_pData) {
    delete[] m_pData;
    m_pData = nullptr;
  }
}

int CAppImageJpeg::ParseMarker() {
  if (m_pData == nullptr) {
    return (-1);
  }

  uint64_t pos = 0;

  while (pos < m_nFileSize - 1) {
    if (m_pData[pos] == 0xff) {
      while (pos + 1 < m_nFileSize && m_pData[pos + 1] == 0xff) {
        pos++;
      }

      if (pos + 1 >= m_nFileSize)
        break;

      uint8_t markerCode = m_pData[pos + 1];
      string markerName = "";

      if (m_MarkerMap.contains(markerCode)) {
        markerName = m_MarkerMap[markerCode];
      } else {
        markerName = "Unknown";
      }

      cout << "Found marker: 0xFF" << hex << uppercase
           << static_cast<int>(markerCode) << " (" << markerName
           << ") at position " << dec << pos;

      if (markerCode == 0xD8 || markerCode == 0xD9) {
        // SOI|EOI (Start Of Image | End Of Image)
        pos += 2;

        switch (markerCode) {
        case 0xD8:
          cout << endl;
          break;

        case 0xD9:
          cout << endl << endl;
          break;
        }
      } else {
        pos += 2;

        if (pos + 1 >= m_nFileSize) {
          break;
        }

        uint16_t len = (m_pData[pos] << 8) | m_pData[pos + 1];
        cout << ", Length: " << len << endl;
        pos += len;

        if (markerCode == 0xDA) {
          // SOS (Start Of Scan)

          m_vt_BitData.clear();

          while (pos + 1 < m_nFileSize) {
            if (m_pData[pos] == 0xff && m_pData[pos + 1] != 0x00) {
              break;
            }

            for (int n = 0; n < 8; n++) {
              uint8_t bit = (m_pData[pos] & (1 << n)) ? 1 : 0;
              m_vt_BitData.push_back(bit);
            }

            if (m_pData[pos] == 0xff && m_pData[pos + 1] == 0x00) {
              pos++;
            }

            pos++;
          }
        } else {
          // TBD
        }
      }
    } else {
      cerr << "Unexpected Format! Data Fragment Happened." << endl << endl;
      return (-1);
    }
  }

  return (0);
}
