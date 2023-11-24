/*
 *   Copyright (C) 2023 by Jonathan Naylor G4KLX
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

#include "YSFVWP25PCM.h"

#include "Debug.h"

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


CYSFVWP25PCM::CYSFVWP25PCM() :
m_buffer(),
m_inUse(false)
{
}

CYSFVWP25PCM::~CYSFVWP25PCM()
{
}

uint8_t CYSFVWP25PCM::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("PCM frame is being overwritten");
    return 0x04U;
  }

  if (length != YSFVW_P25_DATA_LENGTH) {
    DEBUG2("YSF VW/P25 frame length is invalid", length);
    return 0x04U;
  }

  // FIXME TODO This doesn't fix any errors in the IMBE data and it's inefficient

  uint8_t data[11U];

  bool bit[144U];

  // De-interleave
  for (unsigned int i = 0U; i < 144U; i++) {
    unsigned int n = IMBE_INTERLEAVE[i];
    bit[i] = READ_BIT1(buffer, n);
  }

  // now ..

  // 12 voice bits     0
  // 11 golay bits     12
  //
  // 12 voice bits     23
  // 11 golay bits     35
  //
  // 12 voice bits     46
  // 11 golay bits     58
  //
  // 12 voice bits     69
  // 11 golay bits     81
  //
  // 11 voice bits     92
  //  4 hamming bits   103
  //
  // 11 voice bits     107
  //  4 hamming bits   118
  //
  // 11 voice bits     122
  //  4 hamming bits   133
  //
  //  7 voice bits     137

  // c0
  unsigned int c0data = 0U;
  for (unsigned int i = 0U; i < 12U; i++)
    c0data = (c0data << 1) | (bit[i] ? 0x01U : 0x00U);

  bool prn[114U];

  // Create the whitening vector and save it for future use
  unsigned int p = 16U * c0data;
  for (unsigned int i = 0U; i < 114U; i++) {
    p = (173U * p + 13849U) % 65536U;
    prn[i] = p >= 32768U;
  }

  // De-whiten some bits
  for (unsigned int i = 0U; i < 114U; i++)
    bit[i + 23U] ^= prn[i];

  unsigned int offset = 0U;
  for (unsigned int i = 0U; i < 12U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 0U]);
  for (unsigned int i = 0U; i < 12U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 23U]);
  for (unsigned int i = 0U; i < 12U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 46U]);
  for (unsigned int i = 0U; i < 12U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 69U]);
  for (unsigned int i = 0U; i < 11U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 92U]);
  for (unsigned int i = 0U; i < 11U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 107U]);
  for (unsigned int i = 0U; i < 11U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 122U]);
  for (unsigned int i = 0U; i < 7U; i++, offset++)
    WRITE_BIT1(data, offset, bit[i + 137U]);

  int16_t frame[8U] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

  offset = 0U;

  int16_t mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    frame[0U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    frame[1U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    frame[2U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    frame[3U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    frame[4U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    frame[5U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    frame[6U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  mask = 0x0040;
  for (unsigned int i = 0U; i < 7U; i++, mask >>= 1, offset++)
    frame[7U] |= READ_BIT1(data, offset) != 0x00U ? mask : 0x0000;

  imbe.imbe_decode(frame, (int16_t*)m_buffer);

  m_inUse = true;

  return 0x00U;
}

uint16_t CYSFVWP25PCM::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, PCM_DATA_LENGTH);
  m_inUse = false;

  return PCM_DATA_LENGTH;
}

