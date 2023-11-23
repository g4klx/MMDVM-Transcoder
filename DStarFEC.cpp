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

#include "DStarFEC.h"

#include "AMBEPRNGTable.h"
#include "Golay.h"
#include "Debug.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])


const uint8_t DSTAR_A_TABLE[] = {0U,  6U, 12U, 18U, 24U, 30U, 36U, 42U, 48U, 54U, 60U, 66U,
                                 1U,  7U, 13U, 19U, 25U, 31U, 37U, 43U, 49U, 55U, 61U, 67U};
const uint8_t DSTAR_B_TABLE[] = {2U,  8U, 14U, 20U, 26U, 32U, 38U, 44U, 50U, 56U, 62U, 68U,
                                 3U,  9U, 15U, 21U, 27U, 33U, 39U, 45U, 51U, 57U, 63U, 69U};
const uint8_t DSTAR_C_TABLE[] = {4U, 10U, 16U, 22U, 28U, 34U, 40U, 46U, 52U, 58U, 64U, 70U,
                                 5U, 11U, 17U, 23U, 29U, 35U, 41U, 47U, 53U, 59U, 65U, 71U};

CDStarFEC::CDStarFEC() :
m_buffer(),
m_inUse(false)
{
}

CDStarFEC::~CDStarFEC()
{
}

uint8_t CDStarFEC::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("D-Star frame is being overwritten");
    return 0x04U;
  }

  if (length != DSTAR_DATA_LENGTH) {
    DEBUG2("D-Star frame length is invalid", length);
    return 0x04U;
  }

  uint32_t a = 0U;
  uint32_t b = 0U;
  uint32_t c = 0U;

  uint32_t MASK = 0x800000U;
  for (uint8_t i = 0U; i < 24U; i++) {
    if (READ_BIT1(buffer, DSTAR_A_TABLE[i]))
      a |= MASK;
    if (READ_BIT1(buffer, DSTAR_B_TABLE[i]))
      b |= MASK;
    if (READ_BIT1(buffer, DSTAR_C_TABLE[i]))
      c |= MASK;

    MASK >>= 1;
  }

  bool ret = regenerateDStar(a, b);
  if (!ret) {
    DEBUG1("D-Star frame has uncorrectable errors");
    return 0x04U;
  }

  MASK = 0x800000U;
  for (uint8_t i = 0U; i < 24U; i++) {
    WRITE_BIT1(m_buffer, DSTAR_A_TABLE[i], a & MASK);
    WRITE_BIT1(m_buffer, DSTAR_B_TABLE[i], b & MASK);
    WRITE_BIT1(m_buffer, DSTAR_C_TABLE[i], c & MASK);

    MASK >>= 1;
  }

  m_inUse = true;

  return 0x00U;
}

uint16_t CDStarFEC::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, DSTAR_DATA_LENGTH);
  m_inUse = false;

  return DSTAR_DATA_LENGTH;
}

bool CDStarFEC::regenerateDStar(uint32_t& a, uint32_t& b) const
{
  uint32_t data;
  bool valid1 = CGolay::decode24128(a, data);
  if (!valid1)
    return false;

  // The PRNG
  uint32_t p = CAMBEPRNGTable::TABLE[data];

  b ^= p;

  uint32_t datb;
  bool valid2 = CGolay::decode24128(b, datb);
  if (!valid2)
    return false;

  a = CGolay::encode24128(data);
  b = CGolay::encode24128(datb);

  b ^= p;

  return true;
}

