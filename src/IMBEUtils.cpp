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

#include "IMBEUtils.h"

#include "Hamming.h"
#include "Golay.h"
#include "Debug.h"

const uint8_t BIT_MASK_TABLE8[]  = { 0x80U, 0x40U, 0x20U, 0x10U,
                                     0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT8(p,i,b)   p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE8[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE8[(i)&7])
#define READ_BIT8(p,i)     (p[(i)>>3] & BIT_MASK_TABLE8[(i)&7])

const int16_t BIT_MASK_TABLE16[] = { 0x0000, 0x4000, 0x2000, 0x1000,
                                     0x0800, 0x0400, 0x0200, 0x0100,
                                     0x0080, 0x0040, 0x0020, 0x0010,
                                     0x0008, 0x0004, 0x0002, 0x0001 };

#define WRITE_BIT16(p,i,b)   p[(i)>>4] = (b) ? (p[(i)>>4] | BIT_MASK_TABLE16[(i)&15]) : (p[(i)>>4] & ~BIT_MASK_TABLE16[(i)&15])
#define READ_BIT16(p,i)     (p[(i)>>4] & BIT_MASK_TABLE16[(i)&15])

const uint32_t BIT_MASK_TABLE32[] = { 0x80000000U, 0x40000000U, 0x20000000U, 0x10000000U,
                                      0x08000000U, 0x04000000U, 0x02000000U, 0x01000000U,
                                      0x00800000U, 0x00400000U, 0x00200000U, 0x00100000U,
                                      0x00080000U, 0x00040000U, 0x00020000U, 0x00010000U,
                                      0x00008000U, 0x00004000U, 0x00002000U, 0x00001000U,
                                      0x00000800U, 0x00000400U, 0x00000200U, 0x00000100U,
                                      0x00000080U, 0x00000040U, 0x00000020U, 0x00000010U,
                                      0x00000008U, 0x00000004U, 0x00000002U, 0x00000001U };

#define WRITE_BIT32(p,i,b)   p = (b) ? (p | BIT_MASK_TABLE32[(i)]) : (p & ~BIT_MASK_TABLE32[(i)])
#define READ_BIT32(p,i)     (p & BIT_MASK_TABLE32[(i)])


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

void CIMBEUtils::imbeToFEC(const int16_t* in, uint8_t* out)
{
  bool bTemp[144U];
  bool* bit = bTemp;

  // c0
  uint32_t c0 = in[0U];
  uint32_t g0 = CGolay::encode23127(c0);
  for (unsigned int i = 0U; i < 23U; i++)
    bit[i] = READ_BIT32(g0, i + 9U) != 0;
  bit += 23U;

  // c1
  uint32_t c1 = in[1U];
  uint32_t g1 = CGolay::encode23127(c1);
  for (unsigned int i = 0U; i < 23U; i++)
    bit[i] = READ_BIT32(g1, i + 9U) != 0;
  bit += 23U;

  // c2
  uint32_t c2 = in[2U];
  uint32_t g2 = CGolay::encode23127(c2);
  for (unsigned int i = 0U; i < 23U; i++)
    bit[i] = READ_BIT32(g2, i + 9U) != 0;
  bit += 23U;

  // c3
  uint32_t c3 = in[3U];
  uint32_t g3 = CGolay::encode23127(c3);
  for (unsigned int i = 0U; i < 23U; i++)
    bit[i] = READ_BIT32(g3, i + 9U) != 0;
  bit += 23U;

  // c4
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT16(in, i + 69U) != 0;
  CHamming::encode15113(bit);
  bit += 15U;

  // c5
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT16(in, i + 85U) != 0;
  CHamming::encode15113(bit);
  bit += 15U;

  // c6
  for (unsigned int i = 0U; i < 11U; i++)
    bit[i] = READ_BIT16(in, i + 100U) != 0;
  CHamming::encode15113(bit);
  bit += 15U;

  // c7
  for (unsigned int i = 0U; i < 7U; i++)
    bit[i] = READ_BIT16(in, i + 121U) != 0;

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
    WRITE_BIT8(out, n, bTemp[i]);
  }
}

void CIMBEUtils::fecToIMBE(const uint8_t* in, int16_t* out)
{
  bool temp[144U];

  // De-interleave
  for (uint8_t i = 0U; i < 144U; i++) {
    uint8_t n = IMBE_INTERLEAVE[i];
    temp[i] = READ_BIT8(in, n) != 0;
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
  uint32_t g0 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    WRITE_BIT32(g0, i + 9U, bit[i]);
  uint32_t c0 = CGolay::decode23127(g0);
  bit += 23U;

  bool prn[114U];

  // Create the whitening vector and save it for future use
  unsigned int p = 16U * c0;
  for (unsigned int i = 0U; i < 114U; i++) {
    p = (173U * p + 13849U) % 65536U;
    prn[i] = p >= 32768U;
  }

  // De-whiten some bits
  for (uint8_t i = 0U; i < 114U; i++)
    temp[i + 23U] ^= prn[i];

  // c1
  uint32_t g1 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    WRITE_BIT32(g1, i + 9U, bit[i]);
  uint32_t c1 = CGolay::decode23127(g1);
  bit += 23U;

  // c2
  uint32_t g2 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    WRITE_BIT32(g2, i + 9U, bit[i]);
  uint32_t c2 = CGolay::decode23127(g2);
  bit += 23U;

  // c3
  uint32_t g3 = 0U;
  for (uint8_t i = 0U; i < 23U; i++)
    WRITE_BIT32(g3, i + 9U, bit[i]);
  uint32_t c3 = CGolay::decode23127(g3);
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
  out[4U] = 0;
  out[5U] = 0;
  out[6U] = 0;
  out[7U] = 0;

  for (unsigned int i = 0U; i < 11U; i++)
    WRITE_BIT16(out, i + 69U, temp[i + 92U]);

  for (unsigned int i = 0U; i < 11U; i++)
    WRITE_BIT16(out, i + 85U, temp[i + 107U]);

  for (unsigned int i = 0U; i < 11U; i++)
    WRITE_BIT16(out, i + 101U, temp[i + 122U]);

  for (unsigned int i = 0U; i < 7U; i++)
    WRITE_BIT16(out, i + 121U, temp[i + 137U]);
}
