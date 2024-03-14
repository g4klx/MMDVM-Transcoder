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

#include "YSFDNDStar.h"

#include "ModeDefines.h"
#include "Debug.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t INTERLEAVE[] = {0U,  3U,   6U,  9U, 12U, 15U, 18U, 21U, 24U, 27U, 30U, 33U,           // u0
                              36U, 39U, 41U, 43U, 45U, 47U,  1U,  4U,  7U, 10U, 13U, 16U,           // u1
                              19U, 22U, 25U, 28U, 31U, 34U, 37U, 40U, 42U, 44U, 46U,                // u2
                              48U,  2U,  5U,  8U, 11U, 14U, 17U, 20U, 23U, 26U, 29U, 32U, 35U, 3U}; // u3

CYSFDNDStar::CYSFDNDStar() :
m_n(0U),
m_state(YDNDS_NONE),
m_utils(),
m_buffer1(),
m_buffer2(),
m_len1(0U),
m_len2(0U)
{
}

CYSFDNDStar::~CYSFDNDStar()
{
}

uint8_t CYSFDNDStar::init(uint8_t n)
{
  m_n = n;

  // Create mode change frames for later
  m_len1 = m_utils.createModeChange(m_n, YSFDN_TO_PCM, m_buffer1);  
  m_len2 = m_utils.createModeChange(m_n, PCM_TO_DSTAR, m_buffer2);  

  ambe3000.init(n, YSFDN_TO_PCM);

  return 0x00U;
}

uint8_t CYSFDNDStar::input(const uint8_t* buffer, uint16_t length)
{
  if (length != YSFDN_DATA_LENGTH) {
    DEBUG2("YSF DN frame length is invalid", length);
    return 0x04U;
  }

  if (m_state != YDNDS_NONE) {
    DEBUG1("The AMBE3000 is busy");
    return 0x04U;
  }

  uint8_t ambe[7U];
  ::memset(ambe, 0x00U, 7U);

  uint8_t n = 0U;

  // u0 + u1
  for (uint8_t i = 0U; i < 81U; i += 3, n++) {
    uint8_t vote = 0U;
    vote += READ_BIT1(buffer, i + 0U) ? 1U : 0U;
    vote += READ_BIT1(buffer, i + 1U) ? 1U : 0U;
    vote += READ_BIT1(buffer, i + 2U) ? 1U : 0U;

    uint8_t pos = INTERLEAVE[n];

    switch (vote) {
      case 0U:    // 0 0 0
      case 1U:    // 1 0 0, or 0 1 0, or 0 0 1, convert to 0
        WRITE_BIT1(ambe, pos, false);
        break;
      case 3U:    // 1 1 1
      case 2U:    // 1 1 0, or 0 1 1, or 1 0 1, convert to 1
        WRITE_BIT1(ambe, pos, true);
        break;
    }
  }

  // u2 + u3
  for (uint8_t i = 81U; i < 103U; i++, n++) {
    bool b = READ_BIT1(buffer, i) != 0U;

    uint8_t pos = INTERLEAVE[n];

    WRITE_BIT1(ambe, pos, b);
  }

  m_state = YDNDS_STATE1;

  return ambe3000.writeAMBE(m_n, ambe);
}

void CYSFDNDStar::process()
{
  if (m_state != YDNDS_STATE1)
    return;

  uint8_t buffer[400U];
  AD_STATE ret = ambe3000.readPCM(m_n, buffer);

  switch (ret) {
    case ADS_NO_DATA:
      break;

    case ADS_DATA:
      // Receive PCM from YSF DN, send back to the chip and switch back to YSF DN to PCM
      ambe3000.writePCM(m_n, buffer, m_buffer1, m_len1);
      m_state = YDNDS_STATE2;
      break;

    default:
      DEBUG1("YSFDNDStar:1: Invalid returned data type");
      m_state = YDNDS_NONE;
      break;
  }
}

int16_t CYSFDNDStar::output(uint8_t* buffer)
{
  if (m_state != YDNDS_STATE2)
    return 0;

  AD_STATE ret = ambe3000.readAMBE(m_n, buffer);
  switch (ret) {
      case ADS_NO_DATA:
        return 0;

      case ADS_DATA:
        m_state = YDNDS_NONE;
        return DSTAR_DATA_LENGTH;

      default:
        DEBUG1("YSFDNDStar:2: Invalid returned data type");
        m_state = YDNDS_NONE;
        return -0x06;
  }
}

void CYSFDNDStar::finish()
{
  // Drain any outstanding replies from the chip
  ambe3000.drain(m_n);
}
