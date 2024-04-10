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

#ifndef Tester_H
#define Tester_H

#include <string>
#include <cstdint>

#include "StopWatch.h"
#include "UARTController.h"

class CTester {
public:
	CTester(const std::string& device, unsigned int speed);
	~CTester();

	int run();

protected:
	CUARTController m_serial;
	CStopWatch      m_stopwatch;
	unsigned int    m_count;
	unsigned int    m_ok;
	unsigned int    m_failed;

	bool     test(const char* title, const uint8_t* inData, uint16_t inLen, const uint8_t* outData, uint16_t outLen);
	void     dump(const char* title, const uint8_t* buffer, uint16_t length) const;
	uint16_t read(uint8_t* buffer, uint16_t timeout);
};

#endif
