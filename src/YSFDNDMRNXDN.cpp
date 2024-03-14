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

#include "YSFDNDMRNXDN.h"

#include "AMBEPRNGTable.h"
#include "Golay.h"
#include "Debug.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t DMR_A_TABLE[] = { 0U,  4U,  8U, 12U, 16U, 20U, 24U, 28U, 32U, 36U, 40U, 44U,
                               48U, 52U, 56U, 60U, 64U, 68U,  1U,  5U,  9U, 13U, 17U, 21U};
const uint8_t DMR_B_TABLE[] = {25U, 29U, 33U, 37U, 41U, 45U, 49U, 53U, 57U, 61U, 65U, 69U,
                                2U,  6U, 10U, 14U, 18U, 22U, 26U, 30U, 34U, 38U, 42U};
const uint8_t DMR_C_TABLE[] = {46U, 50U, 54U, 58U, 62U, 66U, 70U,  3U,  7U, 11U, 15U, 19U,
                               23U, 27U, 31U, 35U, 39U, 43U, 47U, 51U, 55U, 59U, 63U, 67U, 71U};

CYSFDNDMRNXDN::CYSFDNDMRNXDN() :
m_buffer(),
m_inUse(false)
{
}

CYSFDNDMRNXDN::~CYSFDNDMRNXDN()
{
}

uint8_t CYSFDNDMRNXDN::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("DMR/NXDN frame is being overwritten");
    return 0x05U;
  }

  if (length != YSFDN_DATA_LENGTH) {
    DEBUG2("YSF DN frame length is invalid", length);
    return 0x04U;
  }

  uint32_t data = 0U;
  uint32_t datb = 0U;
  uint32_t datc = 0U;

  for (uint8_t i = 0U; i < 12U; i++) {
    data <<= 1;

    if (READ_BIT1(buffer, 3U * i + 1U))
      data |= 0x01U;
  }
    
  for (uint8_t i = 0U; i < 12U; i++) {
    datb <<= 1;

    if (READ_BIT1(buffer, 3U * (i + 12U) + 1U))
      datb |= 0x01U;;
  }
    
  for (uint8_t i = 0U; i < 3U; i++) {
    datc <<= 1;

    if (READ_BIT1(buffer, 3U * (i + 24U) + 1U))
      datc |= 0x01U;;
  }

  for (uint8_t i = 0U; i < 22U; i++) {
    datc <<= 1;

    if (READ_BIT1(buffer, i + 81U))
      datc |= 0x01U;;
  }

  uint8_t a = CGolay::encode24128(data);
  uint8_t p = CAMBEPRNGTable::TABLE[data] >> 1;
  uint8_t b = CGolay::encode23127(datb) >> 1;
  b ^= p;

  uint32_t MASK = 0x800000U;
  for (uint8_t i = 0U; i < 24U; i++, MASK >>= 1) {
    uint8_t aPos = DMR_A_TABLE[i];
    WRITE_BIT1(m_buffer, aPos, a & MASK);
  }

  MASK = 0x400000U;
  for (uint8_t i = 0U; i < 23U; i++, MASK >>= 1) {
    uint8_t bPos = DMR_B_TABLE[i];
    WRITE_BIT1(m_buffer, bPos, b & MASK);
  }

  MASK = 0x1000000U;
  for (uint8_t i = 0U; i < 25U; i++, MASK >>= 1) {
    uint8_t cPos = DMR_C_TABLE[i];
    WRITE_BIT1(m_buffer, cPos, datc & MASK);
  }

  m_inUse = true;

  return 0x00U;
}

int16_t CYSFDNDMRNXDN::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, DMR_NXDN_DATA_LENGTH);
  m_inUse = false;

  return DMR_NXDN_DATA_LENGTH;
}
