#pragma once

#include <iostream>
#include <vector>

#include "AppDataType.h"
#include "AppDefine.h"
#include "AppImageJpeg.h"
#include "AppStruct.h"

using namespace std;

class CAppData {
private:
  CAppData();
  virtual ~CAppData();
  static CAppData *m_pObject;

public:
  static CAppData *GetInstance();
};
