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

#ifndef	DVSIDriver1_H
#define	DVSIDriver1_H

#include "Config.h"

#if AMBE_TYPE > 0

#include "DVSIDriver.h"

#include <Arduino.h>

#include <cstdint>

class CDVSIDriver1 : public IDVSIDriver {
  public:
    CDVSIDriver1();
    virtual ~CDVSIDriver1();

    virtual void     startup();

    virtual void     reset();

    virtual bool     ready() const;

    virtual void     write(const uint8_t* buffer, uint16_t length);

    virtual uint16_t read(uint8_t* buffer);

  private:
    HardwareSerial m_serial;
    uint8_t        m_buffer[512U];
    uint16_t       m_len;
    uint16_t       m_ptr;
};

#endif

#endif
