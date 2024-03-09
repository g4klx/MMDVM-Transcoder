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

#ifndef	YSFDNDStar_H
#define	YSFDNDStar_H

#include "Processor.h"

#include "AMBE3000Utils.h"

enum YDND_STATE {
  YDNDS_NONE,
  YDNDS_STATE1,
  YDNDS_STATE2
};

class CYSFDNDStar : public IProcessor {
  public:
    CYSFDNDStar();
    virtual ~CYSFDNDStar();

    virtual void     init(uint8_t n) override;

    virtual uint8_t  input(const uint8_t* buffer, uint16_t length) override;

    virtual void     process() override;

    virtual uint16_t output(uint8_t* buffer) override;

    virtual void     finish() override;

  private:
    uint8_t        m_n;
    YDND_STATE     m_state;
    CAMBE3000Utils m_utils;
    uint8_t        m_buffer1[50U];
    uint8_t        m_buffer2[50U];
    uint16_t       m_len1;
    uint16_t       m_len2;
};

#endif
