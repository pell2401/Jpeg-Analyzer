#pragma once

#include <iostream>
#include <string>

using namespace std;

class CAppImage {
public:
  CAppImage();
  virtual ~CAppImage();

  int Clear();
  int ReadFile(string fileName);
  int Dump();

protected:
  uint8_t *m_pData;
  string m_FileName;
  uint64_t m_nFileSize;
};
