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

#include "WAVFileReader.h"
#include "WAVFileWriter.h"
#include "DVFileReader.h"
#include "DVFileWriter.h"
#include "Transcoder.h"
#include "StopWatch.h"
#include "Thread.h"

#include <cstring>
#include <cassert>

uint8_t convertMode(const char* text)
{
	if (::strcmp(text, "dstar") == 0)
		return MODE_DSTAR;
	else if (::strcmp(text, "dmr") == 0)
		return MODE_DMR_NXDN;
	else if (::strcmp(text, "nxdn") == 0)
		return MODE_DMR_NXDN;
	else if (::strcmp(text, "ysfdn") == 0)
		return MODE_YSFDN;
	else if (::strcmp(text, "imbe") == 0)
		return MODE_IMBE;
	else if (::strcmp(text, "imbe_fec") == 0)
		return MODE_IMBE_FEC;
	else if (::strcmp(text, "m17") == 0)
		return MODE_CODEC2_3200;
	else if (::strcmp(text, "wav") == 0)
		return MODE_PCM;
	else
		return MODE_PASS_THROUGH;
}

int main(int argc, char** argv)
{
	if (argc < 6) {
		::fprintf(stderr, "Usage: FileConvert <port> <input mode> <input file> <output mode> <output file>\n");
		::fprintf(stderr, "Modes are: dstar    - D-Star\n");
		::fprintf(stderr, "           dmr      - DMR\n");
		::fprintf(stderr, "           nxdn     - NXDN\n");
		::fprintf(stderr, "           ysfdn    - YSF DN\n");
		::fprintf(stderr, "           imbe     - IMBE (no FEC)\n");
		::fprintf(stderr, "           imbe_fec - IMBE (with FEC)\n");
		::fprintf(stderr, "           m17      - M17\n");
		::fprintf(stderr, "           wav      - WAV\n");
		return 1;
	}

	std::string port = argv[1U];

	uint8_t inMode = convertMode(argv[2U]);
	if (inMode == MODE_PASS_THROUGH) {
		::fprintf(stderr, "FileConvert: unknown mode - %s\n", argv[2U]);
		return 1;
	}
	std::string inFile = argv[3U];

	uint8_t outMode = convertMode(argv[4U]);
	if (outMode == MODE_PASS_THROUGH) {
		::fprintf(stderr, "FileConvert: unknown mode - %s\n", argv[4U]);
		return 1;
	}
	std::string outFile = argv[5U];

	CFileConvert file(port, inMode, inFile, outMode, outFile);

	return file.run();
}

CFileConvert::CFileConvert(const std::string& port, uint8_t inMode, const std::string& inFile, uint8_t outMode, const std::string& outFile) :
m_serial(port, TRANSCODER_SPEED),
m_inMode(inMode),
m_inFile(inFile),
m_outMode(outMode),
m_outFile(outFile),
m_hasAMBE(false)
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
	bool ret = open();
	if (!ret)
		return 1;

	if ((m_inMode == MODE_PCM) && (m_outMode == MODE_PCM)) {
	} else if (m_inMode == MODE_PCM) {
	} else if (m_outMode == MODE_PCM) {
	} else {
	}

	m_serial.close();

	return 0;
}

bool CFileConvert::open()
{
	bool ret1 = m_serial.open();
	if (!ret1) {
		::fprintf(stderr, "FileConvert: cannot open the transcoder port\n");
		return false;
	}

	int16_t ret2 = m_serial.write(GET_VERSION, GET_VERSION_LEN);
	if (ret2 <= 0) {
		::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
		return false;
	}

	uint8_t buffer[400U];
	uint16_t len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "FileConvert: version read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "FileConvert: NAK returned for get version - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_GET_VERSION:
		if (buffer[GET_VERSION_PROTOCOL_POS] != PROTOCOL_VERSION) {
			::fprintf(stderr, "FileConvert: unknown protocol version - %u\n", buffer[GET_VERSION_PROTOCOL_POS]);
			m_serial.close();
			return false;
		}
		break;

	default:
		::fprintf(stderr, "FileConvert: unknown response from the transcoder to get version - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

	::fprintf(stdout, "Transcoder version - %.*s\n", len - 5U, buffer + 5U);

	ret2 = m_serial.write(GET_CAPABILITIES, GET_CAPABILITIES_LEN);
	if (ret2 <= 0) {
		::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
		return false;
	}

	len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "FileConvert: capabilities read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "FileConvert: NAK returned for get capabilities - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_GET_CAPABILITIES:
		break;

	default:
		::fprintf(stderr, "FileConvert: unknown response from the transcoder to get capabilities - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

	m_hasAMBE = buffer[GET_CAPABILITIES_AMBE_TYPE_POS] == HAS_AMBE_CHIP;

	::fprintf(stdout, "Transcoder has AMBE - %s\n", m_hasAMBE ? "yes" : "no");

	return true;
}


uint16_t CFileConvert::read(uint8_t* buffer, uint16_t timeout)
{
	assert(buffer != NULL);
	assert(timeout > 0U);

	uint16_t len = 0U;
	uint16_t ptr = 0U;

	CStopWatch stopwatch;
	stopwatch.start();

	for (;;) {
		uint8_t c = 0U;
		if (m_serial.read(&c, 1U) == 1) {
			if (ptr == MARKER_POS) {
				if (c == MARKER) {
					// Handle the frame start correctly
					buffer[0U] = c;
					ptr = 1U;
					len = 0U;
				} else {
					ptr = 0U;
					len = 0U;
				}
			} else if (ptr == LENGTH_LSB_POS) {
				// Handle the frame length LSB
				uint8_t val = buffer[ptr] = c;
				len = (val << 0) & 0x00FFU;
				ptr = 2U;
			} else if (ptr == LENGTH_MSB_POS) {
				// Handle the frame length MSB
				uint8_t val = buffer[ptr] = c;
				len |= (val << 8) & 0xFF00U;
				ptr = 3U;
			} else {
				// Any other bytes are added to the buffer
				buffer[ptr] = c;
				ptr++;

				// The full packet has been received, process it
				if (ptr == len)
					return len;
			}
		} else {
			unsigned long elapsed = stopwatch.elapsed() / 1000U;
			if (elapsed > timeout) {
				::fprintf(stderr, "FileConvert: read has timed out after %u ms\n", timeout);
				return len;
			}
		}
	}
}
