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

#include "FileConvert.h"

#include <cassert>

int main(int argc, char** argv)
{
	if (argc < 6) {
		::fprintf(stderr, "Usage: FileConvert <port> <input mode> <input file> <output mode> <output file>\n");
		::fprintf(stderr, "Modes are: 1 - D-Star\n");
		::fprintf(stderr, "           2 - DMR/NXDN\n");
		::fprintf(stderr, "           3 - YSF DN\n");
		::fprintf(stderr, "           4 - IMBE\n");
		::fprintf(stderr, "           5 - IMBE (with FEC)\n");
		::fprintf(stderr, "           6 - M17\n");
		::fprintf(stderr, "          99 - WAV\n");
		return 1;
	}

	std::string port = argv[1U];

	MODE inMode        = MODE(::atoi(argv[2U]));
	std::string inFile = argv[3U];

	MODE outMode        = MODE(::atoi(argv[4U]));
	std::string outFile = argv[5U];

	CFileConvert file(port, inMode, inFile, outMode, outFile);

	return file.run();
}

CFileConvert::CFileConvert(const std::string& port, MODE inMode, const std::string& inFile, MODE outMode, const std::string& outFile) :
m_port(port),
m_inMode(inMode),
m_inFile(inFile),
m_outMode(outMode),
m_outFile(outFile)
{
	assert(!port.empty());
	assert(!inFile.empty());
	assert(!outFile.empty());
}

CFileConvert::~CFileConvert()
{
}

int CFileConvert::run()
{
	return 0;
}
