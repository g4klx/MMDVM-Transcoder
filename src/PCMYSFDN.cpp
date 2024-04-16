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

#if AMBE_TYPE > 0

#include "ModeDefines.h"
#include "YSFDNUtils.h"
#include "Debug.h"

CPCMYSFDN::CPCMYSFDN() :
m_ambe(nullptr)
{
}

CPCMYSFDN::~CPCMYSFDN()
{
}

uint8_t CPCMYSFDN::init(uint8_t n)
{
#if AMBE_TYPE > 1
  switch (n) {
    case 0U:
      m_ambe = &ambe30001;
      break;
    case 1U:
      m_ambe = &ambe30002;
      break;
    default:
      return 0x04U;
  }
#else
  m_ambe = &ambe30001;
#endif

  m_ambe->init(PCM_TO_YSFDN);

  return 0x00U;
}

uint8_t CPCMYSFDN::input(const uint8_t* buffer, uint16_t length)
{
  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  return m_ambe->writePCM(buffer);
}

int16_t CPCMYSFDN::output(uint8_t* buffer)
{
  uint8_t ambe[10U];
  AD_STATE ret = m_ambe->readAMBE(ambe);
  switch (ret) {
      case ADS_NO_DATA:
        return 0;

      case ADS_WRONG_TYPE:
        DEBUG1("PCMYSFDN: Invalid returned data type");
        return -0x06;

      default:
        CYSFDNUtils::fromMode34(ambe, buffer);
        return YSFDN_DATA_LENGTH;
  }
}

#endif
