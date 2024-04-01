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

#include "DStarYSFDN.h"

#include "ModeDefines.h"
#include "YSFDNUtils.h"
#include "Config.h"
#include "Debug.h"

CDStarYSFDN::CDStarYSFDN() :
m_n(0U),
m_state(DYDNS_NONE),
m_utils(),
m_buffer1(),
m_buffer2(),
m_len1(0U),
m_len2(0U)
{
}

CDStarYSFDN::~CDStarYSFDN()
{
}

uint8_t CDStarYSFDN::init(uint8_t n)
{
  m_n = n;

  // Create mode change frames for later
  m_len1 = m_utils.createModeChange(m_n, DSTAR_TO_PCM, m_buffer1);  
  m_len2 = m_utils.createModeChange(m_n, PCM_TO_YSFDN, m_buffer2);  

  ambe3000.init(n, DSTAR_TO_PCM);

  return 0x00U;
}

uint8_t CDStarYSFDN::input(const uint8_t* buffer, uint16_t length)
{
  if (length != DSTAR_DATA_LENGTH) {
    DEBUG2("D-Star frame length is invalid", length);
    return 0x04U;
  }

  if (m_state != DYDNS_NONE) {
    DEBUG1("The AMBE3000 is busy");
    return 0x04U;
  }

  m_state = DYDNS_STATE1;

  return ambe3000.writeAMBE(m_n, buffer, m_buffer2, m_len2);
}

uint8_t CDStarYSFDN::process()
{
  if (m_state != DYDNS_STATE1)
    return 0x00U;

  uint8_t buffer[400U];
  AD_STATE ret = ambe3000.readPCM(m_n, buffer);

  switch (ret) {
    case ADS_NO_DATA:
      return 0x00U;

    case ADS_DATA:
      // Receive PCM from D-Star, send back to the chip and switch back to D-Star to PCM
      ambe3000.writePCM(m_n, buffer, m_buffer1, m_len1);
      m_state = DYDNS_STATE2;
      return 0x00U;

    default:
      DEBUG1("DStarYSFDN:1: Invalid returned data type");
      m_state = DYDNS_NONE;
      return 0x06U;
  }
}

int16_t CDStarYSFDN::output(uint8_t* buffer)
{
  if (m_state != DYDNS_STATE2)
    return 0;

  uint8_t ambe[10U];
  AD_STATE ret = ambe3000.readAMBE(m_n, ambe);
  switch (ret) {
      case ADS_NO_DATA:
        return 0;

      case ADS_WRONG_TYPE:
        DEBUG1("DStarYSFDN:2: Invalid returned data type");
        m_state = DYDNS_NONE;
        return -0x06;

      default:
        CYSFDNUtils::fromMode34(ambe, buffer);
        m_state = DYDNS_NONE;
        return YSFDN_DATA_LENGTH;
  }
}

void CDStarYSFDN::finish()
{
  // Drain any outstanding replies from the chip
  ambe3000.drain(m_n);
}
