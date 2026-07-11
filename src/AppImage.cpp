#include <iostream>
#include <iomanip>
#include <fstream>
#include "AppImage.h"

CAppImage::CAppImage() {
  m_pData = nullptr;
  Clear();
}

CAppImage::~CAppImage() {
  if (m_pData != nullptr) {
    delete[] m_pData;
    m_pData = nullptr;
  }
}

int CAppImage::Clear() {
  if (m_pData != nullptr) {
    delete[] m_pData;
    m_pData = nullptr;
  }

  m_nFileSize = 0x00;
  m_FileName = "";

  return (0);
}

int CAppImage::ReadFile(string fileName) {
  fstream file;

  file.open(fileName, ios_base::in | ios_base::binary);

  if (!file.is_open()) {
    cerr << "File open failed: " << fileName << endl;
    return (-1);
  }

  cout << "File open succeeded: " << fileName << endl;

  m_FileName = fileName;

  file.seekg(0, ios::end);
  m_nFileSize = file.tellg();
  file.seekg(0, ios::beg);

  m_pData = new uint8_t[m_nFileSize];
  file.read((char *)m_pData, m_nFileSize);

  return (0);
}

int CAppImage::Dump() {
  const uint64_t nCol = 8;
  uint64_t nRow = m_nFileSize / nCol;

  for (uint64_t pos = 0; pos < m_nFileSize; pos++) {
    cout << setw(2) << setfill('0') << hex << (int)m_pData[pos];

    if (pos % 16 == 15) {
      cout << endl;
    } else {
      cout << " ";
    }
  }

  if (m_nFileSize % 16 != 0) {
    cout << endl;
  }

  return (0);
}
