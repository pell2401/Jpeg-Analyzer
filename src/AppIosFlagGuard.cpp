#include <iostream>
#include <iomanip>
#include "AppIosFlagGuard.h"

CAppIosFlagGuard::CAppIosFlagGuard(std::ostream &stream)
    : m_stream(stream), m_flags(stream.flags()), m_fill(stream.fill())
{
}

CAppIosFlagGuard::~CAppIosFlagGuard(void)
{
  m_stream.flags(m_flags);
  m_stream.fill(m_fill);
}


void CAppIosFlagGuard::SetHexMode(void)
{
  m_stream << std::hex << std::uppercase << std::setfill('0');
}
