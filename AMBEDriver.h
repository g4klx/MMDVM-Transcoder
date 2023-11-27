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

enum AMBE_MODE {
  MODE_NONE,
  DSTAR_TO_PCM,
  DMR_NXDN_TO_PCM,
  PCM_TO_DSTAR,
  PCM_TO_DMR_NXDN
};

enum AMBE_STATE {
  AS_NONE
};

class CAMBEDriver {
  public:
    CAMBEDriver();

    void init(uint8_t n, AMBE_MODE mode);

    void write(uint8_t n, const uint8_t* buffer, uint16_t length);

    bool read(uint8_t n, uint8_t* buffer);

  private:
    uint8_t    m_n;
    AMBE_MODE  m_mode;
    AMBE_STATE m_state;

    void setN(uint8_t n);
};

#endif

