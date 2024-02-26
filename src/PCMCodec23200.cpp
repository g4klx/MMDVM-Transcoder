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

#include "PCMCodec23200.h"

#include "Debug.h"

CPCMCodec23200::CPCMCodec23200() :
m_buffer(),
m_inUse(false)
{
}

CPCMCodec23200::~CPCMCodec23200()
{
}

uint8_t CPCMCodec23200::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("Codec2 3200 frame is being overwritten");
    return 0x05U;
  }

  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

#if defined(HAS_STLINK)
  serial.dump("Codec2 3200 PCM", buffer, PCM_DATA_LENGTH);
#endif

  codec23200.codec2_encode((unsigned char*)m_buffer, (short*)buffer);

#if defined(HAS_STLINK)
  serial.dump("Codec2 3200 Data", m_buffer, CODEC2_3200_DATA_LENGTH);
#endif

  m_inUse = true;

  return 0x00U;
}

uint16_t CPCMCodec23200::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0U;

  ::memcpy(buffer, m_buffer, CODEC2_3200_DATA_LENGTH);
  m_inUse = false;

  return CODEC2_3200_DATA_LENGTH;
}

