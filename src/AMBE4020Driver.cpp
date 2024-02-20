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

#include "AMBE4020Driver.h"

#if AMBE_TYPE == 3

#include "Globals.h"
#include "Debug.h"

const uint8_t DVSI_START_BYTE = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET        = 0x09U;
const uint8_t DVSI_PKT_INIT         = 0x0BU;
const uint8_t DVSI_PKT_PRODID       = 0x30U;
const uint8_t DVSI_PKT_VERSTRING    = 0x31U;
const uint8_t DVSI_PKT_RESETSOFTCFG = 0x34U;
const uint8_t DVSI_PKT_READY        = 0x39U;

const uint8_t DVSI_REQ_PRODID[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_PRODID};
const uint16_t DVSI_REQ_PRODID_LEN = 5U;

const uint8_t DVSI_REQ_VERSTRING[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_VERSTRING};
const uint16_t DVSI_REQ_VERSTRING_LEN = 5U;

const uint8_t DVSI_REQ_RESET[]    = {DVSI_START_BYTE, 0x00U, 0x07U, DVSI_TYPE_CONTROL, DVSI_PKT_RESETSOFTCFG, 0x05U, 0x00U, 0x00U, 0x0FU, 0x00U, 0x00U};
const uint16_t DVSI_REQ_RESET_LEN = 11U;

const uint8_t DVSI_PKT_DSTAR_FEC[]    = {DVSI_PKT_RATET, 63U};
const uint16_t DVSI_PKT_DSTAR_FEC_LEN = 2U;

const uint8_t DVSI_AUDIO_HEADER[]    = {DVSI_START_BYTE, 0x01U, 0x42U, DVSI_TYPE_AUDIO, 0x00U, 0xA0U};
const uint16_t DVSI_AUDIO_HEADER_LEN = 6U;

const uint8_t DVSI_AMBE_HEADER[]     = {DVSI_START_BYTE, 0x00U, 0x00U, DVSI_TYPE_AMBE, 0x01U, 0x00U};
const uint16_t DVSI_AMBE_HEADER_LEN  = 6U;

CAMBE4020Driver::CAMBE4020Driver() :
m_buffer(),
m_length(0U),
m_mode(MODE_NONE)
{
}

void CAMBE4020Driver::startup()
{
  dvsi.reset4020();
}

void CAMBE4020Driver::init(AMBE_MODE mode)
{
  uint8_t  buffer[100U];
  uint16_t length = 0U;

  buffer[length++] = DVSI_START_BYTE;
  buffer[length++] = 0x00U;
  buffer[length++] = 0x00U;
  buffer[length++] = DVSI_TYPE_CONTROL;

  switch (mode) {
    case DSTAR_TO_PCM:
    case PCM_TO_DSTAR:
      ::memcpy(buffer + length, DVSI_PKT_DSTAR_FEC, DVSI_PKT_DSTAR_FEC_LEN);
      length += DVSI_PKT_DSTAR_FEC_LEN;
      break;
    default:
      DEBUG2("Unknown AMBE4020 mode received ", mode);
      return;
  }

  buffer[length++] = DVSI_PKT_INIT;
  buffer[length++] = 0x03U;

  buffer[2U] = uint8_t(length - 4U);

  dvsi.write4020(buffer, length);

  m_mode = mode;
}

void CAMBE4020Driver::process()
{
  uint8_t buffer[500U];
  uint16_t length = dvsi.read4020(buffer);
  if (length == 0U)
    return;

  uint16_t pos = 0U;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 4U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_INIT:
            if (buffer[pos + 1U] != 0x00U)
              DEBUG2("Response to AMBE4020 PKT_INIT is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATET:
            if (buffer[pos + 1U] != 0x00U)
              DEBUG2("Response to AMBE4020 PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_READY:
            pos += 1U;
            break;

          default:
            DEBUG2("Unknown AMBE4020 control type response of ", buffer[pos]);
            return;
        }
      }
      break;

    case DVSI_TYPE_AMBE:
      pos = 5U;
      length = buffer[pos] / 8U;
      ::memcpy(m_buffer, buffer + pos + 1U, length);
      m_length = length;
      break;

    case DVSI_TYPE_AUDIO:
      pos = 5U;
      length = buffer[pos] * sizeof(uint16_t);
#if defined(SWAP_BYTES)
      swapBytes(m_buffer, buffer + pos + 1U, length);
#else
      ::memcpy(m_buffer, buffer + pos + 1U, length);
#endif
      m_length = length;
      break;

    default:
      DEBUG2("Unknown AMBE4020 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE4020Driver::write(const uint8_t* buffer, uint16_t length)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (dvsi.RTS4020()) {
    DEBUG1("The AMBE4020 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[500U];
  uint16_t pos = 0U;

  switch (m_mode) {
    case DSTAR_TO_PCM:
      out[pos++] = DVSI_START_BYTE;
      out[pos++] = 0x00U;
      out[pos++] = 0x00U;
      out[pos++] = DVSI_TYPE_AMBE;

      out[pos++] = 0x01U;
      out[pos++] = length * 8U;

      ::memcpy(out + pos, buffer, length);
      pos += length;

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

      dvsi.write4020(out, pos);
      break;

    case PCM_TO_DSTAR:
      out[pos++] = DVSI_START_BYTE;
      out[pos++] = 0x00U;
      out[pos++] = 0x00U;
      out[pos++] = DVSI_TYPE_AUDIO;

      out[pos++] = 0x00U;
      out[pos++] = length / sizeof(int16_t);

#if defined(SWAP_BYTES)
      swapBytes(out + pos, buffer, length);
#else
      ::memcpy(out + pos, buffer, length);
#endif
      pos += length;

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

      dvsi.write4020(out, pos);
      break;
  }

  return 0x00U;
}

bool CAMBE4020Driver::read(uint8_t* buffer)
{
  if (m_length > 0U) {
    ::memcpy(buffer, m_buffer, m_length);
    m_length = 0U;
    return true;
  }

  return false;
}

#if defined(SWAP_BYTES)
void CAMBE4020Driver::swapBytes(uint8_t* out, const uint8_t* in, uint16_t length) const
{
  for (uint16_t i = 0U; i < length; i += 2U) {
    out[i + 0U] = in[i + 1U];
    out[i + 1U] = in[i + 0U];
  }
}
#endif

#endif
