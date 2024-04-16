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

#ifndef	DStarPCM_H
#define	DStarPCM_H

#include "Config.h"

#if AMBE_TYPE > 0

#include "Processor.h"

#include "AMBE3000Driver.h"

class CDStarPCM : public IProcessor {
  public:
    CDStarPCM();
    virtual ~CDStarPCM();

    virtual uint8_t init(uint8_t n) override;

    virtual uint8_t input(const uint8_t* buffer, uint16_t length) override;

    virtual int16_t output(uint8_t* buffer) override;

  private:
    CAMBE3000Driver* m_ambe;
};

#endif

#endif
