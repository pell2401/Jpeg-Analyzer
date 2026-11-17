#include <iostream>
#include <fstream>
#include <charconv>
#include "AppData.h"

using namespace std;

CAppData::CAppData()
{
  ReadInifile(INIFILE_NAME);
}

CAppData::~CAppData()
{
  WriteInifile(INIFILE_NAME);
}

static string Trim(const string &str)
{
  size_t start = str.find_first_not_of(" \t\r\n");
  size_t end = str.find_last_not_of(" \t\r\n");

  if (start == string::npos) {
    return ("");
  }

  return (str.substr(start, end - start + 1));
}

int CAppData::ReadInifile(const std::string &path)
{
  fstream file(path, ios::in);

  if (file.is_open() == false) {
    return (RET_ERR);
  }

  string line;
  string section;
  string key;
  string value;

  while (getline(file, line)) {
    line = Trim(line);

    // ';' and '#' are delimeters for comment out
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      continue;
    }

    // '[' and ']' are delimeters for a section
    if (line.front() == '[' && line.back() == ']') {
      section = line.substr(1, line.size() - 2);
      continue;
    }

    // '=' is a delimeter for a key and value pair
    size_t eq = line.find('=');
    if (eq == string::npos) {
      continue;
    }

    // Extract key and value
    key = Trim(line.substr(0, eq));
    value = Trim(line.substr(eq + 1));

    // Save it
    m_IniData[section][key] = value;
  }

  return (RET_OK);
}

int CAppData::WriteInifile(const std::string &path)
{
  fstream file(path, ios::out);

  if (file.is_open() == false) {
    return (RET_ERR);
  }

  for (const auto &[section, kv] : m_IniData) {
    file << "[" << section << "]" << endl;

    for (const auto &[key, value] : kv) {
      file << key << "=" << value << endl;
    }
  }

  return (RET_OK);
}

// Meyers' Singleton
CAppData *CAppData::GetInstance()
{
  static CAppData instance;
  return (&instance);
}

///
/// Return Values
///
/// true  : The key is enabled (not zero)
/// false : The key is not enabled (is zero)
///
bool CAppData::IsEnable(const std::string &section, const std::string &key)
{
  bool bRet = false;
  string str = GetInstance()->m_IniData[section][key];
  int value = 0;

  // No exceptions occur
  auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);

  if (ec == errc()) {
    // Convert Success
    if (value != 0x00) {
      bRet = true;
    }
  }
  else if (ec == errc::invalid_argument) {
    // Not Numerical
    bRet = false;
  }
  else if (ec == errc::result_out_of_range) {
    // The value is greater than max of type of int
    bRet = false;
  }

  return (bRet);
}

///
/// Return Values
///
/// RET_ERR(-1)   : Read integer failed
/// Others        : Read value
///
int CAppData::GetInt(const std::string &section, const std::string &key)
{
  int ret = -1;

  // TODO: Modify: If value does not exist, empty data is automatically added
  string str = GetInstance()->m_IniData[section][key];
  int value = 0x00;

  // No exceptions occur
  auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);

  if (ec == errc()) {
    // Convert Success
    ret = value;
  }
  else if (ec == errc::invalid_argument) {
    // Not Numerical
    ret = RET_ERR;
  }
  else if (ec == errc::result_out_of_range) {
    // The value is greater than max of type of int
    ret = RET_ERR;
  }

  return (ret);
}

///
/// Return Values
///
/// true  : Output APPn file
/// false : NOT Output APPn file
///
bool CAppData::IsPrintAPP()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_APP");
  return (bRet);
}

///
/// Return Values
///
/// RET_ERR(-1) : Not Defined
/// 1           : Hexadecimal (PRINT_APP_FORMAT_HEX)
/// 2           : ASCII (PRINT_APP_FORMAT_ASCII)
/// 4           : Both (PRINT_APP_FORMAT_BOTH)
/// 8           : Hexdump (PRINT_APP_FORMAT_HEXDUMP)
///
int CAppData::GetPrintAPPFormat()
{
  int nRet = GetInt("MARKER_REPORT", "PRINT_APP_FORMAT");

  if (nRet == RET_ERR ||
      (nRet != PRINT_APP_FORMAT_HEX && nRet != PRINT_APP_FORMAT_ASCII &&
        nRet != PRINT_APP_FORMAT_BOTH && nRet != PRINT_APP_FORMAT_HEXDUMP)) {
    nRet = RET_ERR;
  }

  return (nRet);
}

///
/// Return Values
///
/// true  : Output COM file
/// false : NOT Output COM file
///
bool CAppData::IsPrintCOM()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_COM");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output DQT file
/// false : NOT Output DQT file
///
bool CAppData::IsPrintDQT()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_DQT");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output SOF file
/// false : NOT Output SOF file
///
bool CAppData::IsPrintSOF()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_SOF");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output DHT file
/// false : NOT Output DHT file
///
bool CAppData::IsPrintDHT()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_DHT");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output DRI file
/// false : NOT Output DRI file
///
bool CAppData::IsPrintDRI()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_DRI");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output SOS file
/// false : NOT Output SOS file
///
bool CAppData::IsPrintSOS()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_SOS");
  return (bRet);
}

///
/// Return Values
///
/// true  : Output scanned data file
/// false : Not Output scanned data file
///
bool CAppData::IsPrintRAW()
{
  bool bRet = IsEnable("MARKER_REPORT", "PRINT_RAW");
  return (bRet);
}