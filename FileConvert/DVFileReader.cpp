/*
*   Copyright (C) 2017,2018,2024 by Jonathan Naylor G4KLX
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

#include "DVFileReader.h"

#include <cassert>

CDVFileReader::CDVFileReader(const std::string& fileName, const std::string& signature) :
m_fileName(fileName),
m_signature(signature),
m_fp(nullptr)
{
}

CDVFileReader::~CDVFileReader()
{
}

bool CDVFileReader::open()
{
	m_fp = ::fopen(m_fileName.c_str(), "rb");
	if (m_fp == nullptr) {
		::fprintf(stderr, "FileConvert: could not open the DV file %s\n", m_fileName.c_str());
		return false;
	}

	if (!m_signature.empty()) {
		char buffer[25U];
		size_t n = ::fread(buffer, sizeof(uint8_t), m_signature.size(), m_fp);
		if ((n != m_signature.size()) || (m_signature != std::string(buffer, m_signature.size()))) {
			::fprintf(stderr, "FileConvert: the file signature didn't match the one specified\n");
			close();
			return false;
		}
	}

	return true;
}

unsigned int CDVFileReader::read(uint8_t* buffer, unsigned int length)
{
	assert(m_fp != nullptr);
	assert(buffer != nullptr);
	assert(length > 0U);

	return (unsigned int)::fread(buffer, sizeof(uint8_t), length, m_fp);
}

void CDVFileReader::close()
{
	assert(m_fp != nullptr);

	::fclose(m_fp);

	m_fp = nullptr;
}
