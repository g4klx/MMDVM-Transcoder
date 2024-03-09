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

#include "AMBE3000Driver.h"

#if AMBE_TYPE > 0

#include "Globals.h"
#include "Debug.h"

const uint8_t DVSI_START_BYTE   = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET    = 0x09U;
const uint8_t DVSI_PKT_RATEP    = 0x0AU;
const uint8_t DVSI_PKT_CHANNEL0 = 0x40U;
const uint8_t DVSI_PKT_CHANNEL1 = 0x41U;
const uint8_t DVSI_PKT_CHANNEL2 = 0x42U;

CAMBE3000Driver::CAMBE3000Driver() :
m_buffer(),
m_length(),
m_utils()
{
  m_buffer[0U] = new uint8_t[400U];
#if AMBE_TYPE == 2
  m_buffer[1U] = new uint8_t[400U];
  m_buffer[2U] = new uint8_t[400U];
#endif

  m_length[0U] = 0U;
#if AMBE_TYPE == 2
  m_length[1U] = 0U;
  m_length[2U] = 0U;
#endif
}

void CAMBE3000Driver::startup()
{
  dvsi.reset();
}

void CAMBE3000Driver::init(uint8_t n, AMBE_MODE mode)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  uint8_t buffer[100U];
  uint16_t length = m_utils[n].createModeChange(n, mode, buffer);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(buffer, length);
}

void CAMBE3000Driver::process()
{
  uint8_t buffer[500U];
  uint16_t length = dvsi.read(buffer);
  if (length == 0U)
    return;

#if defined(HAS_LEDS)
  leds.setLED1(false);
#endif

  uint16_t pos = 0U;
  uint8_t n = 0U;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 4U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_CHANNEL0:
            DEBUG1("Response AMBE3000/3003 to PKT_CHANNEL0");
            pos += 2U;
            n    = 0U;
            break;

          case DVSI_PKT_CHANNEL1:
            DEBUG1("Response AMBE3000/3003 to PKT_CHANNEL1");
            pos += 2U;
            n    = 1U;
            break;

          case DVSI_PKT_CHANNEL2:
            DEBUG1("Response AMBE3000/3003 to PKT_CHANNEL2");
            pos += 2U;
            n    = 2U;
            break;

          case DVSI_PKT_RATET:
            DEBUG2("Response AMBE3000/3003 to PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATEP:
            DEBUG2("Response AMBE3000/3003 to PKT_RATEP is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          default:
            DEBUG2("Unknown AMBE3000/3003 control type response of ", buffer[pos]);
            return;
        }
      }
      break;

    case DVSI_TYPE_AMBE:
      m_length[n] = m_utils[n].extractAMBEFrame(buffer, m_buffer[n]);
      break;

    case DVSI_TYPE_AUDIO:
      m_length[n] = m_utils[n].extractPCMFrame(buffer, m_buffer[n]);
      break;

    default:
      DEBUG2("Unknown AMBE3000/3003 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE3000Driver::writeAMBE(uint8_t n, const uint8_t* buffer, const uint8_t* frame, uint16_t len)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!dvsi.ready()) {
    DEBUG1("The AMBE3000/3003 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[50U];
  uint16_t pos = m_utils[n].createAMBEFrame(n, buffer, out);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(out, pos, frame, len);

  return 0x00U;
}

uint8_t CAMBE3000Driver::writePCM(uint8_t n, const uint8_t* buffer, const uint8_t* frame, uint16_t len)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!dvsi.ready()) {
    DEBUG1("The AMBE3000/3003 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[400U];
  uint16_t pos = m_utils[n].createPCMFrame(n, buffer, out);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(out, pos, frame, len);

  return 0x00U;
}

bool CAMBE3000Driver::read(uint8_t n, uint8_t* buffer)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  if (m_length[n] > 0U) {
    ::memcpy(buffer, m_buffer[n], m_length[n]);
    m_length[n] = 0U;
    return true;
  }

  return false;
}

#endif
