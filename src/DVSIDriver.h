/*
 *   Copyright (C) 2024 by Jonathan Naylor G4KLX
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

#include <cstdint>

class IDVSIDriver {
  public:
    virtual ~IDVSIDriver() = 0;

    virtual void     startup() = 0;

    virtual void     reset() = 0;

    virtual bool     ready() const = 0;

    virtual void     write(const uint8_t* buffer, uint16_t length) = 0;

    virtual uint16_t read(uint8_t* buffer) = 0;

  private:
};

#endif
