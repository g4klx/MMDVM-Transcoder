/*
 *   Copyright (C) 2010,2014,2016,2018,2023,2024 by Jonathan Naylor G4KLX
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

#include "DMRNXDNFEC.h"

#include "AMBEPRNGTable.h"
#include "Golay.h"
#include "Debug.h"
#include "Utils.h"

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t DMR_A_TABLE[] = { 0U,  4U,  8U, 12U, 16U, 20U, 24U, 28U, 32U, 36U, 40U, 44U,
                               48U, 52U, 56U, 60U, 64U, 68U,  1U,  5U,  9U, 13U, 17U, 21U};
const uint8_t DMR_B_TABLE[] = {25U, 29U, 33U, 37U, 41U, 45U, 49U, 53U, 57U, 61U, 65U, 69U,
                                2U,  6U, 10U, 14U, 18U, 22U, 26U, 30U, 34U, 38U, 42U};
const uint8_t DMR_C_TABLE[] = {46U, 50U, 54U, 58U, 62U, 66U, 70U,  3U,  7U, 11U, 15U, 19U,
                               23U, 27U, 31U, 35U, 39U, 43U, 47U, 51U, 55U, 59U, 63U, 67U, 71U};

CDMRNXDNFEC::CDMRNXDNFEC() :
m_buffer(),
m_inUse(false)
{
}

CDMRNXDNFEC::~CDMRNXDNFEC()
{
}

uint8_t CDMRNXDNFEC::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("DMR/NXDN frame is being overwritten");
    return 0x05U;
  }

  if (length != DMR_NXDN_DATA_LENGTH) {
    DEBUG2("DMR/NXDN frame length is invalid", length);
    return 0x04U;
  }

  uint32_t a = 0U;
  uint32_t MASK = 0x800000U;
  for (uint8_t i = 0U; i < 24U; i++, MASK >>= 1) {
    uint8_t aPos = DMR_A_TABLE[i];
    if (READ_BIT1(buffer, aPos))
      a |= MASK;
  }

  uint32_t b = 0U;
  MASK = 0x400000U;
  for (uint8_t i = 0U; i < 23U; i++, MASK >>= 1) {
    uint8_t bPos = DMR_B_TABLE[i];
    if (READ_BIT1(buffer, bPos))
      b |= MASK;
  }

  uint32_t c = 0U;
  MASK = 0x1000000U;
  for (uint8_t i = 0U; i < 25U; i++, MASK >>= 1) {
    uint8_t cPos = DMR_C_TABLE[i];
    if (READ_BIT1(buffer, cPos))
      c |= MASK;
  }

  bool ret = regenerateDMR(a, b, c);
  if (!ret) {
    DEBUG1("DMR/NXDN frame has uncorrectable errors");
    return 0x04U;
  }

  MASK = 0x800000U;
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
    WRITE_BIT1(m_buffer, cPos, c & MASK);
  }

  m_inUse = true;

  return 0x00U;
}

uint16_t CDMRNXDNFEC::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, DMR_NXDN_DATA_LENGTH);
  m_inUse = false;

  return DMR_NXDN_DATA_LENGTH;
}

bool CDMRNXDNFEC::regenerateDMR(uint32_t& a, uint32_t& b, uint32_t& c) const
{
  uint32_t orig_a = a;
  uint32_t orig_b = b;

  uint32_t data;
  bool valid = CGolay::decode24128(a, data);
  if (!valid)
    return false;

  a = CGolay::encode24128(data);

  // The PRNG
  uint32_t p = CAMBEPRNGTable::TABLE[data] >> 1;

  b ^= p;

  uint32_t datb = CGolay::decode23127(b);

  b = CGolay::encode23127(datb) >> 1;

  b ^= p;

  uint32_t v = a ^ orig_a;
  unsigned int errsA = ::countBits32(v);

  v = b ^ orig_b;
  unsigned int errsB = ::countBits32(v);

  if (errsA >= 4U || ((errsA + errsB) >= 6U && errsA >= 2U))
    return false;

  return true;
}
