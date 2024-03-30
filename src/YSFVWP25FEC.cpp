/*
 *   Copyright (C) 2010,2014,2016,2018,2023,2024 by Jonathan Naylor G4KLX
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

#include "YSFVWP25FEC.h"

#include "YSFVWUtils.h"
#include "Debug.h"


CYSFVWP25FEC::CYSFVWP25FEC() :
m_buffer(),
m_inUse(false)
{
}

CYSFVWP25FEC::~CYSFVWP25FEC()
{
}

uint8_t CYSFVWP25FEC::input(const uint8_t* buffer, uint16_t length)
{
  if (m_inUse) {
    DEBUG1("YSF VW/P25 FEC frame is being overwritten");
    return 0x05U;
  }

  if (length != YSFVW_P25_FEC_DATA_LENGTH) {
    DEBUG2("YSF VW/P25 FEC frame length is invalid", length);
    return 0x04U;
  }

  int16_t frame[8U];
  CYSFVWUtils::toIMBE(buffer, frame);
  CYSFVWUtils::fromIMBE(frame, m_buffer);

  m_inUse = true;

  return 0x00U;
}

int16_t CYSFVWP25FEC::output(uint8_t* buffer)
{
  if (!m_inUse)
    return 0;

  ::memcpy(buffer, m_buffer, YSFVW_P25_FEC_DATA_LENGTH);
  m_inUse = false;

  return YSFVW_P25_FEC_DATA_LENGTH;
}
