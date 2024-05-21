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

#include "PCMALaw.h"

#include "Debug.h"

CPCMALaw::CPCMALaw() :
m_buffer(),
m_inUse(false)
{
}

CPCMALaw::~CPCMALaw()
{
}

uint8_t CPCMALaw::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("A-Law frame is being overwritten");
    return 0x05U;
  }

  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  int16_t audio[PCM_DATA_LENGTH / sizeof(short)];
  ::memcpy(audio, buffer, PCM_DATA_LENGTH);

  for (unsigned int i = 0U; i < (PCM_DATA_LENGTH / sizeof(short)); i++) {
    short pcm = audio[i];
 
    uint8_t eee  = 7U;
    int32_t mask = 0x4000;

    int32_t sign = (pcm & 0x8000) >> 8;

    pcm = sign ? (-pcm - 1) : pcm;

    while (((pcm & mask) == 0) && (eee > 0U)) {
      eee--;
      mask >>= 1;
    }

    uint8_t abcd = (pcm >> (eee ? (eee + 3U) : 4U)) & 0x0FU;

    eee <<= 4;

    m_buffer[i] = (sign | eee | abcd) ^ 0xD5U;
  }

  m_inUse = true;

  return 0x00U;
}

int16_t CPCMALaw::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, ALAW_DATA_LENGTH);
  m_inUse = false;

  return ALAW_DATA_LENGTH;
}
