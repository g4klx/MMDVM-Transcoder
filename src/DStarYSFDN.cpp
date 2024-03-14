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
#include "Config.h"
#include "Debug.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t INTERLEAVE[] = {0U,  3U,   6U,  9U, 12U, 15U, 18U, 21U, 24U, 27U, 30U, 33U,           // u0
                              36U, 39U, 41U, 43U, 45U, 47U,  1U,  4U,  7U, 10U, 13U, 16U,           // u1
                              19U, 22U, 25U, 28U, 31U, 34U, 37U, 40U, 42U, 44U, 46U,                // u2
                              48U,  2U,  5U,  8U, 11U, 14U, 17U, 20U, 23U, 26U, 29U, 32U, 35U, 3U}; // u3

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

void CDStarYSFDN::process()
{
  if (m_state != DYDNS_STATE1)
    return;

  uint8_t buffer[400U];
  AD_STATE ret = ambe3000.readPCM(m_n, buffer);

  switch (ret) {
    case ADS_NO_DATA:
      break;

    case ADS_DATA:
      // Receive PCM from D-Star, send back to the chip and switch back to D-Star to PCM
      ambe3000.writePCM(m_n, buffer, m_buffer1, m_len1);
      m_state = DYDNS_STATE2;
      break;

    default:
      DEBUG1("DStarYSFDN:1: Invalid returned data type");
      m_state = DYDNS_NONE;
      break;
  }
}

int16_t CDStarYSFDN::output(uint8_t* buffer)
{
  if (m_state != DYDNS_STATE2)
    return 0;

  uint8_t ambe[7U];
  AD_STATE ret = ambe3000.readAMBE(m_n, ambe);
  switch (ret) {
      case ADS_NO_DATA:
        return 0;

      case ADS_WRONG_TYPE:
        DEBUG1("DStarYSFDN:2: Invalid returned data type");
        m_state = DYDNS_NONE;
        return -0x06;

      default:
        break;
  }

  uint8_t n = 0U;

  // u0 + u1
  for (uint8_t i = 0U; i < 27U; i++) {
    uint8_t pos = INTERLEAVE[i];
    bool b = READ_BIT1(ambe, pos) != 0;

    WRITE_BIT1(buffer, n, b);
    n++;
    WRITE_BIT1(buffer, n, b);
    n++;
    WRITE_BIT1(buffer, n, b);
    n++;
  }

  // u2 + u3
  for (uint8_t i = 27U; i < 49U; i++) {
    uint8_t pos = INTERLEAVE[i];
    bool b = READ_BIT1(ambe, pos) != 0;

    WRITE_BIT1(buffer, n, b);
    n++;
  }

  m_state = DYDNS_NONE;

  return YSFDN_DATA_LENGTH;
}

void CDStarYSFDN::finish()
{
  // Drain any outstanding replies from the chip
  ambe3000.drain(m_n);
}
