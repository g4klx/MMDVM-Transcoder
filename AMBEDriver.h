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

#ifndef	AMBEDriver_H
#define	AMBEDriver_H

#include <cstdint>

#include "Config.h"

enum AMBE_MODE {
  MODE_NONE,
  DSTAR_TO_PCM,
  DMR_NXDN_TO_PCM,
  PCM_TO_DSTAR,
  PCM_TO_DMR_NXDN
};

class CAMBEDriver {
  public:
    CAMBEDriver();

    void startup();

    void init(uint8_t n, AMBE_MODE mode);

    void process();

    void write(uint8_t n, const uint8_t* buffer, uint16_t length);

    bool read(uint8_t n, uint8_t* buffer);

  private:
    uint8_t    m_buffer0[500U];
    uint16_t   m_length0;
#if AMBE_TYPE == 2
    uint8_t    m_buffer1[500U];
    uint16_t   m_length1;
    uint8_t    m_buffer2[500U];
    uint16_t   m_length2;
#endif
    AMBE_MODE  m_mode;
};

#endif

