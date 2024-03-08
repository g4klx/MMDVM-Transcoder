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

#include "DMRNXDNDStar.h"

#include "ModeDefines.h"
#include "Debug.h"

CDMRNXDNDStar::CDMRNXDNDStar() :
m_n(0U),
m_state(DNDS_NONE),
m_utils(),
m_buffer1(),
m_buffer2(),
m_len1(0U),
m_len2(0U)
{
}

CDMRNXDNDStar::~CDMRNXDNDStar()
{
}

void CDMRNXDNDStar::init(uint8_t n)
{
  m_n = n;

  // Create mode change frames for later
  m_len1 = m_utils.createModeChange(m_n, DMR_NXDN_TO_PCM, m_buffer1);  
  m_len2 = m_utils.createModeChange(m_n, PCM_TO_DSTAR, m_buffer2);  

  ambe3000.init(n, DMR_NXDN_TO_PCM);
}

uint8_t CDMRNXDNDStar::input(const uint8_t* buffer, uint16_t length)
{
  if (length != DMR_NXDN_DATA_LENGTH) {
    DEBUG2("DMR/NXDN frame length is invalid", length);
    return 0x04U;
  }

  if (m_state != DNDS_NONE) {
    DEBUG1("The AMBE3000 is busy");
    return 0x04U;
  }

  // Send DMR/NXDN AMBE with a mode change to PCM to D-Star following
  uint8_t frame[50U];
  uint16_t len = m_utils.createAMBEFrame(m_n, buffer, frame);

  m_state = DNDS_STATE1;

  return ambe3000.write(frame, len, m_buffer2, m_len2);
}

void CDMRNXDNDStar::process()
{
  if (m_state == DNDS_STATE1) {
    uint8_t buffer[400U];
    bool ret = ambe3000.read(m_n, buffer);
    if (!ret)
      return;

    // Receive PCM from DMR/NXDN, send back to the chip and switch back to DMR/NXDN to PCM
    uint8_t frame[400U];
    uint16_t length = m_utils.createPCMFrame(m_n, buffer, frame);

    ambe3000.write(frame, length, m_buffer1, m_len1);

    m_state = DNDS_STATE2;
  }
}

uint16_t CDMRNXDNDStar::output(uint8_t* buffer)
{
  if (m_state != DNDS_STATE2)
    return 0U;

  bool ret = ambe3000.read(m_n, buffer);
  if (!ret)
    return 0U;

  m_state = DNDS_NONE;

  return DSTAR_DATA_LENGTH;
}
