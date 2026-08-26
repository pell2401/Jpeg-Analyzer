#pragma once

#include "AppDefine.h"
#include <string>
#include <map>

using IniData = std::map<std::string, std::map<std::string, std::string>>;

class CAppData {
private:
  CAppData();
  virtual ~CAppData();

private:
  static CAppData *GetInstance();
  int ReadInifile(const std::string &path);
  int WriteInifile(const std::string &path);

private:
  IniData m_IniData;

private:
  static bool IsEnable(const std::string &section, const std::string &key);

public:
  static bool IsPrintAPP();
  static bool IsPrintCOM();
  static bool IsPrintDQT();
  static bool IsPrintSOF();
  static bool IsPrintDHT();
  static bool IsPrintDRI();
  static bool IsPrintSOS();
  static bool IsPrintRAW();
};
