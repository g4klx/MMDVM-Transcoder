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

#include "Codec23200PCM.h"

#include "Debug.h"

CCodec23200PCM::CCodec23200PCM() :
m_buffer(),
m_inUse(false)
{
}

CCodec23200PCM::~CCodec23200PCM()
{
}

uint8_t CCodec23200PCM::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("PCM frame is being overwritten");
    return 0x05U;
  }

  if (length != CODEC2_3200_DATA_LENGTH) {
    DEBUG2("Codec2 3200 frame length is invalid", length);
    return 0x04U;
  }

  codec23200.codec2_decode((short*)m_buffer, (unsigned char*)buffer);

  m_inUse = true;

  return 0x00U;
}

uint16_t CCodec23200PCM::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, PCM_DATA_LENGTH);
  m_inUse = false;

  return PCM_DATA_LENGTH;
}

