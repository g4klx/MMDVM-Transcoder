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

#ifndef	AMBE3003Driver_H
#define	AMBE3003Driver_H

#include "Config.h"

#if AMBE_TYPE == 3

#include "AMBE3003Utils.h"
#include "DVSIDriver.h"

#include <cstdint>

enum AD_STATE {
  ADS_NO_DATA,
  ADS_WRONG_TYPE,
  ADS_DATA
};

class CAMBE3003Driver {
  public:
    CAMBE3003Driver();

    void startup();

    void init(uint8_t n, AMBE_MODE mode);

    void process();

    uint8_t writeAMBE(uint8_t n, const uint8_t* buffer);

    uint8_t writePCM(uint8_t n, const uint8_t* buffer);

    AD_STATE readAMBE(uint8_t n, uint8_t* buffer);

    AD_STATE readPCM(uint8_t n, uint8_t* buffer);

    void drain(uint8_t n);

  private:
    uint8_t        m_buffer0[400U];
    uint8_t        m_buffer1[400U];
    uint8_t        m_buffer2[400U];
    uint16_t       m_length0;
    uint16_t       m_length1;
    uint16_t       m_length2;
    CAMBE3003Utils m_utils;
};

#endif

#endif
