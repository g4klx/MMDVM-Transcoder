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

#include "AMBEDriver.h"

#include "Globals.h"
#include "Debug.h"

const uint8_t DVSI_START_BYTE = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET        = 0x09U;
const uint8_t DVSI_PKT_RATEP        = 0x0AU;
const uint8_t DVSI_PKT_INIT         = 0x0BU;
const uint8_t DVSI_PKT_PRODID       = 0x30U;
const uint8_t DVSI_PKT_VERSTRING    = 0x31U;
const uint8_t DVSI_PKT_RESETSOFTCFG = 0x34U;
const uint8_t DVSI_PKT_READY        = 0x39U;
const uint8_t DVSI_PKT_CHANNEL0     = 0x40U;
const uint8_t DVSI_PKT_CHANNEL1     = 0x41U;
const uint8_t DVSI_PKT_CHANNEL2     = 0x42U;

const uint8_t DVSI_REQ_PRODID[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_PRODID};
const uint16_t DVSI_REQ_PRODID_LEN = 5U;

const uint8_t DVSI_REQ_VERSTRING[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_VERSTRING};
const uint16_t DVSI_REQ_VERSTRING_LEN = 5U;

const uint8_t DVSI_REQ_RESET[]    = {DVSI_START_BYTE, 0x00U, 0x07U, DVSI_TYPE_CONTROL, DVSI_PKT_RESETSOFTCFG, 0x05U, 0x00U, 0x00U, 0x0FU, 0x00U, 0x00U};
const uint16_t DVSI_REQ_RESET_LEN = 11U;

const uint8_t DVSI_PKT_DSTAR_FEC[]    = {DVSI_PKT_RATEP, 0x01U, 0x30U, 0x07U, 0x63U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x48U};
const uint16_t DVSI_PKT_DSTAR_FEC_LEN = 13U;

const uint8_t DVSI_PKT_DMR_NXDN_FEC[]    = {DVSI_PKT_RATET, 33U};
const uint16_t DVSI_PKT_DMR_NXDN_FEC_LEN = 2U;

const uint8_t DVSI_AUDIO_HEADER[]    = {DVSI_START_BYTE, 0x01U, 0x42U, DVSI_TYPE_AUDIO, 0x00U, 0xA0U};
const uint16_t DVSI_AUDIO_HEADER_LEN = 6U;

const uint8_t DVSI_AMBE_HEADER[]     = {DVSI_START_BYTE, 0x00U, 0x00U, DVSI_TYPE_AMBE, 0x01U, 0x00U};
const uint16_t DVSI_AMBE_HEADER_LEN  = 6U;

CAMBEDriver::CAMBEDriver() :
m_buffer0(),
m_length0(0U),
#if AMBE_TYPE == 2
m_buffer1(),
m_length1(0U),
m_buffer2(),
m_length2(0U),
#endif
m_mode(MODE_NONE)
{
}

void CAMBEDriver::startup()
{
  dvsi.reset();
}

void CAMBEDriver::init(uint8_t n, AMBE_MODE mode)
{
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
      break;
    case DMR_NXDN_TO_PCM:
    case PCM_TO_DMR_NXDN:
      ::memcpy(buffer + length, DVSI_PKT_DMR_NXDN_FEC, DVSI_PKT_DMR_NXDN_FEC_LEN);
      length += DVSI_PKT_DMR_NXDN_FEC_LEN;
      break;
    default:
      DEBUG2("Unknown mode received ", mode);
      return;
  }

  buffer[length++] = DVSI_PKT_INIT;
  buffer[length++] = 0x03U;

  buffer[2U] = uint8_t(length - 4U);

  dvsi.write(buffer, length);

  m_mode = mode;
}

void CAMBEDriver::process()
{
  uint8_t buffer[500U];
  uint16_t length = dvsi.read(buffer);
  if (length == 0U)
    return;

  uint16_t pos = 0U;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 4U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_CHANNEL0:
          case DVSI_PKT_CHANNEL1:
          case DVSI_PKT_CHANNEL2:
            DEBUG2("Response to PKT_CHANNELn ", buffer[pos] - 0x40U);
            pos += 2U;
            break;

          case DVSI_PKT_INIT:
            if (buffer[pos + 1U] != 0x00U)
              DEBUG2("Response to PKT_INIT is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATET:
            if (buffer[pos + 1U] != 0x00U)
              DEBUG2("Response to PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATEP:
            if (buffer[pos + 1U] != 0x00U)
              DEBUG2("Response to PKT_RATEP is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_READY:
            pos += 1U;
            break;

          default:
            DEBUG2("Unknown control type response of ", buffer[pos]);
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
      length = buffer[pos] / 8U;
#if AMBE_TYPE == 2
      switch (buffer[4U]) {
        case DVSI_PKT_CHANNEL0:
#endif
          ::memcpy(m_buffer0, buffer + pos + 1U, length);
          m_length0 = length;
#if AMBE_TYPE == 2
          break;
        case DVSI_PKT_CHANNEL1:
          ::memcpy(m_buffer1, buffer + pos + 1U, length);
          m_length1 = length;
          break;
        case DVSI_PKT_CHANNEL2:
          ::memcpy(m_buffer2, buffer + pos + 1U, length);
          m_length2 = length;
          break;
        default:
          DEBUG2("Unknown channel id of ", buffer[4U]);
          break;
      }
#endif
      break;

    case DVSI_TYPE_AUDIO:
#if AMBE_TYPE == 2
      pos = 6U;
#else
      pos = 5U;
#endif
      length = buffer[pos] * sizeof(uint16_t);
#if AMBE_TYPE == 2
      switch (buffer[4U]) {
        case DVSI_PKT_CHANNEL0:
#endif
          ::memcpy(m_buffer0, buffer + pos + 1U, length);
          m_length0 = length;
#if AMBE_TYPE == 2
          break;
        case DVSI_PKT_CHANNEL1:
          ::memcpy(m_buffer1, buffer + pos + 1U, length);
          m_length1 = length;
          break;
        case DVSI_PKT_CHANNEL2:
          ::memcpy(m_buffer2, buffer + pos + 1U, length);
          m_length2 = length;
          break;
        default:
          DEBUG2("Unknown channel id of ", buffer[4U]);
          break;
      }
#endif
      break;

    default:
      DEBUG2("Unknown type from the AMBE chip ", buffer[3U]);
      break;
  }
}

void CAMBEDriver::write(uint8_t n, const uint8_t* buffer, uint16_t length)
{
  uint8_t out[500U];
  uint16_t pos = 0U;

  switch (m_mode) {
    case DSTAR_TO_PCM:
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
      out[pos++] = length * 8U;

      ::memcpy(out + pos, buffer, length);
      pos += length;

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

      dvsi.write(out, pos);
      break;

    case PCM_TO_DSTAR:
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

      ::memcpy(out + pos, buffer, length);
      pos += length;

      out[1U] = (pos - 4U) / 256U;
      out[2U] = (pos - 4U) % 256U;

      dvsi.write(out, pos);
      break;
  }
}

bool CAMBEDriver::read(uint8_t n, uint8_t* buffer)
{
  switch (n) {
    case 0U:
#if AMBE_TYPE == 1
    case 1U:
    case 2U:
#endif
      if (m_length0 > 0U) {
        ::memcpy(buffer, m_buffer0, m_length0);
        m_length0 = 0U;
        return true;
      }
      break;

#if AMBE_TYPE == 2
    case 1U:
      if (m_length1 > 0U) {
        ::memcpy(buffer, m_buffer1, m_length1);
        m_length1 = 0U;
        return true;
      }
      break;

    case 2U:
      if (m_length2 > 0U) {
        ::memcpy(buffer, m_buffer2, m_length2);
        m_length2 = 0U;
        return true;
      }
      break;
#endif

    default:
      DEBUG2("Unknown value of n received ", n);
      break;
  }

  return false;
}

