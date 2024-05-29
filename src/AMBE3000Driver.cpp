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

const uint16_t DVSI_PCM_SAMPLES = 160U;
const uint16_t DVSI_PCM_BYTES   = DVSI_PCM_SAMPLES * sizeof(int16_t);

CAMBE3000Driver::CAMBE3000Driver(uint8_t n, IDVSIDriver& dvsi) :
m_n(n),
m_dvsi(dvsi),
m_buffer(),
m_length(0U),
m_utils()
{
}

void CAMBE3000Driver::startup()
{
  m_dvsi.reset();
}

void CAMBE3000Driver::init(AMBE_MODE mode)
{
  uint8_t buffer[100U];
  uint16_t length = m_utils.createModeChange(mode, buffer);

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (m_n == 0U)
    leds.setLED1(true);
  else
    leds.setLED3(true);
#else
  leds.setLED1(true);
#endif
#endif

  m_dvsi.write(buffer, length);
}

void CAMBE3000Driver::process()
{
  uint8_t buffer[500U];
  uint16_t length = m_dvsi.read(buffer);
  if (length == 0U)
    return;

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (m_n == 0U)
    leds.setLED1(false);
  else
    leds.setLED3(false);
#else
  leds.setLED1(false);
#endif
#endif

  uint16_t pos = 0U;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 4U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_RATET:
            DEBUG2("Response AMBE3000 to PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATEP:
            DEBUG2("Response AMBE3000 to PKT_RATEP is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          default:
            DEBUG2("Unknown AMBE3000 control type response of ", buffer[pos]);
            return;
        }
      }
      break;

    case DVSI_TYPE_AMBE:
      m_length = m_utils.extractAMBEFrame(buffer, m_buffer);
      break;

    case DVSI_TYPE_AUDIO:
      m_length = m_utils.extractPCMFrame(buffer, m_buffer);
      break;

    default:
      DEBUG2("Unknown AMBE3000 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE3000Driver::writeAMBE(const uint8_t* buffer)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!m_dvsi.ready()) {
    DEBUG1("The AMBE3000 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[50U];
  uint16_t pos = m_utils.createAMBEFrame(buffer, out);

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (m_n == 0U)
    leds.setLED1(true);
  else
    leds.setLED3(true);
#else
  leds.setLED1(true);
#endif
#endif

  m_dvsi.write(out, pos);

  return 0x00U;
}

uint8_t CAMBE3000Driver::writePCM(const uint8_t* buffer)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!m_dvsi.ready()) {
    DEBUG1("The AMBE3000 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[400U];
  uint16_t pos = m_utils.createPCMFrame(buffer, out);

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (m_n == 0U)
    leds.setLED1(true);
  else
    leds.setLED3(true);
#else
  leds.setLED1(true);
#endif
#endif

  m_dvsi.write(out, pos);

  return 0x00U;
}

AD_STATE CAMBE3000Driver::readAMBE(uint8_t* buffer)
{
  switch (m_length) {
    case 0U:
      return ADS_NO_DATA;

    case DVSI_PCM_BYTES:
      m_length = 0U;
      return ADS_WRONG_TYPE;

    default:
      ::memcpy(buffer, m_buffer, m_length);
      m_length = 0U;
      return ADS_DATA;
  }
}

AD_STATE CAMBE3000Driver::readPCM(uint8_t* buffer)
{
  switch (m_length) {
    case 0U:
      return ADS_NO_DATA;

    case DVSI_PCM_BYTES:
      ::memcpy(buffer, m_buffer, DVSI_PCM_BYTES);
      m_length = 0U;
      return ADS_DATA;

    default:
      m_length = 0U;
      return ADS_WRONG_TYPE;
  }
}

void CAMBE3000Driver::drain()
{
  m_length = 0U;
}

#endif
