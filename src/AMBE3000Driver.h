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

#ifndef	AMBE3000Driver_H
#define	AMBE3000Driver_H

#include "Config.h"

#if AMBE_TYPE > 0

#include "AMBE3000Utils.h"

#include <cstdint>

enum AD_STATE {
  ADS_NO_DATA,
  ADS_WRONG_TYPE,
  ADS_DATA
};

class CAMBE3000Driver {
  public:
    CAMBE3000Driver();

    void startup();

    void init(uint8_t n, AMBE_MODE mode);

    void process();

    uint8_t writeAMBE(uint8_t n, const uint8_t* buffer, const uint8_t* frame = nullptr, uint16_t len = 0U);

    uint8_t writePCM(uint8_t n, const uint8_t* buffer, const uint8_t* frame = nullptr, uint16_t len = 0U);

    AD_STATE readAMBE(uint8_t n, uint8_t* buffer);

    AD_STATE readPCM(uint8_t n, uint8_t* buffer);

    void drain(uint8_t n);

  private:
#if AMBE_TYPE == 2
    uint8_t*       m_buffer[3U];
    uint16_t       m_length[3U];
    CAMBE3000Utils m_utils[3U];
#else
    uint8_t*       m_buffer[1U];
    uint16_t       m_length[1U];
    CAMBE3000Utils m_utils[1U];
#endif
};

#endif

#endif
