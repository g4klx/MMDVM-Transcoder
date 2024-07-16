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
    CAMBE3003Driver(uint8_t n, IDVSIDriver& dvsi);

    void startup();

    void init(AMBE_MODE mode);

    void process();

    uint8_t writeAMBE(const uint8_t* buffer);

    uint8_t writePCM(const uint8_t* buffer);

    AD_STATE readAMBE(uint8_t* buffer);

    AD_STATE readPCM(uint8_t* buffer);

    void drain();

  private:
    uint8_t        m_n;
    IDVSIDriver&   m_dvsi;
    uint8_t        m_buffer[400U];
    uint16_t       m_length;
    CAMBE3003Utils m_utils;
};

#endif
