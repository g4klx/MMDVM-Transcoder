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

const uint8_t DVSI_START_BYTE = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET        = 0x09U;
const uint8_t DVSI_PKT_RATEP        = 0x0AU;
const uint8_t DVSI_PKT_CHANNEL0     = 0x40U;
const uint8_t DVSI_PKT_CHANNEL1     = 0x41U;
const uint8_t DVSI_PKT_CHANNEL2     = 0x42U;

const uint8_t DVSI_PKT_DSTAR_FEC[]         = {DVSI_PKT_RATEP, 0x01U, 0x30U, 0x07U, 0x63U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x48U};
const uint16_t DVSI_PKT_DSTAR_FEC_LEN      = 13U;
const uint8_t DVSI_PKT_DSTAR_FEC_BYTES_LEN = 9U;
const uint8_t DVSI_PKT_DSTAR_FEC_BITS_LEN  = 72U;

const uint8_t DVSI_PKT_MODE33[]         = {DVSI_PKT_RATET, 33U};
const uint16_t DVSI_PKT_MODE33_LEN      = 2U;
const uint8_t DVSI_PKT_MODE33_BYTES_LEN = 9U;
const uint8_t DVSI_PKT_MODE33_BITS_LEN  = 72U;

const uint8_t DVSI_PKT_MODE34[]         = {DVSI_PKT_RATET, 34U};
const uint16_t DVSI_PKT_MODE34_LEN      = 2U;
const uint8_t DVSI_PKT_MODE34_BYTES_LEN = 7U;
const uint8_t DVSI_PKT_MODE34_BITS_LEN  = 49U;

const uint8_t DVSI_AUDIO_HEADER[]    = {DVSI_START_BYTE, 0x01U, 0x42U, DVSI_TYPE_AUDIO, 0x00U, 0xA0U};
const uint16_t DVSI_AUDIO_HEADER_LEN = 6U;

const uint8_t DVSI_AMBE_HEADER[]     = {DVSI_START_BYTE, 0x00U, 0x00U, DVSI_TYPE_AMBE, 0x01U, 0x00U};
const uint16_t DVSI_AMBE_HEADER_LEN  = 6U;

CAMBE3000Driver::CAMBE3000Driver() :
m_buffer(),
m_length(),
m_mode(),
m_bytesLen(),
m_bitsLen()
{
  m_buffer[0U] = new uint8_t[512U];
#if AMBE_TYPE == 2
  m_buffer[1U] = new uint8_t[512U];
  m_buffer[2U] = new uint8_t[512U];
#else
  m_buffer[1U] = NULL;
  m_buffer[2U] = NULL;
#endif

  m_length[0U] = 0U;
  m_length[1U] = 0U;
  m_length[2U] = 0U;

  m_mode[0U] = MODE_NONE;
  m_mode[1U] = MODE_NONE;
  m_mode[2U] = MODE_NONE;

  m_bytesLen[0U] = 0U;
  m_bytesLen[1U] = 0U;
  m_bytesLen[2U] = 0U;

  m_bitsLen[0U] = 0U;
  m_bitsLen[1U] = 0U;
  m_bitsLen[2U] = 0U;
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

  uint8_t  buffer[100U];
  uint16_t length = 0U;

  buffer[length++] = DVSI_START_BYTE;
  buffer[length++] = 0x00U;
  buffer[length++] = 0x00U;
  buffer[length++] = DVSI_TYPE_CONTROL;

#if AMBE_TYPE == 2
  switch (n) {
    case 0U:
      buffer[length++] = DVSI_PKT_CHANNEL0;
      break;
    case 1U:
      buffer[length++] = DVSI_PKT_CHANNEL1;
      break;
    case 2U:
      buffer[length++] = DVSI_PKT_CHANNEL2;
      break;
    default:
      DEBUG2("Unknown value of n received ", n);
      return;
  }
#endif

  switch (mode) {
    case DSTAR_TO_PCM:
    case PCM_TO_DSTAR:
      ::memcpy(buffer + length, DVSI_PKT_DSTAR_FEC, DVSI_PKT_DSTAR_FEC_LEN);
      length += DVSI_PKT_DSTAR_FEC_LEN;
      m_bytesLen[n] = DVSI_PKT_DSTAR_FEC_BYTES_LEN;
      m_bitsLen[n]  = DVSI_PKT_DSTAR_FEC_BITS_LEN;
      break;
    case DMR_NXDN_TO_PCM:
    case PCM_TO_DMR_NXDN:
      ::memcpy(buffer + length, DVSI_PKT_MODE33, DVSI_PKT_MODE33_LEN);
      length += DVSI_PKT_MODE33_LEN;
      m_bytesLen[n] = DVSI_PKT_MODE33_BYTES_LEN;
      m_bitsLen[n]  = DVSI_PKT_MODE33_BITS_LEN;
      break;
    case YSFDN_TO_PCM:
    case PCM_TO_YSFDN:
      ::memcpy(buffer + length, DVSI_PKT_MODE34, DVSI_PKT_MODE34_LEN);
      length += DVSI_PKT_MODE34_LEN;
      m_bytesLen[n] = DVSI_PKT_MODE34_BYTES_LEN;
      m_bitsLen[n]  = DVSI_PKT_MODE34_BITS_LEN;
      break;
    default:
      DEBUG2("Unknown AMBE300/3003 mode received ", mode);
      return;
  }

  buffer[2U] = uint8_t(length - 4U);

#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif

  dvsi.write(buffer, length);

  m_mode[n] = mode;
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
#if AMBE_TYPE == 2
      pos = 6U;
#else
      pos = 5U;
#endif
      ::memcpy(m_buffer[n], buffer + pos + 1U, m_bytesLen[n]);
      m_length[n] = m_bytesLen[n];
      break;

    case DVSI_TYPE_AUDIO:
#if AMBE_TYPE == 2
      pos = 6U;
#else
      pos = 5U;
#endif
      length = buffer[pos] * sizeof(uint16_t);
      swapBytes(m_buffer[n], buffer + pos + 1U, length);
      m_length[n] = length;
      break;

    default:
      DEBUG2("Unknown AMBE3000/3003 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE3000Driver::write(uint8_t n, const uint8_t* buffer, uint16_t length)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!dvsi.ready()) {
    DEBUG1("The AMBE3000/3003 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[500U];
  uint16_t pos = 0U;

  switch (m_mode[n]) {
    case DSTAR_TO_PCM:
    case YSFDN_TO_PCM:
    case DMR_NXDN_TO_PCM:
      out[pos++] = DVSI_START_BYTE;
      out[pos++] = 0x00U;
      out[pos++] = 0x00U;
      out[pos++] = DVSI_TYPE_AMBE;

#if AMBE_TYPE == 2
      switch (n) {
        case 0U:
          out[pos++] = DVSI_PKT_CHANNEL0;
          break;
        case 1U:
          out[pos++] = DVSI_PKT_CHANNEL1;
          break;
        case 2U:
          out[pos++] = DVSI_PKT_CHANNEL2;
          break;
        default:
          DEBUG2("Unknown value of n received ", n);
          return;
      }
#endif

      out[pos++] = 0x01U;
      out[pos++] = m_bitsLen[n];

      ::memcpy(out + pos, buffer, m_bytesLen[n]);
      pos += m_bytesLen[n];

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

#if defined(HAS_LEDS)
      leds.setLED1(true);
#endif
      dvsi.write(out, pos);
      break;

    case PCM_TO_DSTAR:
    case PCM_TO_YSFDN:
    case PCM_TO_DMR_NXDN:
      out[pos++] = DVSI_START_BYTE;
      out[pos++] = 0x00U;
      out[pos++] = 0x00U;
      out[pos++] = DVSI_TYPE_AUDIO;

#if AMBE_TYPE == 2
      switch (n) {
        case 0U:
          out[pos++] = DVSI_PKT_CHANNEL0;
          break;
        case 1U:
          out[pos++] = DVSI_PKT_CHANNEL1;
          break;
        case 2U:
          out[pos++] = DVSI_PKT_CHANNEL2;
          break;
        default:
          DEBUG2("Unknown value of n received ", n);
          return;
      }
#endif

      out[pos++] = 0x00U;
      out[pos++] = length / sizeof(int16_t);

      swapBytes(out + pos, buffer, length);
      pos += length;

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

#if defined(HAS_LEDS)
      leds.setLED1(true);
#endif
      dvsi.write(out, pos);
      break;
  }

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

void CAMBE3000Driver::swapBytes(uint8_t* out, const uint8_t* in, uint16_t length) const
{
  for (uint16_t i = 0U; i < length; i += 2U) {
    out[i + 0U] = in[i + 1U];
    out[i + 1U] = in[i + 0U];
  }
}

#endif
