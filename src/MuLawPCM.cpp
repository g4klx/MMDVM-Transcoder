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

#include "MuLawPCM.h"

#include "Debug.h"

CMuLawPCM::CMuLawPCM() :
m_buffer(),
m_inUse(false)
{
}

CMuLawPCM::~CMuLawPCM()
{
}

uint8_t CMuLawPCM::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("PCM frame is being overwritten");
    return 0x05U;
  }

  if (length != MULAW_DATA_LENGTH) {
    DEBUG2("Mu-Law frame length is invalid", length);
    return 0x04U;
  }

  short audio[PCM_DATA_LENGTH / sizeof(short)];

  const unsigned short MULAW_BIAS = 33U;

  for (unsigned int i = 0U; i < MULAW_DATA_LENGTH; i++) {
    bool sign = true;

    unsigned char number = ~buffer[i];
    if (number & 0x80U) {
      number &= 0x7FU;
      sign = false;
    }

    unsigned char position = ((number & 0xF0U) >> 4) + 5U;

    short decoded = ((1 << position) | ((number & 0x0FU) << (position - 4U)) | (1 << (position - 5U))) - MULAW_BIAS;

    audio[i] = sign ? decoded : -decoded;
  }

  ::memcpy(m_buffer, audio, PCM_DATA_LENGTH);

  m_inUse = true;

  return 0x00U;
}

int16_t CMuLawPCM::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, PCM_DATA_LENGTH);
  m_inUse = false;

  return PCM_DATA_LENGTH;
}
