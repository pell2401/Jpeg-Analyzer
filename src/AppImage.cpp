#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "AppImage.h"
#include "AppDefine.h"
#include "AppIosFlagGuard.h"

CAppImage::CAppImage()
{
  m_nFileSize = 0x00;
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

int CAppImage::ReadFile(const string &filePath)
{
  fstream file;

  file.open(filePath, ios::in | ios::binary | ios::ate);

  if (!file.is_open()) {
    cerr << "File open failed: " << filePath << endl;
    return (RET_ERR);
  }

  streamoff size = file.tellg();

  if (size < 0) {
    cerr << "Failed to get file size: " << filePath << endl;
    return (RET_ERR);
  }
  else {
    file.seekg(0, ios::beg);
  }

  cout << "File open succeeded: " << filePath << endl;

  ClearMemory();

  SetFileName(filePath);
  SetFileSize((uint64_t)size);

  m_pData = make_unique<uint8_t[]>(GetFileSize());

  if (!file.read((char *)m_pData.get(), GetFileSize())) {

    cerr << "Failed to read file: " << filePath << endl;

    ClearMemory();

    return (RET_ERR);
  }

  return (RET_OK);
}

int CAppImage::DumpFile() const
{
  const uint64_t nCol = 8;
  uint64_t nRow = GetFileSize() / nCol;

  CAppIosFlagGuard lock(cout);
  lock.SetHexMode();

  // Set format flags (std::ios_base::fmtflags)
  cout << hex << uppercase << setfill('0');

  for (uint64_t pos = 0; pos < GetFileSize(); pos++) {
    cout << setw(2) << (int)m_pData[pos];

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
  CAppIosFlagGuard lock(cout);

  for (uint64_t i = 0; i < nByte; i++) {

    cout << hex << uppercase << setw(2) << setfill('0')
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

///
/// Return Values
///
/// true    : The data is null (not loaded)
/// false   : The data is not null (loaded)
///
const bool CAppImage::IsDataNull() const
{
  if (m_pData == nullptr) {
    return (true);
  }

  if (GetFileSize() <= 0) {
    return (true);
  }

  return (false);
}

///
/// Return Values
///
/// true    :   The position is valid in the data (m_pData[pos])
/// false   :   The position is out of valid range
///
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

///
/// Return Values
///
/// true    :   Printable
/// false   :   Not Printable
///
const bool CAppImage::IsPrintable(uint8_t c) const
{
  if (c < 0x20 || c >= 0x7F) {
    return (false);
  }

  return (true);
}
