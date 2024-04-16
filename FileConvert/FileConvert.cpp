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

const unsigned int PCM_SAMPLE_RATE = 8000U;
const unsigned int PCM_BLOCK_SIZE  = 160U;		// 20ms at 8000 samples/sec
const unsigned int PCM_BLOCK_TIME  = 20U;
const unsigned int PCM_NO_CHANNELS = 1U;

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
m_hasAMBE(NO_AMBE_CHIP)
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

	ret = validateOptions();
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
		::fprintf(stderr, "Error writing the data to the transcoder\n");
		return false;
	}

	uint8_t buffer[400U];
	uint16_t len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "Transcoder version read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "NAK returned for get version - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_GET_VERSION:
		if (buffer[GET_VERSION_PROTOCOL_POS] != PROTOCOL_VERSION) {
			::fprintf(stderr, "Unknown protocol version - %u\n", buffer[GET_VERSION_PROTOCOL_POS]);
			m_serial.close();
			return false;
		}

		::fprintf(stdout, "Transcoder version - %.*s\n", len - 5U, buffer + 5U);
		break;

	default:
		::fprintf(stderr, "Unknown response from the transcoder to get version - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

	ret2 = m_serial.write(GET_CAPABILITIES, GET_CAPABILITIES_LEN);
	if (ret2 <= 0) {
		::fprintf(stderr, "Error writing data to the transcoder\n");
		return false;
	}

	len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "Transcoder capabilities read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "NAK returned for get capabilities - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_GET_CAPABILITIES:
		break;

	default:
		::fprintf(stderr, "Unknown response from the transcoder to get capabilities - 0x%02X\n", buffer[TYPE_POS]);
		m_serial.close();
		return false;
	}

	m_hasAMBE = buffer[GET_CAPABILITIES_AMBE_TYPE_POS];
	switch (m_hasAMBE) {
	case HAS_1AMBE_CHIP:
		::fprintf(stdout, "Transcoder has 1 AMBE chip\n");
		break;
	case HAS_2AMBE_CHIPS:
		::fprintf(stdout, "Transcoder has 2 AMBE chips\n");
		break;
	default:
		::fprintf(stdout, "Transcoder has no AMBE chips\n");
		break;
	}

	uint8_t command[10U];
	::memcpy(command + 0U, SET_MODE_HEADER, SET_MODE_HEADER_LEN);
	command[INPUT_MODE_POS]  = m_inMode;
	command[OUTPUT_MODE_POS] = m_outMode;

	ret2 = m_serial.write(command, SET_MODE_LEN);
	if (ret2 <= 0) {
		::fprintf(stderr, "Error writing data to the transcoder\n");
		return false;
	}

	len = read(buffer, 200U);
	if (len == 0U) {
		::fprintf(stderr, "Set mode read timeout (200 me)\n");
		m_serial.close();
		return false;
	}

	switch (buffer[TYPE_POS]) {
	case TYPE_NAK:
		::fprintf(stderr, "NAK returned for set mode - %u\n", buffer[NAK_ERROR_POS]);
		m_serial.close();
		return false;

	case TYPE_ACK:
		::fprintf(stdout, "Conversion modes - set\n");
		return true;

	default:
		::fprintf(stderr, "Unknown response from the transcoder to set mode - 0x%02X\n", buffer[TYPE_POS]);
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
				::fprintf(stderr, "Read has timed out after %u ms\n", timeout);
				return len;
			}
		}
	}
}

// The transcoder can do PCM to PCM, but it seems a bit silly to use it for such a simple task.
bool CFileConvert::convertPCMtoPCM()
{
	CWAVFileReader reader(m_inFile, PCM_BLOCK_SIZE);
	bool ret = reader.open();
	if (!ret)
		return false;

	unsigned int sampleRate = reader.getSampleRate();
	if (sampleRate != PCM_SAMPLE_RATE) {
		::fprintf(stderr, "Invalid file sample rate - %u\n", sampleRate);
		reader.close();
		return false;
	}

	unsigned int channels = reader.getChannels();
	if (channels != PCM_NO_CHANNELS) {
		::fprintf(stderr, "Invalid number of channels - %u\n", channels);
		reader.close();
		return false;
	}

	CWAVFileWriter writer(m_outFile, PCM_SAMPLE_RATE, PCM_NO_CHANNELS, 16U, PCM_BLOCK_SIZE);
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	CStopWatch stopWatch;
	stopWatch.start();

	unsigned int frames = 0U;

	float buffer[PCM_BLOCK_SIZE];

	unsigned int length = reader.read(buffer, PCM_BLOCK_SIZE);
	while (length > 0U) {
		writer.write(buffer, length);
		frames++;

		length = reader.read(buffer, PCM_BLOCK_SIZE);
	}

	reader.close();
	writer.close();

	unsigned int ms = stopWatch.elapsed();

	::fprintf(stdout, "Converted %u frames (%.1fs) in %.1fs\n", frames, float(frames * PCM_BLOCK_TIME) / 1000.0F, float(ms) / 1000.0F);

	return true;
}

bool CFileConvert::convertPCMtoDV()
{
	CWAVFileReader reader(m_inFile, PCM_BLOCK_SIZE);
	bool ret = reader.open();
	if (!ret)
		return false;

	unsigned int sampleRate = reader.getSampleRate();
	if (sampleRate != PCM_SAMPLE_RATE) {
		::fprintf(stderr, "Invalid file sample rate - %u\n", sampleRate);
		reader.close();
		return false;
	}

	unsigned int channels = reader.getChannels();
	if (channels != PCM_NO_CHANNELS) {
		::fprintf(stderr, "Invalid number of channels - %u\n", channels);
		reader.close();
		return false;
	}

	CDVFileWriter writer(m_outFile, getFileSignature(m_outMode));
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int dvLength = getBlockLength(m_outMode);

	CStopWatch stopWatch;
	stopWatch.start();

	unsigned int frames = 0U;

	float buffer1[PCM_BLOCK_SIZE];
	unsigned int length = reader.read(buffer1, PCM_BLOCK_SIZE);
	while (length > 0U) {
		int16_t buffer2[PCM_BLOCK_SIZE];
		for (unsigned int i = 0U; i < PCM_BLOCK_SIZE; i++)
			buffer2[i] = int16_t(buffer1[i] * 32768.0F + 0.5F);

		uint8_t buffer3[PCM_DATA_LEN];
		::memcpy(buffer3 + 0U, PCM_DATA_HEADER, DATA_HEADER_LEN);
		::memcpy(buffer3 + DATA_START_POS, buffer2, PCM_DATA_LENGTH);

		int16_t ret = m_serial.write(buffer3, PCM_DATA_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "Error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer3, 200U);
		if (len == 0U) {
			::fprintf(stderr, "Transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer3[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "NAK returned for transcoding - %u\n", buffer3[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "Unknown response from the transcoder to transcoding - 0x%02X\n", buffer3[TYPE_POS]);
			return false;
		}

		frames++;

		writer.write(buffer3 + DATA_START_POS, dvLength);

		length = reader.read(buffer1, PCM_BLOCK_SIZE);
	}

	reader.close();
	writer.close();

	unsigned int ms = stopWatch.elapsed();

	::fprintf(stdout, "Converted %u frames (%.1fs) in %.1fs", frames, float(frames * PCM_BLOCK_TIME) / 1000.0F, float(ms) / 1000.0F);

	return true;
}

bool CFileConvert::convertDVtoPCM()
{
	CDVFileReader reader(m_inFile, getFileSignature(m_inMode));
	bool ret = reader.open();
	if (!ret)
		return false;

	CWAVFileWriter writer(m_outFile, PCM_SAMPLE_RATE, PCM_NO_CHANNELS, 16U, PCM_BLOCK_SIZE);
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int dvLength = getBlockLength(m_inMode);
	const uint8_t* header = getDataHeader(m_inMode);

	CStopWatch stopWatch;
	stopWatch.start();

	unsigned int frames = 0U;

	uint8_t buffer1[50U];
	unsigned int length = reader.read(buffer1, dvLength);
	while (length > 0U) {
		uint8_t buffer2[PCM_DATA_LEN];
		::memcpy(buffer2 + 0U, header, DATA_HEADER_LEN);
		::memcpy(buffer2 + DATA_START_POS, buffer1, dvLength);

		int16_t ret = m_serial.write(buffer2, dvLength + DATA_HEADER_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "Error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer2, 200U);
		if (len == 0U) {
			::fprintf(stderr, "Transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer2[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "NAK returned for transcoding - %u\n", buffer2[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "Unknown response from the transcoder to transcoding - 0x%02X\n", buffer2[TYPE_POS]);
			return false;
		}

		frames++;

		int16_t* buffer3 = (int16_t*)(buffer2 + DATA_START_POS);

		float buffer4[PCM_BLOCK_SIZE];
		for (unsigned int i = 0U; i < PCM_BLOCK_SIZE; i++)
			buffer4[i] = float(buffer3[i]) / 32768.0F;

		writer.write(buffer4, PCM_BLOCK_SIZE);

		length = reader.read(buffer1, dvLength);
	}

	reader.close();
	writer.close();

	unsigned int ms = stopWatch.elapsed();

	::fprintf(stdout, "Converted %u frames (%.1fs) in %.1fs\n", frames, float(frames * PCM_BLOCK_TIME) / 1000.0F, float(ms) / 1000.0F);

	return true;
}

bool CFileConvert::convertDVtoDV()
{
	CDVFileReader reader(m_inFile, getFileSignature(m_inMode));
	bool ret = reader.open();
	if (!ret)
		return false;

	CDVFileWriter writer(m_outFile, getFileSignature(m_outMode));
	ret = writer.open();
	if (!ret) {
		reader.close();
		return false;
	}

	unsigned int inLength  = getBlockLength(m_inMode);
	unsigned int outLength = getBlockLength(m_outMode);
	const uint8_t* header  = getDataHeader(m_inMode);

	CStopWatch stopWatch;
	stopWatch.start();

	unsigned int frames = 0U;

	uint8_t buffer1[50U];
	unsigned int length = reader.read(buffer1, inLength);
	while (length > 0U) {
		uint8_t buffer2[50U];
		::memcpy(buffer2 + 0U, header, DATA_HEADER_LEN);
		::memcpy(buffer2 + DATA_START_POS, buffer1, inLength);

		int16_t ret = m_serial.write(buffer2, inLength + DATA_HEADER_LEN);
		if (ret <= 0) {
			::fprintf(stderr, "Error writing the data to the transcoder\n");
			return false;
		}

		uint16_t len = read(buffer2, 200U);
		if (len == 0U) {
			::fprintf(stderr, "Transcode read timeout (200 me)\n");
			return false;
		}

		switch (buffer2[TYPE_POS]) {
		case TYPE_NAK:
			::fprintf(stderr, "NAK returned for transcoding - %u\n", buffer2[NAK_ERROR_POS]);
			return false;

		case TYPE_DATA:
			break;

		default:
			::fprintf(stderr, "Unknown response from the transcoder to transcoding - 0x%02X\n", buffer2[TYPE_POS]);
			return false;
		}

		frames++;

		writer.write(buffer2 + DATA_START_POS, outLength);

		length = reader.read(buffer1, inLength);
	}

	reader.close();
	writer.close();

	unsigned int ms = stopWatch.elapsed();

	::fprintf(stdout, "Converted %u frames (%.1fs) in %.1fs\n", frames, float(frames * PCM_BLOCK_TIME) / 1000.0F, float(ms) / 1000.0F);

	return true;
}

unsigned int CFileConvert::getBlockLength(uint8_t mode) const
{
	switch (mode) {
	case MODE_DSTAR:
		return DSTAR_DATA_LENGTH;
	case MODE_DMR_NXDN:
		return DMR_NXDN_DATA_LENGTH;
	case MODE_YSFDN:
		return YSFDN_DATA_LENGTH;
	case MODE_IMBE:
		return IMBE_DATA_LENGTH;
	case MODE_IMBE_FEC:
		return IMBE_FEC_DATA_LENGTH;
	case MODE_CODEC2_3200:
		return CODEC2_3200_DATA_LENGTH;
	default:
		return PCM_DATA_LENGTH;
	}
}

const uint8_t* CFileConvert::getDataHeader(uint8_t mode) const
{
	switch (mode) {
	case MODE_DSTAR:
		return DSTAR_DATA_HEADER;
	case MODE_DMR_NXDN:
		return DMR_NXDN_DATA_HEADER;
	case MODE_YSFDN:
		return YSFDN_DATA_HEADER;
	case MODE_IMBE:
		return IMBE_DATA_HEADER;
	case MODE_IMBE_FEC:
		return IMBE_FEC_DATA_HEADER;
	case MODE_CODEC2_3200:
		return CODEC2_3200_DATA_HEADER;
	default:
		return PCM_DATA_HEADER;
	}
}

std::string CFileConvert::getFileSignature(uint8_t mode) const
{
	switch (mode) {
	case MODE_DSTAR:
		return "AMBE";
	default:
		return "";
	}
}

void CFileConvert::dump(const char* text, const uint8_t* buffer, size_t length) const
{
	assert(buffer != nullptr);
	assert(length > 0U);

	::fprintf(stdout, "%s\n", text);

	size_t offset = 0U;

	while (offset < length) {
		::fprintf(stdout, "%04X: ", (unsigned int)offset);

		for (unsigned int i = 0U; i < 16U; i++) {
			if ((offset + i) < length)
				::fprintf(stdout, "%02X ", buffer[offset + i]);
			else
				::fprintf(stdout, "   ");
		}

		::fprintf(stdout, "  *");

		for (unsigned int i = 0U; i < 16U; i++) {
			if ((offset + i) < length) {
				if (isprint(buffer[offset + i]))
					::fprintf(stdout, "%c", buffer[offset + i]);
				else
					::fprintf(stdout, ".");
			}
		}

		::fprintf(stdout, "*\n");

		offset += 16U;
	}
}

bool CFileConvert::validateOptions() const
{
	switch (m_hasAMBE) {
	case HAS_1AMBE_CHIP:
		if ((m_inMode == MODE_DSTAR) && (m_outMode == MODE_DMR_NXDN)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		if ((m_inMode == MODE_DMR_NXDN) && (m_outMode == MODE_DSTAR)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		if ((m_inMode == MODE_DSTAR) && (m_outMode == MODE_YSFDN)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		if ((m_inMode == MODE_YSFDN) && (m_outMode == MODE_DSTAR)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		break;

	case HAS_2AMBE_CHIPS:
		return true;

	default:
		if ((m_inMode == MODE_DSTAR) && (m_outMode != MODE_DSTAR)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		if ((m_inMode != MODE_DSTAR) && (m_outMode == MODE_DSTAR)) {
			::fprintf(stderr, "Transcoding isn't possible without an AMBE chip\n");
			return false;
		}
		if ((m_inMode == MODE_DMR_NXDN) && ((m_outMode != MODE_DMR_NXDN) && (m_outMode != MODE_YSFDN))) {
			::fprintf(stderr, "Transcoding isn't possinle without an AMBE chip\n");
			return false;
		}
		if ((m_outMode == MODE_DMR_NXDN) && ((m_inMode != MODE_DMR_NXDN) && (m_inMode != MODE_YSFDN))) {
			::fprintf(stderr, "Transcoding isn't possinle without an AMBE chip\n");
			return false;
		}
		if ((m_inMode == MODE_YSFDN) && ((m_outMode != MODE_DMR_NXDN) && (m_outMode != MODE_YSFDN))) {
			::fprintf(stderr, "Transcoding isn't possinle without an AMBE chip\n");
			return false;
		}
		if ((m_outMode == MODE_YSFDN) && ((m_inMode != MODE_DMR_NXDN) && (m_inMode != MODE_YSFDN))) {
			::fprintf(stderr, "Transcoding isn't possinle without an AMBE chip\n");
			return false;
		}
		return true;
	}
}
