#pragma once

#include <iostream>

class CAppIosFlagGuard {

public:
  explicit CAppIosFlagGuard(std::ostream &stream);
  virtual ~CAppIosFlagGuard(void);
  CAppIosFlagGuard(const CAppIosFlagGuard &) = delete;
  CAppIosFlagGuard &operator=(const CAppIosFlagGuard &) = delete;

  void SetHexMode(void);

private:
  std::ostream &m_stream;
  std::ios_base::fmtflags m_flags;
  char m_fill;
};
