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

    void     startup3000();

    void     reset3000();

    bool     ready3000() const;

    void     write3000(const uint8_t* buffer, uint16_t length);
    uint16_t read3000(uint8_t* buffer);

#if AMBE_TYPE == 3
    void     startup4020();

    void     reset4020();

    bool     RTS4020() const;

    void     write4020(const uint8_t* buffer, uint16_t length);
    uint16_t read4020(uint8_t* buffer);
#endif

  private:
    uint8_t  m_buffer3000[512U];
    uint16_t m_len3000;
    uint16_t m_ptr3000;
#if AMBE_TYPE == 3
    uint8_t  m_buffer4020[512U];
    uint16_t m_len4020;
    uint16_t m_ptr4020;
#endif
};

#endif

#endif

