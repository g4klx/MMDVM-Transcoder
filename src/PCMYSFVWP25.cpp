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

#include "PCMYSFVWP25.h"

#include "Hamming.h"
#include "Debug.h"
#include "Golay.h"

const uint8_t  BIT_MASK_TABLE8[]  = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT1(p,i,b)   p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE8[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE8[(i)&7])
#define READ_BIT1(p,i)     (p[(i)>>3] & BIT_MASK_TABLE8[(i)&7])

const uint8_t IMBE_INTERLEAVE[] = {
  0,  7, 12, 19, 24, 31, 36, 43, 48, 55, 60, 67, 72, 79, 84, 91,  96, 103, 108, 115, 120, 127, 132, 139,
  1,  6, 13, 18, 25, 30, 37, 42, 49, 54, 61, 66, 73, 78, 85, 90,  97, 102, 109, 114, 121, 126, 133, 138,
  2,  9, 14, 21, 26, 33, 38, 45, 50, 57, 62, 69, 74, 81, 86, 93,  98, 105, 110, 117, 122, 129, 134, 141,
  3,  8, 15, 20, 27, 32, 39, 44, 51, 56, 63, 68, 75, 80, 87, 92,  99, 104, 111, 116, 123, 128, 135, 140,
  4, 11, 16, 23, 28, 35, 40, 47, 52, 59, 64, 71, 76, 83, 88, 95, 100, 107, 112, 119, 124, 131, 136, 143,
  5, 10, 17, 22, 29, 34, 41, 46, 53, 58, 65, 70, 77, 82, 89, 94, 101, 106, 113, 118, 125, 130, 137, 142};


CPCMYSFVWP25::CPCMYSFVWP25() :
m_buffer(),
m_inUse(false)
{
}

CPCMYSFVWP25::~CPCMYSFVWP25()
{
}

uint8_t CPCMYSFVWP25::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("YSF VW/P25 frame is being overwritten");
    return 0x05U;
  }

  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  int16_t frameInt[8U];
  imbe.imbe_encode(frameInt, (int16_t*)buffer);

  uint8_t frame[11U];
  unsigned int offset = 0U;

  int16_t mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[0U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[1U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[2U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[3U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[4U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[5U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[6U] & mask) != 0);

  mask = 0x0040;
  for (unsigned int i = 0U; i < 7U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (frameInt[7U] & mask) != 0);

  bool bTemp[144U];
  bool* bit = bTemp;

  // c0
  unsigned int c0 = 0U;
  for (unsigned int i = 0U; i < 12U; i++) {
    bool b = READ_BIT1(frame, i);
    c0 = (c0 << 1) | (b ? 0x01U : 0x00U);
  }
  unsigned int g2 = CGolay::encode23127(c0);
  for (int i = 23; i >= 0; i--) {
    bit[i] = (g2 & 0x01U) == 0x01U;
    g2 >>= 1;
  }
  bit += 23U;

  // c1
  unsigned int c1 = 0U;
  for (unsigned int i = 12U; i < 24U; i++) {
    bool b = READ_BIT1(frame, i);
    c1 = (c1 << 1) | (b ? 0x01U : 0x00U);
  }
  g2 = CGolay::encode23127(c1);
  for (int i = 23; i >= 0; i--) {
    bit[i] = (g2 & 0x01U) == 0x01U;
    g2 >>= 1;
  }
  bit += 23U;

  // c2
  unsigned int c2 = 0;
  for (unsigned int i = 24U; i < 36U; i++) {
    bool b = READ_BIT1(frame, i);
    c2 = (c2 << 1) | (b ? 0x01U : 0x00U);
  }
  g2 = CGolay::encode23127(c2);
  for (int i = 23; i >= 0; i--) {
    bit[i] = (g2 & 0x01U) == 0x01U;
    g2 >>= 1;
  }
  bit += 23U;

  // c3
  unsigned int c3 = 0U;
  for (unsigned int i = 36U; i < 48U; i++) {
    bool b = READ_BIT1(frame, i);
    c3 = (c3 << 1) | (b ? 0x01U : 0x00U);
  }
  g2 = CGolay::encode23127(c3);
  for (int i = 23; i >= 0; i--) {
    bit[i] = (g2 & 0x01U) == 0x01U;
    g2 >>= 1;
  }
  bit += 23U;

  // c4
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT1(frame, i + 48U);
  CHamming::encode15113(bit);
  bit += 15U;

  // c5
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT1(frame, i + 59U);
  CHamming::encode15113(bit);
  bit += 15U;

  // c6
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT1(frame, i + 70U);
  CHamming::encode15113(bit);
  bit += 15U;

  // c7
  for (unsigned int i = 0U; i < 7U; i++)
    bit[i] = READ_BIT1(frame, i + 81U);

  bool prn[114U];

  // Create the whitening vector and save it for future use
  unsigned int p = 16U * c0;
  for (unsigned int i = 0U; i < 114U; i++) {
    p = (173U * p + 13849U) % 65536U;
    prn[i] = p >= 32768U;
  }

  // Whiten some bits
  for (unsigned int i = 0U; i < 114U; i++)
    bTemp[i + 23U] ^= prn[i];

  // Interleave
  for (unsigned int i = 0U; i < 144U; i++) {
    unsigned int n = IMBE_INTERLEAVE[i];
    WRITE_BIT1(m_buffer, n, bTemp[i]);
  }

  m_inUse = true;

  return 0x00U;
}

uint16_t CPCMYSFVWP25::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, YSFVW_P25_DATA_LENGTH);
  m_inUse = false;

  return YSFVW_P25_DATA_LENGTH;
}

