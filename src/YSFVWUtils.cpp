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

#include "YSFVWUtils.h"

#include "Hamming.h"
#include "Debug.h"
#include "Golay.h"

const uint8_t  BIT_MASK_TABLE[]  = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT1(p,i,b)   p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)     (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t IMBE_INTERLEAVE[] = {
  0U,  7U, 12U, 19U, 24U, 31U, 36U, 43U, 48U, 55U, 60U, 67U, 72U, 79U, 84U, 91U,
      96U, 103U, 108U, 115U, 120U, 127U, 132U, 139U,
  1U,  6U, 13U, 18U, 25U, 30U, 37U, 42U, 49U, 54U, 61U, 66U, 73U, 78U, 85U, 90U,
      97U, 102U, 109U, 114U, 121U, 126U, 133U, 138U,
  2U,  9U, 14U, 21U, 26U, 33U, 38U, 45U, 50U, 57U, 62U, 69U, 74U, 81U, 86U, 93U,
      98U, 105U, 110U, 117U, 122U, 129U, 134U, 141U,
  3U,  8U, 15U, 20U, 27U, 32U, 39U, 44U, 51U, 56U, 63U, 68U, 75U, 80U, 87U, 92U,
      99U, 104U, 111U, 116U, 123U, 128U, 135U, 140U,
  4U, 11U, 16U, 23U, 28U, 35U, 40U, 47U, 52U, 59U, 64U, 71U, 76U, 83U, 88U, 95U,
     100U, 107U, 112U, 119U, 124U, 131U, 136U, 143U,
  5U, 10U, 17U, 22U, 29U, 34U, 41U, 46U, 53U, 58U, 65U, 70U, 77U, 82U, 89U, 94U,
     101U, 106U, 113U, 118U, 125U, 130U, 137U, 142U};

void CYSFVWUtils::fromIMBE(const int16_t* in, uint8_t* out)
{
  uint8_t frame[11U];
  unsigned int offset = 0U;

  int16_t mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[0U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[1U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[2U] & mask) != 0);

  mask = 0x0800;
  for (unsigned int i = 0U; i < 12U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[3U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[4U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[5U] & mask) != 0);

  mask = 0x0400;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[6U] & mask) != 0);

  mask = 0x0040;
  for (unsigned int i = 0U; i < 7U; i++, mask >>= 1, offset++)
    WRITE_BIT1(frame, offset, (in[7U] & mask) != 0);

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
    WRITE_BIT1(out, n, bTemp[i]);
  }
}

void CYSFVWUtils::toIMBE(const uint8_t* in, int16_t* out)
{
  bool temp[144U];

  // De-interleave
  for (uint8_t i = 0U; i < 144U; i++) {
    uint8_t n = IMBE_INTERLEAVE[i];
    temp[i] = READ_BIT1(in, n);
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

  // Process the c0 section first to allow the de-whitening to be accurate

  // Check/Fix FEC
  bool* bit = temp;

  // c0
  uint32_t g1 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    g1 = (g1 << 1) | (bit[i] ? 0x01U : 0x00U);
  uint32_t c0 = CGolay::decode23127(g1);
  bit += 23U;

  bool prn[114U];

  // Create the whitening vector and save it for future use
  uint8_t p = 16U * c0;
  for (uint8_t i = 0U; i < 114U; i++) {
    p = (173U * p + 13849U) % 65536U;
    prn[i] = p >= 32768U;
  }

  // De-whiten some bits
  for (uint8_t i = 0U; i < 114U; i++)
    temp[i + 23U] ^= prn[i];

  // c1
  g1 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    g1 = (g1 << 1) | (bit[i] ? 0x01U : 0x00U);
  uint32_t c1 = CGolay::decode23127(g1);
  bit += 23U;

  // c2
  g1 = 0;
  for (uint8_t i = 0U; i < 23U; i++)
    g1 = (g1 << 1) | (bit[i] ? 0x01U : 0x00U);
  uint32_t c2 = CGolay::decode23127(g1);
  bit += 23U;

  // c3
  g1 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    g1 = (g1 << 1) | (bit[i] ? 0x01U : 0x00U);
  uint32_t c3 = CGolay::decode23127(g1);
  bit += 23U;

  // c4
  CHamming::decode15113(bit);
  bit += 15U;

  // c5
  CHamming::decode15113(bit);
  bit += 15U;

  // c6
  CHamming::decode15113(bit);
  bit += 15U;

  // c7

  out[0U] = c0;
  out[1U] = c1;
  out[2U] = c2;
  out[3U] = c3;

  unsigned int offset = 92U;
  int16_t mask = 0x0400;
  out[4U] = 0x0000;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++) {
    if (temp[offset])
      out[4U] |= mask;
  }

  offset = 107U;
  mask = 0x0400;
  out[5U] = 0x0000;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++) {
    if (temp[offset])
      out[5U] |= mask;
  }

  offset = 122U;
  mask = 0x0400;
  out[6U] = 0x0000;
  for (unsigned int i = 0U; i < 11U; i++, mask >>= 1, offset++) {
    if (temp[offset])
      out[6U] |= mask;
  }

  offset = 137U;
  mask = 0x0040;
  out[7U] = 0x0000;
  for (unsigned int i = 0U; i < 7U; i++, mask >>= 1, offset++) {
    if (temp[offset])
      out[7U] |= mask;
  }
}
