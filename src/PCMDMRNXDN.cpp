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

#include "PCMDMRNXDN.h"

#include "AMBE3003Driver.h"
#include "ModeDefines.h"
#include "Debug.h"

CPCMDMRNXDN::CPCMDMRNXDN() :
m_n(0U)
{
}

CPCMDMRNXDN::~CPCMDMRNXDN()
{
}

uint8_t CPCMDMRNXDN::init(uint8_t n)
{
  m_n = n;

  ambe.init(m_n, PCM_TO_DMR_NXDN);

  return 0x00U;
}

uint8_t CPCMDMRNXDN::input(const uint8_t* buffer, uint16_t length)
{
  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  return ambe.writePCM(m_n, buffer);
}

int16_t CPCMDMRNXDN::output(uint8_t* buffer)
{
  AD_STATE ret = ambe.readAMBE(m_n, buffer);
  switch (ret) {
      case ADS_NO_DATA:
        return 0;

      case ADS_DATA:
        return DMR_NXDN_DATA_LENGTH;

      default:
        DEBUG1("PCMDMRNXDN: Invalid returned data type");
        return -0x06;
  }
}
