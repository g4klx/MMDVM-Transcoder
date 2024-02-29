/*
 *   Copyright (C) 2023,2024 by Jonathan Naylor G4KLX
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

#include "ModeDefines.h"
#include "Config.h"
#include "Debug.h"

CDStarPCM::CDStarPCM() :
m_n(0U)
{
}

CDStarPCM::~CDStarPCM()
{
}

void CDStarPCM::init(uint8_t n)
{
  m_n = n;

  ambe3000.init(n, DSTAR_TO_PCM);
}

uint8_t CDStarPCM::input(const uint8_t* buffer, uint16_t length)
{
  if (length != DSTAR_DATA_LENGTH) {
    DEBUG2("D-Star frame length is invalid", length);
    return 0x04U;
  }

  return ambe3000.write(m_n, buffer, length);
}

uint16_t CDStarPCM::output(uint8_t* buffer)
{
  bool ret = ambe3000.read(m_n, buffer);
  if (!ret)
    return 0U;

  return PCM_DATA_LENGTH;
}
