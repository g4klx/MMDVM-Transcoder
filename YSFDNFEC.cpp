/*
 *   Copyright (C) 2010,2014,2016,2018,2023 by Jonathan Naylor G4KLX
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

#include "YSFDNFEC.h"

#include "Debug.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CYSFDNFEC::CYSFDNFEC() :
m_buffer(),
m_inUse(false)
{
}

CYSFDNFEC::~CYSFDNFEC()
{
}

uint8_t CYSFDNFEC::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("YSF DN frame is being overwritten");
    return 0x04U;
  }

  if (length != YSFDN_DATA_LENGTH) {
    DEBUG2("YSF DN frame length is invalid", length);
    return 0x04U;
  }

  for (uint8_t i = 0U; i < 81U; i += 3) {
    uint8_t vote = 0U;
    vote += READ_BIT1(buffer, i + 0U) ? 1U : 0U;
    vote += READ_BIT1(buffer, i + 1U) ? 1U : 0U;
    vote += READ_BIT1(buffer, i + 2U) ? 1U : 0U;

    switch (vote) {
      case 0U:    // 0 0 0
      case 1U:    // 1 0 0, or 0 1 0, or 0 0 1, convert to 0 0 0
        WRITE_BIT1(m_buffer, i + 0U, false);
        WRITE_BIT1(m_buffer, i + 1U, false);
        WRITE_BIT1(m_buffer, i + 2U, false);
        break;
      case 3U:    // 1 1 1
      case 2U:    // 1 1 0, or 0 1 1, or 1 0 1, convert to 1 1 1
        WRITE_BIT1(m_buffer, i + 0U, true);
        WRITE_BIT1(m_buffer, i + 1U, true);
        WRITE_BIT1(m_buffer, i + 2U, true);
        break;
      default:
        break;
    }
  }

  for (uint8_t i = 81U; i < 103U; i++) {
    bool b = READ_BIT1(buffer, i) != 0U;
    WRITE_BIT1(m_buffer, i, b);
  }

  WRITE_BIT1(m_buffer, 103U, false);

  m_inUse = true;

  return 0x00U;
}

uint16_t CYSFDNFEC::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, YSFDN_DATA_LENGTH);
  m_inUse = false;

  return YSFDN_DATA_LENGTH;
}

