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

#ifndef	ModeDefines_H
#define	ModeDefines_H

#include <cstdint>

const uint8_t MODE_PASS_THROUGH  = 0x00U;

const uint8_t MODE_DSTAR       = 0x01U;
const uint8_t MODE_DMR_NXDN    = 0x02U;
const uint8_t MODE_YSFDN       = 0x03U;
const uint8_t MODE_IMBE        = 0x04U;
const uint8_t MODE_IMBE_FEC    = 0x05U;
const uint8_t MODE_CODEC2_3200 = 0x06U;
const uint8_t MODE_ACELP       = 0x07U;

const uint8_t MODE_ALAW        = 0xFDU;
const uint8_t MODE_MULAW       = 0xFEU;
const uint8_t MODE_PCM         = 0xFFU;

const uint16_t DSTAR_DATA_LENGTH       = 9U;
const uint16_t DMR_NXDN_DATA_LENGTH    = 9U;
const uint16_t YSFDN_DATA_LENGTH       = 13U;
const uint16_t IMBE_DATA_LENGTH        = 11U;
const uint16_t IMBE_FEC_DATA_LENGTH    = 18U;
const uint16_t CODEC2_3200_DATA_LENGTH = 8U;

const uint16_t ALAW_DATA_LENGTH        = 160U;
const uint16_t MULAW_DATA_LENGTH       = 160U;
const uint16_t PCM_DATA_LENGTH         = 320U;

#endif
