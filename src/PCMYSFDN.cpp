/*
 *   Copyright (C) 2024 by Jonathan Naylor G4KLX
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

#include "PCMYSFDN.h"

#include "ModeDefines.h"
#include "Debug.h"

CPCMYSFDN::CPCMYSFDN() :
m_n(0U)
{
}

CPCMYSFDN::~CPCMYSFDN()
{
}

void CPCMYSFDN::init(uint8_t n)
{
  m_n = n;
  
  ambe3000.init(n, PCM_TO_YSFDN);
}

uint8_t CPCMYSFDN::input(const uint8_t* buffer, uint16_t length)
{
  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  return ambe3000.write(m_n, buffer, length);
}

uint16_t CPCMYSFDN::output(uint8_t* buffer)
{
  bool ret = ambe3000.read(m_n, buffer);
  if (!ret)
    return 0U;

  return YSFDN_DATA_LENGTH;
}

