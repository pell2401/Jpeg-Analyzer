#include <iostream>
#include <iomanip>
#include <fstream>

#include "AppImage.h"
#include "AppDefine.h"

CAppImage::CAppImage()
{
}

CAppImage::~CAppImage()
{
}

int CAppImage::ClearMemory()
{
  ClearMemoryEx();

  m_pData.reset();
  SetFileSize(0x00);
  SetFileName("");

  return (RET_OK);
}

int CAppImage::SetFileSize(uint64_t fileSize)
{
  m_nFileSize = fileSize;

  return (RET_OK);
}

int CAppImage::SetFileName(string fileName)
{
  m_FileName = fileName;

  return (RET_OK);
}

int CAppImage::ReadFile(const string &fileName)
{
  fstream file;

  file.open(fileName, ios::in | ios::binary | ios::ate);

  if (!file.is_open()) {
    cerr << "File open failed: " << fileName << endl;
    return (RET_ERR);
  }

  streamoff size = file.tellg();

  if (size < 0) {
    cerr << "Failed to get file size: " << fileName << endl;
    return (RET_ERR);
  }
  else {
    file.seekg(0, ios::beg);
  }

  cout << "File open succeeded: " << fileName << endl;

  ClearMemory();

  SetFileName(fileName);
  SetFileSize((uint64_t)size);

  m_pData = make_unique<uint8_t[]>(GetFileSize());

  if (!file.read((char *)m_pData.get(), GetFileSize())) {

    cerr << "Failed to read file: " << fileName << endl;

    ClearMemory();

    return (RET_ERR);
  }

  return (RET_OK);
}

int CAppImage::DumpFile() const
{
  const uint64_t nCol = 8;
  uint64_t nRow = GetFileSize() / nCol;

  for (uint64_t pos = 0; pos < GetFileSize(); pos++) {
    cout << setw(2) << setfill('0') << hex << (int)m_pData[pos];

    if (pos % 16 == 15) {
      cout << endl;
    }
    else {
      cout << " ";
    }
  }

  if (GetFileSize() % 16 != 0) {
    cout << endl;
  }

  return (RET_OK);
}

int CAppImage::DumpPtr(const uint8_t *ptr, const uint64_t nByte,
                       const uint64_t nStartPos) const
{
  for (uint64_t i = 0; i < nByte; i++) {
    cout << hex << setw(2) << setfill('0')
         << static_cast<int>(ptr[nStartPos + i]);

    if (i % 16 == 15) {
      cout << endl;
    }
    else {
      cout << " ";
    }
  }

  if (nByte % 16 != 0) {
    cout << endl;
  }

  cout << dec << setfill(' ');

  return (RET_OK);
}

const uint64_t CAppImage::GetFileSize() const
{
  return (m_nFileSize);
}

const string CAppImage::GetFileName() const
{
  return (m_FileName);
}

const uint8_t CAppImage::GetData(uint64_t pos) const
{
  if (IsDataNull()) {
    throw runtime_error("Data is null");
  }
  else if (!IsIndexValid(pos)) {
    throw out_of_range("Index out of range: " + to_string(pos));
  }

  return (m_pData[pos]);
}

const bool CAppImage::IsDataNull() const
{
  if (m_pData == nullptr) {
    return (true);
  }

  return (false);
}

const bool CAppImage::IsIndexValid(uint64_t pos) const
{
  if (IsDataNull()) {
    return (false);
  }
  else if (pos + 1 > GetFileSize()) {
    return (false);
  }

  return (true);
}
