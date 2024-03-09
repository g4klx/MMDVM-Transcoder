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

#ifndef	DVSIDriver_H
#define	DVSIDriver_H

#include "Config.h"

#if AMBE_TYPE > 0

#include <cstdint>

class CDVSIDriver {
  public:
    CDVSIDriver();

    void     startup();

    void     reset();

    bool     ready() const;

    void     write(const uint8_t* buffer, uint16_t length, const uint8_t* frame = nullptr, uint16_t len = 0U);

    uint16_t read(uint8_t* buffer);

  private:
    uint8_t  m_buffer[512U];
    uint16_t m_len;
    uint16_t m_ptr;
    uint8_t  m_frame[512U];
    uint16_t m_length;
};

#endif

#endif

