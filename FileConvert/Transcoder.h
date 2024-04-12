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

#ifndef	Transcoder_H
#define	Transcoder_H

#include <cstdint>

const uint8_t MODE_PASS_THROUGH  = 0x00U;

const uint8_t MODE_DSTAR       = 0x01U;
const uint8_t MODE_DMR_NXDN    = 0x02U;
const uint8_t MODE_YSFDN       = 0x03U;
const uint8_t MODE_IMBE        = 0x04U;
const uint8_t MODE_IMBE_FEC    = 0x05U;
const uint8_t MODE_CODEC2_3200 = 0x06U;

const uint8_t MODE_PCM         = 0xFFU;

const uint16_t DSTAR_DATA_LENGTH       = 9U;
const uint16_t DMR_NXDN_DATA_LENGTH    = 9U;
const uint16_t YSFDN_DATA_LENGTH       = 13U;
const uint16_t IMBE_DATA_LENGTH        = 11U;
const uint16_t IMBE_FEC_DATA_LENGTH    = 18U;
const uint16_t CODEC2_3200_DATA_LENGTH = 8U;

const uint16_t PCM_DATA_LENGTH         = 320U;

const unsigned int TRANSCODER_SPEED = 460800U;

const uint8_t MARKER = 0xE1U;

const uint8_t TYPE_GET_VERSION      = 0x00U;
const uint8_t TYPE_GET_CAPABILITIES = 0x01U;
const uint8_t TYPE_SET_MODE         = 0x02U;
const uint8_t TYPE_ACK              = 0x03U;
const uint8_t TYPE_NAK              = 0x04U;
const uint8_t TYPE_DATA             = 0x05U;

const uint8_t  GET_VERSION[]   = { MARKER, 0x04U, 0x00U, TYPE_GET_VERSION };
const uint16_t GET_VERSION_LEN = 4U;

const uint8_t  GET_CAPABILITIES[]   = { MARKER, 0x04U, 0x00U, TYPE_GET_CAPABILITIES };
const uint16_t GET_CAPABILITIES_LEN = 4U;

const uint8_t  SET_MODE_HEADER[] = { MARKER, 0x06U, 0x00U, TYPE_SET_MODE };
const uint16_t SET_MODE_LEN      = 6U;

const uint8_t  DSTAR_DATA_HEADER[] = { MARKER, 0x0DU, 0x00U, TYPE_DATA };
const uint16_t DSTAR_DATA_LEN      = DSTAR_DATA_LENGTH + 4U;

const uint8_t  DMR_DATA_HEADER[] = { MARKER, 0x0DU, 0x00U, TYPE_DATA };
const uint16_t DMR_DATA_LEN      = DMR_NXDN_DATA_LENGTH + 4U;

const uint8_t  YSFDN_DATA_HEADER[] = { MARKER, 0x11U, 0x00U, TYPE_DATA };
const uint16_t YSFDN_DATA_LEN      = YSFDN_DATA_LENGTH + 4U;

const uint8_t  IMBE_DATA_HEADER[] = { MARKER, 0x0FU, 0x00U, TYPE_DATA };
const uint16_t IMBE_DATA_LEN      = IMBE_DATA_LENGTH + 4U;

const uint8_t  IMBE_FEC_DATA_HEADER[] = { MARKER, 0x16U, 0x00U, TYPE_DATA };
const uint16_t IMBE_FEC_DATA_LEN      = IMBE_FEC_DATA_LENGTH + 4U;

const uint8_t  CODEC2_3200_DATA_HEADER[] = { MARKER, 0x0CU, 0x00U, TYPE_DATA };
const uint16_t CODEC2_3200_DATA_LEN      = CODEC2_3200_DATA_LENGTH + 4U;

const uint8_t  PCM_DATA_HEADER[] = { MARKER, 0x44U, 0x01U, TYPE_DATA };
const uint16_t PCM_DATA_LEN      = PCM_DATA_LENGTH + 4U;

const uint16_t MARKER_POS     = 0U;
const uint16_t LENGTH_LSB_POS = 1U;
const uint16_t LENGTH_MSB_POS = 2U;
const uint16_t TYPE_POS       = 3U;
const uint16_t DATA_START_POS = 4U;

// Get Version layout and data
const uint8_t GET_VERSION_PROTOCOL_POS = 4U;
const uint8_t GET_VERSION_HARWARE_POS  = 5U;
const uint8_t PROTOCOL_VERSION = 1U;

// Get Capabilities layout and data
const uint8_t GET_CAPABILITIES_AMBE_TYPE_POS = 4U;
const uint8_t NO_AMBE_CHIP  = 0U;
const uint8_t HAS_AMBE_CHIP = 1U;

// NAK layout
const uint8_t NAK_ERROR_POS = 4U;

#endif
