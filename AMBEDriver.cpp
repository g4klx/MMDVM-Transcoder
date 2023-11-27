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

const uint8_t DVSI_START_BYTE = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET        = 0x09U;
const uint8_t DVSI_PKT_RATEP        = 0x0AU;
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

const uint8_t DVSI_REQ_CHANNEL0[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_CHANNEL0};
const uint16_t DVSI_REQ_CHANNEL0_LEN = 6U;

const uint8_t DVSI_REQ_CHANNEL1[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_CHANNEL1};
const uint16_t DVSI_REQ_CHANNEL1_LEN = 6U;

const uint8_t DVSI_REQ_CHANNEL2[]    = {DVSI_START_BYTE, 0x00U, 0x01U, DVSI_TYPE_CONTROL, DVSI_PKT_CHANNEL2};
const uint16_t DVSI_REQ_CHANNEL2_LEN = 6U;

const uint8_t DVSI_REQ_DSTAR_FEC[]    = {DVSI_START_BYTE, 0x00U, 0x0DU, DVSI_TYPE_CONTROL, DVSI_PKT_RATEP, 0x01U, 0x30U, 0x07U, 0x63U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x48U};
const uint16_t DVSI_REQ_DSTAR_FEC_LEN = 17U;

const uint8_t DVSI_REQ_DMR_NXDN_FEC[]    = {DVSI_START_BYTE, 0x00U, 0x02U, DVSI_TYPE_CONTROL, DVSI_PKT_RATET, 33U};
const uint16_t DVSI_REQ_DMR_NXDN_FEC_LEN = 6U;

const uint8_t DVSI_AUDIO_HEADER[]    = {DVSI_START_BYTE, 0x01U, 0x42U, DVSI_TYPE_AUDIO, 0x00U, 0xA0U};
const uint16_t DVSI_AUDIO_HEADER_LEN = 6U;

const uint8_t DVSI_AMBE_HEADER[]     = {DVSI_START_BYTE, 0x00U, 0x00U, DVSI_TYPE_AMBE, 0x01U, 0x00U};
const uint16_t DVSI_AMBE_HEADER_LEN  = 6U;

CAMBEDriver::CAMBEDriver() :
m_n(0U),
m_mode(MODE_NONE),
m_state(AS_NONE)
{
}

void CAMBEDriver::init(uint8_t n, AMBE_MODE mode)
{
#if AMBE_TYPE > 1
  if (n != m_n)
    setN(n);
#endif

  m_mode = mode;

  switch (m_mode) {
    case DSTAR_TO_PCM:
    case PCM_TO_DSTAR:
      dvsi.write(DVSI_REQ_DSTAR_FEC, DVSI_REQ_DSTAR_FEC_LEN);
      break;
    case DMR_NXDN_TO_PCM:
    case PCM_TO_DMR_NXDN:
      dvsi.write(DVSI_REQ_DMR_NXDN_FEC, DVSI_REQ_DMR_NXDN_FEC_LEN);
      break;
  }
}

void CAMBEDriver::write(uint8_t n, const uint8_t* buffer, uint16_t length)
{
#if AMBE_TYPE > 1
  if (n != m_n)
    setN(n);
#endif

  switch (m_mode) {
    case DSTAR_TO_PCM:
      dvsi.write(DVSI_AMBE_HEADER, DVSI_AMBE_HEADER_LEN);
      break;
    case PCM_TO_DSTAR:
      dvsi.write(DVSI_AUDIO_HEADER, DVSI_AUDIO_HEADER_LEN);
      dvsi.write(buffer, length);
      break;
    case DMR_NXDN_TO_PCM:
      dvsi.write(DVSI_AMBE_HEADER, DVSI_AMBE_HEADER_LEN);
      break;
    case PCM_TO_DMR_NXDN:
      dvsi.write(DVSI_AUDIO_HEADER, DVSI_AUDIO_HEADER_LEN);
      dvsi.write(buffer, length);
      break;
  }
}

bool CAMBEDriver::read(uint8_t n, uint8_t* buffer)
{
  return false;
}

