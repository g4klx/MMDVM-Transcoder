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

#include <string>

enum MODE {
	MODE_DSTAR = 1,
	MODE_DMR_NXDN,
	MODE_YSFDN,
	MODE_IMBE,
	MODE_IMBE_FEC,
	MODE_M17,
	MODE_PCM = 99
};

class CFileConvert {
public:
	CFileConvert(const std::string& port, MODE inMode, const std::string& inFile, MODE outMode, const std::string& outFile);
	~CFileConvert();

	int run();

private:
	std::string m_port;
	MODE        m_inMode;
	std::string m_inFile;
	MODE        m_outMode;
	std::string m_outFile;
};

#endif
