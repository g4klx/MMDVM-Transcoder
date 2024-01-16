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

#ifndef	AMBE4020Driver_H
#define	AMBE4020Driver_H

#include <cstdint>

#include "AMBE3000Driver.h"
#include "Config.h"

// Define this on little-endian architectures
#define	SWAP_BYTES

class CAMBE4020Driver {
  public:
    CAMBE4020Driver();

    void startup();

    void init(AMBE_MODE mode);

    void process();

    uint8_t write(const uint8_t* buffer, uint16_t length);

    bool read(uint8_t* buffer);

  private:
    uint8_t    m_buffer[500U];
    uint16_t   m_length;
    AMBE_MODE  m_mode;

#if defined(SWAP_BYTES)
    void swapBytes(uint8_t* out, const uint8_t* in, uint16_t length) const;
#endif
};

#endif

