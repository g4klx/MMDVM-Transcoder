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

#ifndef	PCMCodec21600_H
#define	PCMCodec21600_H

#include "Processor.h"

class CPCMCodec21600 : public IProcessor {
  public:
    CPCMCodec21600();
    virtual ~CPCMCodec21600();

    virtual uint8_t  input(const uint8_t* buffer, uint16_t length);

    virtual uint16_t output(uint8_t* buffer);

  private:
};

#endif

