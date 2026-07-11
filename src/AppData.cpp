#include "AppData.h"

CAppData *CAppData::m_pObject = nullptr;

CAppData::CAppData() {
}

CAppData::~CAppData() {
}

CAppData *CAppData::GetInstance() {
  if (m_pObject == nullptr) {
    m_pObject = new CAppData();
  }

  return (m_pObject);
}
