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

#ifndef FileConvert_H
#define FileConvert_H

#include "UARTController.h"

#include <string>

class CFileConvert {
public:
	CFileConvert(const std::string& port, uint8_t inMode, const std::string& inFile, uint8_t outMode, const std::string& outFile);
	~CFileConvert();

	int run();

private:
	CUARTController m_serial;
	uint8_t         m_inMode;
	std::string     m_inFile;
	uint8_t         m_outMode;
	std::string     m_outFile;
	bool            m_hasAMBE;

	bool open();
	uint16_t read(uint8_t* buffer, uint16_t timeout);
	std::string getFileSignature(uint8_t mode) const;
	unsigned int getBlockLength(uint8_t mode) const;
	const uint8_t* getDataHeader(uint8_t mode) const;
	bool convertPCMtoPCM();
	bool convertPCMtoDV();
	bool convertDVtoPCM();
	bool convertDVtoDV();
};

#endif
