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

#ifndef	AMBE3003Utils_H
#define	AMBE3003Utils_H

#include "Config.h"

#include <cstdint>

enum AMBE_MODE {
  MODE_NONE,
  DSTAR_TO_PCM,
  YSFDN_TO_PCM,
  DMR_NXDN_TO_PCM,
  PCM_TO_DSTAR,
  PCM_TO_YSFDN,
  PCM_TO_DMR_NXDN
};

class CAMBE3003Utils {
  public:
    CAMBE3003Utils();

    uint16_t createModeChange(AMBE_MODE mode, uint8_t* buffer);
    uint16_t createAMBEFrame(const uint8_t* buffer, uint8_t* out) const;
    uint16_t createPCMFrame(const uint8_t* buffer, uint8_t* out) const;

    uint16_t extractAMBEFrame(const uint8_t* buffer, uint8_t* data) const;
    uint16_t extractPCMFrame(const uint8_t* buffer, uint8_t* data) const;

  private:
    AMBE_MODE m_mode;
    uint8_t   m_bytesLen;
    uint8_t   m_bitsLen;

    void swapBytes(uint8_t* out, const uint8_t* in, uint16_t length) const;
};

#endif
