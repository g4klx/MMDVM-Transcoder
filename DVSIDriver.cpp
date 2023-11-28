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

#include "DVSIDriver.h"

#include "Globals.h"
#include "Config.h"

const uint8_t DVSI_START_BYTE = 0x61U;

CDVSIDriver::CDVSIDriver() :
m_buffer(),
m_len(0U),
m_ptr(0U)
{
  serial.beginInt(3U, DVSI_SPEED, true);
}

void CDVSIDriver::reset()
{
  // FIXME TODO Twiddle the GPIO pin
}

void CDVSIDriver::write(const uint8_t* buffer, uint16_t length)
{
  serial.writeInt(3U, buffer, length);
}

uint16_t CDVSIDriver::read(uint8_t* buffer)
{
  while (serial.availableForReadInt(3U)) {
    uint8_t c = serial.readInt(3U);

    if (m_ptr == 0U) {
      if (c == DVSI_START_BYTE) {
        // Handle the frame start correctly
        m_buffer[0U] = c;
        m_ptr = 1U;
        m_len = 0U;
      } else {
        m_ptr = 0U;
        m_len = 0U;
      }
    } else if (m_ptr == 1U) {
      // Handle the frame length MSB
      uint8_t val = m_buffer[m_ptr] = c;
      m_len = (val << 8) & 0xFF00U;
      m_ptr = 2U;
    } else if (m_ptr == 2U) {
      // Handle the frame length LSB
      uint8_t val = m_buffer[m_ptr] = c;
      m_len |= (val << 0) & 0x00FFU;
      m_len += 4U;	// The length in the DVSI message doesn't include the first four bytes
      m_ptr  = 3U;
    } else {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // The full packet has been received, process it
      if (m_ptr == m_len) {
        ::memcpy(buffer, m_buffer, m_len);
        uint16_t length = m_len;

        m_ptr = 0U;
        m_len = 0U;

        return length;
      }
    }
  }

  return 0U;
}

