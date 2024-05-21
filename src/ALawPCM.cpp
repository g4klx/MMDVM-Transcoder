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

#include "ALawPCM.h"

#include "Debug.h"

CALawPCM::CALawPCM() :
m_buffer(),
m_inUse(false)
{
}

CALawPCM::~CALawPCM()
{
}

uint8_t CALawPCM::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("PCM frame is being overwritten");
    return 0x05U;
  }

  if (length != ALAW_DATA_LENGTH) {
    DEBUG2("A-Law frame length is invalid", length);
    return 0x04U;
  }

  short audio[PCM_DATA_LENGTH / sizeof(short)];

  for (unsigned int i = 0U; i < ALAW_DATA_LENGTH; i++) {
    uint8_t alaw = buffer[i] ^ 0xD5U;

    bool sign = (alaw & 0x80U) == 0x80U;

    uint8_t eee = (alaw & 0x70U) >> 4;

    short pcm = (alaw & 0x0FU) << 4 | 8;

    pcm += (eee > 0U) ? 0x100 : 0x000;

    pcm <<= (eee > 1U) ? (eee - 1U) : 0U;

    audio[i] = sign ? -pcm : pcm;
  }
    
  ::memcpy(m_buffer, audio, PCM_DATA_LENGTH);

  m_inUse = true;

  return 0x00U;
}

int16_t CALawPCM::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, PCM_DATA_LENGTH);
  m_inUse = false;

  return PCM_DATA_LENGTH;
}
