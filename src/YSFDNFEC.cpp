/*
 *   Copyright (C) 2010,2014,2016,2018,2023,2024 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "YSFDNFEC.h"

#include "YSFDNUtils.h"
#include "Debug.h"

CYSFDNFEC::CYSFDNFEC() :
m_buffer(),
m_inUse(false)
{
}

CYSFDNFEC::~CYSFDNFEC()
{
}

uint8_t CYSFDNFEC::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("YSF DN frame is being overwritten");
    return 0x05U;
  }

  if (length != YSFDN_DATA_LENGTH) {
    DEBUG2("YSF DN frame length is invalid", length);
    return 0x04U;
  }

  uint8_t ambe[10U];
  CYSFDNUtils::toMode34(buffer, ambe);
  CYSFDNUtils::fromMode34(ambe, m_buffer);

  m_inUse = true;

  return 0x00U;
}

int16_t CYSFDNFEC::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, YSFDN_DATA_LENGTH);
  m_inUse = false;

  return YSFDN_DATA_LENGTH;
}
