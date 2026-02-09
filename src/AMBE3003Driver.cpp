/*
 *   Copyright (C) 2023,2024,2026 by Jonathan Naylor G4KLX
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

#include "AMBE3003Driver.h"

#if AMBE_TYPE == 3

#include "Globals.h"
#include "Debug.h"

const uint8_t DVSI_START_BYTE   = 0x61U;

const uint8_t DVSI_CHANNEL_BASE = 0x40U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET    = 0x09U;
const uint8_t DVSI_PKT_RATEP    = 0x0AU;

const uint16_t DVSI_PCM_SAMPLES = 160U;
const uint16_t DVSI_PCM_BYTES   = DVSI_PCM_SAMPLES * sizeof(int16_t);

CAMBE3003Driver::CAMBE3003Driver() :
m_buffer0(),
m_buffer1(),
m_buffer2(),
m_length0(0U),
m_length1(0U),
m_length2(0U),
m_utils()
{
}

void CAMBE3003Driver::startup()
{
  dvsi.reset();
}

void CAMBE3003Driver::init(uint8_t n, AMBE_MODE mode)
{
  uint8_t buffer[100U];
  uint16_t length = m_utils.createModeChange(n, mode, buffer);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(buffer, length);
}

void CAMBE3003Driver::process()
{
  uint8_t buffer[500U];
  uint16_t length = dvsi.read(buffer);
  if (length == 0U)
    return;

#if defined(HAS_LEDS)
  leds.setLED1(false);
#endif

  uint16_t pos = 0U;

  uint8_t n = buffer[4U] - DVSI_CHANNEL_BASE;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 6U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_RATET:
            DEBUG2("Response AMBE3003 to PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATEP:
            DEBUG2("Response AMBE3003 to PKT_RATEP is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          default:
            DEBUG2("Unknown AMBE3003 control type response of ", buffer[pos]);
            return;
        }
      }
      break;

    case DVSI_TYPE_AMBE:
      switch (n) {
        case 0U:
          m_length0 = m_utils.extractAMBEFrame(buffer, m_buffer0);
          break;
        case 1U:
          m_length1 = m_utils.extractAMBEFrame(buffer, m_buffer1);
          break;
        default:
          m_length2 = m_utils.extractAMBEFrame(buffer, m_buffer2);
          break;
      }
      break;

    case DVSI_TYPE_AUDIO:
      switch (n) {
        case 0U:
          m_length0 = m_utils.extractPCMFrame(buffer, m_buffer0);
          break;
        case 1U:
          m_length1 = m_utils.extractPCMFrame(buffer, m_buffer1);
          break;
        default:
          m_length2 = m_utils.extractPCMFrame(buffer, m_buffer2);
          break;
      }
      break;

    default:
      DEBUG2("Unknown AMBE3003 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE3003Driver::writeAMBE(uint8_t n, const uint8_t* ambe)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!dvsi.ready()) {
    DEBUG1("The AMBE3003 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[50U];
  uint16_t pos = m_utils.createAMBEFrame(n, ambe, out);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(out, pos);

  return 0x00U;
}

uint8_t CAMBE3003Driver::writePCM(uint8_t n, const uint8_t* pcm)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!dvsi.ready()) {
    DEBUG1("The AMBE3003 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[400U];
  uint16_t pos = m_utils.createPCMFrame(n, pcm, out);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(out, pos);

  return 0x00U;
}

AD_STATE CAMBE3003Driver::readAMBE(uint8_t n, uint8_t* ambe)
{
  switch (n) {
    case 0U:
      switch (m_length0) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          m_length0 = 0U;
          return AD_STATE::WRONG_TYPE;

        default:
          ::memcpy(ambe, m_buffer0, m_length0);
          m_length0 = 0U;
          return AD_STATE::DATA;
      }
      break;

    case 1U:
      switch (m_length1) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          m_length1 = 0U;
          return AD_STATE::WRONG_TYPE;

        default:
          ::memcpy(ambe, m_buffer1, m_length1);
          m_length1 = 0U;
          return AD_STATE::DATA;
      }
      break;

    default:
      switch (m_length2) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          m_length2 = 0U;
          return AD_STATE::WRONG_TYPE;

        default:
          ::memcpy(ambe, m_buffer2, m_length2);
          m_length2 = 0U;
          return AD_STATE::DATA;
      }
      break;
  }
}

AD_STATE CAMBE3003Driver::readPCM(uint8_t n, uint8_t* pcm)
{
  switch (n) {
    case 0U:
      switch (m_length0) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          ::memcpy(pcm, m_buffer0, DVSI_PCM_BYTES);
          m_length0 = 0U;
          return AD_STATE::DATA;

        default:
          m_length0 = 0U;
          return AD_STATE::WRONG_TYPE;
      }
      break;

    case 1U:
      switch (m_length1) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          ::memcpy(pcm, m_buffer1, DVSI_PCM_BYTES);
          m_length1 = 0U;
          return AD_STATE::DATA;

        default:
          m_length1 = 0U;
          return AD_STATE::WRONG_TYPE;
      }
      break;

    default:
      switch (m_length2) {
        case 0U:
          return AD_STATE::NO_DATA;

        case DVSI_PCM_BYTES:
          ::memcpy(pcm, m_buffer2, DVSI_PCM_BYTES);
          m_length2 = 0U;
          return AD_STATE::DATA;

        default:
          m_length2 = 0U;
          return AD_STATE::WRONG_TYPE;
      }
      break;
  }
}

void CAMBE3003Driver::drain(uint8_t n)
{
  switch (n) {
    case 0U:
      m_length0 = 0U;
      break;
    case 1U:
      m_length1 = 0U;
      break;
    default:
      m_length2 = 0U;
      break;
  }
}

#endif
