/*
 *   Copyright (C) 2002-2004,2007-2009,2011-2013,2015-2017,2020,2021,2024 by Jonathan Naylor G4KLX
 *   Copyright (C) 1999-2001 by Thomas Sailor HB9JNX
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

#ifndef UARTController_H
#define UARTController_H

#include <string>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

class CUARTController {
public:
	CUARTController(const std::string& device, unsigned int speed);
	~CUARTController();

	bool open();

	int16_t read(uint8_t* buffer, uint16_t length);

	int16_t write(const uint8_t* buffer, uint16_t length);

	void close();

#if defined(__APPLE__)
	int setNonblock(bool nonblock);
#endif

protected:
	std::string    m_device;
	unsigned int   m_speed;
#if defined(_WIN32) || defined(_WIN64)
	HANDLE         m_handle;
#else
	int            m_fd;
#endif

#if defined(_WIN32) || defined(_WIN64)
	int16_t readNonblock(uint8_t* buffer, uint16_t length);
#else
	bool canWrite();
	bool setRaw();
#endif
};

#endif
