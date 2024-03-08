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

#include "AMBE3000Utils.h"

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

const uint16_t DVSI_PCM_SAMPLES = 160U;
const uint16_t DVSI_PCM_BYTES   = DVSI_PCM_SAMPLES * sizeof(int16_t);

CAMBE3000Utils::CAMBE3000Utils() :
m_mode(MODE_NONE),
m_bytesLen(0U),
m_bitsLen(0U)
{
}

uint16_t CAMBE3000Utils::createModeChange(uint8_t n, AMBE_MODE mode, uint8_t* buffer)
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

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
      m_bytesLen = DVSI_PKT_DSTAR_FEC_BYTES_LEN;
      m_bitsLen  = DVSI_PKT_DSTAR_FEC_BITS_LEN;
      break;
    case DMR_NXDN_TO_PCM:
    case PCM_TO_DMR_NXDN:
      ::memcpy(buffer + length, DVSI_PKT_MODE33, DVSI_PKT_MODE33_LEN);
      length += DVSI_PKT_MODE33_LEN;
      m_bytesLen = DVSI_PKT_MODE33_BYTES_LEN;
      m_bitsLen  = DVSI_PKT_MODE33_BITS_LEN;
      break;
    case YSFDN_TO_PCM:
    case PCM_TO_YSFDN:
      ::memcpy(buffer + length, DVSI_PKT_MODE34, DVSI_PKT_MODE34_LEN);
      length += DVSI_PKT_MODE34_LEN;
      m_bytesLen = DVSI_PKT_MODE34_BYTES_LEN;
      m_bitsLen  = DVSI_PKT_MODE34_BITS_LEN;
      break;
    default:
      return 0U;
  }

  buffer[2U] = uint8_t(length - 4U);

  m_mode = mode;

  return length;
}

uint16_t CAMBE3000Utils::createAMBEFrame(uint8_t n, const uint8_t* buffer, uint8_t* out) const
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  uint16_t pos = 0U;

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
  out[pos++] = m_bitsLen;

  ::memcpy(out + pos, buffer, m_bytesLen);
  pos += m_bytesLen;

  out[1U] = (pos - 4U) / 256U;
  out[2U] = (pos - 4U) % 256U;

  return pos;
}

uint16_t CAMBE3000Utils::createPCMFrame(uint8_t n, const uint8_t* buffer, uint8_t* out) const
{
#if AMBE_TYPE == 1
  n = 0U;
#endif

  uint16_t pos = 0U;

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
  out[pos++] = DVSI_PCM_SAMPLES;

  swapBytes(out + pos, buffer, DVSI_PCM_BYTES);
  pos += DVSI_PCM_BYTES;

  out[1U] = (pos - 4U) / 256U;
  out[2U] = (pos - 4U) % 256U;

  return pos;
}

uint16_t CAMBE3000Utils::extractAMBEFrame(const uint8_t* frame, uint8_t* data) const
{
#if AMBE_TYPE == 2
  uint16_t pos = 6U;
#else
  uint16_t pos = 5U;
#endif

  ::memcpy(data, frame + pos + 1U, m_bytesLen);

  return m_bytesLen;
}

uint16_t CAMBE3000Utils::extractPCMFrame(const uint8_t* frame, uint8_t* data) const
{
#if AMBE_TYPE == 2
  uint16_t pos = 6U;
#else
  uint16_t pos = 5U;
#endif

  swapBytes(data, frame + pos + 1U, DVSI_PCM_BYTES);

  return DVSI_PCM_BYTES;
}

void CAMBE3000Utils::swapBytes(uint8_t* out, const uint8_t* in, uint16_t length) const
{
  for (uint16_t i = 0U; i < length; i += 2U) {
    out[i + 0U] = in[i + 1U];
    out[i + 1U] = in[i + 0U];
  }
}

#endif
