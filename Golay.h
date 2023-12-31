/*
 *   Copyright (C) 2010,2016,2021,2023 by Jonathan Naylor G4KLX
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

#ifndef Golay_H
#define Golay_H

#include <cstdint>

class CGolay {
public:
	static uint32_t encode23127(uint32_t data);
	static uint32_t encode24128(uint32_t data);

	static uint32_t decode23127(uint32_t code);

	static bool decode24128(uint32_t in, uint32_t& out);
	static bool decode24128(uint8_t* in, uint32_t& out);
};

#endif
