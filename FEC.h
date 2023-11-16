/*
 *   Copyright (C) 2010,2014,2016,2018,2023 by Jonathan Naylor G4KLX
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

#if !defined(FEC_H)
#define	FEC_H

#include <cstdint>

class CFEC {
public:
	CFEC();
	~CFEC();

	unsigned int regenerateDMR(uint8_t* bytes) const;

	unsigned int regenerateDStar(uint8_t* bytes) const;

	unsigned int regenerateIMBE(uint8_t* bytes) const;

private:
	unsigned int regenerateDStar(uint32_t& a, uint32_t& b) const;
	unsigned int regenerateDMR(uint32_t& a, uint32_t& b, uint32_t& c) const;
};

#endif
