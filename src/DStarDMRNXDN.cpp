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

#include "DStarDMRNXDN.h"

#include "ModeDefines.h"
#include "Debug.h"

CDStarDMRNXDN::CDStarDMRNXDN() :
m_n(0U),
m_state(DDNS_NONE),
m_utils(),
m_buffer1(),
m_buffer2(),
m_len1(0U),
m_len2(0U)
{
}

CDStarDMRNXDN::~CDStarDMRNXDN()
{
}

void CDStarDMRNXDN::init(uint8_t n)
{
  m_n = n;

  // Create mode change frames for later
  m_len1 = m_utils.createModeChange(m_n, DSTAR_TO_PCM, m_buffer1);  
  m_len2 = m_utils.createModeChange(m_n, PCM_TO_DMR_NXDN, m_buffer2);  

  ambe3000.init(n, DSTAR_TO_PCM);
}

uint8_t CDStarDMRNXDN::input(const uint8_t* buffer, uint16_t length)
{
  if (length != DSTAR_DATA_LENGTH) {
    DEBUG2("D-Star frame length is invalid", length);
    return 0x04U;
  }

  if (m_state != DDNS_NONE) {
    DEBUG1("The AMBE3000 is busy");
    return 0x04U;
  }

  m_state = DDNS_STATE1;

  return ambe3000.writeAMBE(m_n, buffer, m_buffer2, m_len2);
}

void CDStarDMRNXDN::process()
{
  if (m_state != DDNS_STATE1)
    return;

  uint8_t buffer[400U];
  AD_STATE ret = ambe3000.readPCM(m_n, buffer);

  switch (ret) {
    case ADS_NO_DATA:
      break;

    case ADS_DATA:
      // Receive PCM from D-Star, send back to the chip and switch back to D-Star to PCM
      ambe3000.writePCM(m_n, buffer, m_buffer1, m_len1);
      m_state = DDNS_STATE2;
      break;

    default:
      DEBUG1("DStarDMRNXDN:1: Invalid returned data type");
      m_state = DDNS_NONE;
      break;
  }
}

uint16_t CDStarDMRNXDN::output(uint8_t* buffer)
{
  if (m_state != DDNS_STATE2)
    return 0U;

  AD_STATE ret = ambe3000.readAMBE(m_n, buffer);
  switch (ret) {
      case ADS_NO_DATA:
        return 0U;

      case ADS_DATA:
        m_state = DDNS_NONE;
        return DMR_NXDN_DATA_LENGTH;

      default:
        DEBUG1("DStarDMRNXDN:2: Invalid returned data type");
        m_state = DDNS_NONE;
        return 0U;
  }
}

void CDStarDMRNXDN::finish()
{
  // Drain any outstanding replies from the chip
  ambe3000.drain(m_n);
}
