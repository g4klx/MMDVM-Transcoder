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

const unsigned int SAMPLE_RATE = 8000U;
const unsigned int BLOCK_SIZE  = 160U;		// 20ms at 8000 samples/sec
const unsigned int BLOCK_TIME  = 20U;
const unsigned int NO_CHANNELS = 1U;

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
		::fprintf(stderr, "           m17      - M17 (Codec2 3200)\n");
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

	if ((m_inMode == MODE_PCM) && (m_outMode == MODE_PCM))
		ret = convertPCMtoPCM();
	else if (m_inMode == MODE_PCM)
		ret = convertPCMtoDV();
	else if (m_outMode == MODE_PCM)
		ret = convertDVtoPCM();
	else
		ret = convertDVtoDV();

	m_serial.close();

	return ret ? 0 : 1;
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

		::fprintf(stdout, "Transcoder version - %.*s\n", len - 5U, buffer + 5U);
		break;

	default:
		::fprintf(stderr, "FileConvert: unknown response from the transcoder to get version - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

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
		m_hasAMBE = buffer[GET_CAPABILITIES_AMBE_TYPE_POS] == HAS_AMBE_CHIP;
		::fprintf(stdout, "Transcoder has AMBE - %s\n", m_hasAMBE ? "yes" : "no");
		break;

	default:
		::fprintf(stderr, "FileConvert: unknown response from the transcoder to get capabilities - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

	uint8_t command[10U];
	::memcpy(command + 0U, SET_MODE_HEADER, SET_MODE_HEADER_LEN);
	command[INPUT_MODE_POS]  = m_inMode;
	command[OUTPUT_MODE_POS] = m_outMode;

	ret2 = m_serial.write(command, SET_MODE_LEN);
	if (ret2 <= 0) {
		::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
		return false;
	}

	len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "FileConvert: set mode read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "FileConvert: NAK returned for set mode - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_ACK:
		::fprintf(stdout, "FileConvert: conversion modes set\n");
		return true;

	default:
		::fprintf(stderr, "FileConvert: unknown response from the transcoder to set mode - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}
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

// The transcoder can do PCM to PCM, but it seems a bit silly to use it for such a simple task.
bool CFileConvert::convertPCMtoPCM()
{
	CWAVFileReader reader(m_inFile, BLOCK_SIZE);
	bool ret = reader.open();
	if (!ret)
		return false;

	unsigned int sampleRate = reader.getSampleRate();
	if (sampleRate != SAMPLE_RATE) {
		::fprintf(stderr, "FileConvert: invalid sample rate - %u\n", sampleRate);
		reader.close();
		return false;
	}

	unsigned int channels = reader.getChannels();
	if (channels != NO_CHANNELS) {
		::fprintf(stderr, "FileConvert: invalid number of channels - %u\n", channels);
		reader.close();
		return false;
	}

	CWAVFileWriter writer(m_outFile, SAMPLE_RATE, NO_CHANNELS, 16U, BLOCK_SIZE);
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int frames = 0U;

	float buffer[BLOCK_SIZE];

	unsigned int length = reader.read(buffer, BLOCK_SIZE);
	while (length > 0U) {
		writer.write(buffer, length);
		frames++;

		length = reader.read(buffer, BLOCK_SIZE);
	}

	reader.close();
	writer.close();

	::fprintf(stdout, "Converted %u frames (%.1fs)", frames, float(frames * BLOCK_TIME) / 1000.0F);

	return true;
}

bool CFileConvert::convertPCMtoDV()
{
	CWAVFileReader reader(m_inFile, BLOCK_SIZE);
	bool ret = reader.open();
	if (!ret)
		return false;

	unsigned int sampleRate = reader.getSampleRate();
	if (sampleRate != SAMPLE_RATE) {
		::fprintf(stderr, "FileConvert: invalid sample rate - %u\n", sampleRate);
		reader.close();
		return false;
	}

	unsigned int channels = reader.getChannels();
	if (channels != NO_CHANNELS) {
		::fprintf(stderr, "FileConvert: invalid number of channels - %u\n", channels);
		reader.close();
		return false;
	}

	CDVFileWriter writer(m_outFile, fileSignature(m_outMode));
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int dvLength = blockLength(m_outMode);

	unsigned int frames = 0U;

	float buffer1[BLOCK_SIZE];
	unsigned int length = reader.read(buffer1, BLOCK_SIZE);
	while (length > 0U) {
		int16_t buffer2[BLOCK_SIZE];
		for (unsigned int i = 0U; i < length; i++)
			buffer2[i] = int16_t(buffer1[i] * 32768.0F + 0.5F);

		uint8_t buffer3[PCM_DATA_LEN];
		::memcpy(buffer3 + 0U, PCM_DATA_HEADER, DATA_HEADER_LEN);
		::memcpy(buffer3 + DATA_START_POS, buffer2, PCM_DATA_LENGTH);

		int16_t ret = m_serial.write(buffer3, PCM_DATA_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer3, 200U);
		if (len == 0U) {
			::fprintf(stderr, "FileConvert: transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer3[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "FileConvert: NAK returned for transcoding - %u\n", buffer3[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "FileConvert: unknown response from the transcoder to transcoding - 0x%02X\n", buffer3[TYPE_POS]);
			return false;
		}

		frames++;

		writer.write(buffer3 + DATA_START_POS, dvLength);

		length = reader.read(buffer1, BLOCK_SIZE);
	}

	reader.close();
	writer.close();

	::fprintf(stdout, "Converted %u frames (%.1fs)", frames, float(frames * BLOCK_TIME) / 1000.0F);

	return true;
}

bool CFileConvert::convertDVtoPCM()
{
	CDVFileReader reader(m_inFile, fileSignature(m_inMode));
	bool ret = reader.open();
	if (!ret)
		return false;

	CWAVFileWriter writer(m_outFile, SAMPLE_RATE, NO_CHANNELS, 16U, BLOCK_SIZE);
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int dvLength = blockLength(m_inMode);
	const uint8_t* header = getDataHeader(m_inMode);

	unsigned int frames = 0U;

	uint8_t buffer1[50U];
	unsigned int length = reader.read(buffer1, dvLength);
	while (length > 0U) {
		uint8_t buffer2[PCM_DATA_LEN];
		::memcpy(buffer2 + 0U, header, DATA_HEADER_LEN);
		::memcpy(buffer2 + DATA_START_POS, buffer1, dvLength);

		int16_t ret = m_serial.write(buffer2, dvLength + DATA_HEADER_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer2, 200U);
		if (len == 0U) {
			::fprintf(stderr, "FileConvert: transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer2[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "FileConvert: NAK returned for transcoding - %u\n", buffer2[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "FileConvert: unknown response from the transcoder to transcoding - 0x%02X\n", buffer2[TYPE_POS]);
			return false;
		}

		frames++;

		float buffer3[BLOCK_SIZE];
		for (unsigned int i = 0U; i < length; i++)
			buffer3[i] = float(buffer2[i]) / 32768.0F;

		writer.write(buffer3, BLOCK_SIZE);

		length = reader.read(buffer1, dvLength);
	}

	reader.close();
	writer.close();

	::fprintf(stdout, "Converted %u frames (%.1fs)", frames, float(frames * BLOCK_TIME) / 1000.0F);

	return true;
}

bool CFileConvert::convertDVtoDV()
{
	CDVFileReader reader(m_inFile, fileSignature(m_inMode));
	bool ret = reader.open();
	if (!ret)
		return false;

	CDVFileWriter writer(m_outFile, fileSignature(m_outMode));
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int inLength  = blockLength(m_inMode);
	unsigned int outLength = blockLength(m_outMode);
	const uint8_t* header  = getDataHeader(m_inMode);

	unsigned int frames = 0U;

	uint8_t buffer1[50U];
	unsigned int length = reader.read(buffer1, inLength);
	while (length > 0U) {
		uint8_t buffer2[50U];
		::memcpy(buffer2 + 0U, header, DATA_HEADER_LEN);
		::memcpy(buffer2 + DATA_START_POS, buffer1, inLength);

		int16_t ret = m_serial.write(buffer2, inLength + DATA_HEADER_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "FileConvert: error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer2, 200U);
		if (len == 0U) {
			::fprintf(stderr, "FileConvert: transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer2[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "FileConvert: NAK returned for transcoding - %u\n", buffer2[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "FileConvert: unknown response from the transcoder to transcoding - 0x%02X\n", buffer2[TYPE_POS]);
			return false;
		}

		frames++;

		writer.write(buffer2 + DATA_START_POS, outLength);

		length = reader.read(buffer1, inLength);
	}

	reader.close();
	writer.close();

	::fprintf(stdout, "Converted %u frames (%.1fs)", frames, float(frames * BLOCK_TIME) / 1000.0F);

	return true;
}
