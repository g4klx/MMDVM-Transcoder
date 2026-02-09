/*
 *   Copyright (C) 2023,2024,2026 by Jonathan Naylor G4KLX
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

#include "DStarPCM.h"

#if AMBE_TYPE > 0

#include "AMBE3003Driver.h"
#include "ModeDefines.h"
#include "Debug.h"

CDStarPCM::CDStarPCM() :
m_n(0U)
{
}

CDStarPCM::~CDStarPCM()
{
}

uint8_t CDStarPCM::init(uint8_t n)
{
  m_n = n;

  ambe.init(m_n, AMBE_MODE::DSTAR_TO_PCM);

  return 0x00U;
}

uint8_t CDStarPCM::input(const uint8_t* buffer, uint16_t length)
{
  if (length != DSTAR_DATA_LENGTH) {
    DEBUG2("D-Star frame length is invalid", length);
    return 0x04U;
  }

  return ambe.writeAMBE(m_n, buffer);
}

int16_t CDStarPCM::output(uint8_t* buffer)
{
  AD_STATE ret = ambe.readPCM(m_n, buffer);
  switch (ret) {
      case AD_STATE::NO_DATA:
        return 0;

      case AD_STATE::DATA:
        return PCM_DATA_LENGTH;

      default:
        DEBUG1("DStarPCM: Invalid returned data type");
        return -0x06;
  }
}

#endif
