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

#include "PCMMuLaw.h"

#include "Debug.h"

CPCMMuLaw::CPCMMuLaw() :
m_buffer(),
m_inUse(false)
{
}

CPCMMuLaw::~CPCMMuLaw()
{
}

uint8_t CPCMMuLaw::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("Mu-Law frame is being overwritten");
    return 0x05U;
  }

  if (length != PCM_DATA_LENGTH) {
    DEBUG2("PCM frame length is invalid", length);
    return 0x04U;
  }

  short audio[PCM_DATA_LENGTH / sizeof(short)];
  ::memcpy(audio, buffer, PCM_DATA_LENGTH);

	const unsigned short MULAW_MAX  = 0x1FFFU;
	const unsigned short MULAW_BIAS = 33U;

	for (unsigned int i = 0U; i < (PCM_DATA_LENGTH / sizeof(short)); i++) {
		unsigned short mask = 0x1000U;
		unsigned char  sign;
		unsigned char position = 12U;
		unsigned short number;

		if (audio[i] < 0) {
			number = -audio[i];
			sign   = 0x80U;
		} else {
			number = audio[i];
			sign   = 0x00U;
		}

		number += MULAW_BIAS;
		if (number > MULAW_MAX)
			number = MULAW_MAX;

		for (; ((number & mask) != mask && position >= 5U); mask >>= 1, position--)
			;

		unsigned char lsb = (number >> (position - 4U)) & 0x0FU;

		m_buffer[i] = ~(sign | ((position - 5U) << 4) | lsb);
	}

  m_inUse = true;

  return 0x00U;
}

int16_t CPCMMuLaw::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, MULAW_DATA_LENGTH);
  m_inUse = false;

  return MULAW_DATA_LENGTH;
}
