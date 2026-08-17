#pragma once

#include <iostream>
#include <string>

using namespace std;

class CAppImage {
public:
  CAppImage();
  virtual ~CAppImage();

public:
  int ReadFile(const string &fileName);

public:
  int DumpFile() const;

  int DumpPtr(const uint8_t *ptr, const uint64_t nByte,
              const uint64_t nStartPos = 0) const;

public:
  const uint64_t GetFileSize() const;
  const string GetFileName() const;
  const uint8_t GetData(uint64_t pos) const;
  const bool IsDataNull() const;
  const bool IsIndexValid(uint64_t pos) const;

protected:
  virtual int ClearMemoryEx() = 0;

private:
  int ClearMemory();
  int SetFileSize(uint64_t fileSize);
  int SetFileName(string fileName);

protected:
  unique_ptr<uint8_t[]> m_pData;

private:
  string m_FileName;
  uint64_t m_nFileSize;
};
