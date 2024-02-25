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

#include "Handler.h"

#include <cassert>

const uint8_t MARKER = 0xE1U;

const uint8_t  GET_VERSION_REQ[]   = { MARKER, 0x04U, 0x00U, 0x00U};
const uint16_t GET_VERSION_REQ_LEN = 4U;
const uint8_t  GET_VERSION_REP[]   = { MARKER, 0x2BU, 0x00U, 0x00U, 0x01U, 0x32U, 0x30U, 0x32U, 0x34U };
const uint16_t GET_VERSION_REP_LEN = 9U;

const uint8_t  GET_CAPABILITIES_REQ[]   = { MARKER, 0x04U, 0x00U, 0x01U };
const uint16_t GET_CAPABILITIES_REQ_LEN = 4U;
const uint8_t  GET_CAPABILITIES_REP[]   = { MARKER, 0x05U, 0x00U, 0x01U, 0x01U };
const uint16_t GET_CAPABILITIES_REP_LEN = 5U;

// D-Star to PCM Mode Set
const uint8_t  SET_MODE1_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0xFFU };
const uint16_t SET_MODE1_REQ_LEN = 6U;

// D-Star to PCM Data
const uint8_t  MODE1_DATA_REQ[]   = { MARKER, 0x0DU, 0x00U, 0x05U, 0x02U, 0x19U, 0x17U, 0xE4U, 0xB3U, 0xE2U, 0x00U, 0xA2U, 0x20U };
const uint16_t MODE1_DATA_REQ_LEN = 13U;
const uint8_t  MODE1_DATA_REP[]   = { MARKER, 0x44U, 0x01U, 0x05U };
const uint16_t MODE1_DATA_REP_LEN = 4U;

// DMR to PCM Mode Set
const uint8_t  SET_MODE2_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0xFFU };
const uint16_t SET_MODE2_REQ_LEN = 6U;

// DMR to PCM Data
const uint8_t  MODE2_DATA_REQ[]   = { MARKER, 0x0DU, 0x00U, 0x05U, 0xA6U, 0xCBU, 0x80U, 0x27U, 0x20U, 0x4FU, 0x9BU, 0xCBU, 0xF3U };
const uint16_t MODE2_DATA_REQ_LEN = 13U;
const uint8_t  MODE2_DATA_REP[]   = { MARKER, 0x44U, 0x01U, 0x05U };
const uint16_t MODE2_DATA_REP_LEN = 4U;

// DMR to unknown Mode Set
const uint8_t  SET_MODE3_REQ[] = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x7FU };
const uint16_t SET_MODE3_REQ_LEN = 6U;

// Invalid command
const uint8_t  INVALID_REQ[]   = { MARKER, 0x04U, 0x00U, 0xA0U };
const uint16_t INVALID_REQ_LEN = 4U;

const uint8_t  ACK[]   = { MARKER, 0x04U, 0x00U, 0x03U };
const uint16_t ACK_LEN = 4U;

const uint8_t  NAK0[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x00U };
const uint16_t NAK0_LEN = 5U;

const uint8_t  NAK1[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x01U };
const uint16_t NAK1_LEN = 5U;

const uint8_t  NAK2[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x02U };
const uint16_t NAK2_LEN = 5U;

const uint8_t  NAK3[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x03U };
const uint16_t NAK3_LEN = 5U;

const uint8_t  NAK4[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x04U };
const uint16_t NAK4_LEN = 5U;

const uint8_t  NAK5[]   = { MARKER, 0x05U, 0x00U, 0x04U, 0x05U };
const uint16_t NAK5_LEN = 5U;

int main(int argc, char** argv)
{
    if (argc == 1) {
        ::fprintf(stderr, "Usage: Handler <port> [speed]\n");
        return 1;
    }

    std::string port = std::string(argv[1]);
    unsigned int speed = 460800U;

    if (argc == 3)
        speed = (unsigned int)atoi(argv[2]);

    CHandler handler(port, speed);

    return handler.run();
}

CHandler::CHandler(const std::string& device, unsigned int speed) :
m_serial(device, speed),
m_stopwatch()
{
	assert(!device.empty());
	assert(speed > 0U);
}

CHandler::~CHandler()
{
}

int CHandler::run()
{
    bool ret = m_serial.open();
    if (!ret)
        return 1;

    ret = test("Get Version", GET_VERSION_REQ, GET_VERSION_REQ_LEN, GET_VERSION_REP, GET_VERSION_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Get Capabilities", GET_CAPABILITIES_REQ, GET_CAPABILITIES_REQ_LEN, GET_CAPABILITIES_REP, GET_CAPABILITIES_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to PCM", SET_MODE1_REQ, SET_MODE1_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to PCM", MODE1_DATA_REQ, MODE1_DATA_REQ_LEN, MODE1_DATA_REP, MODE1_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR to PCM", SET_MODE2_REQ, SET_MODE2_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR to PCM", MODE2_DATA_REQ, MODE2_DATA_REQ_LEN, MODE2_DATA_REP, MODE2_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR to unknown", SET_MODE3_REQ, SET_MODE3_REQ_LEN, NAK2, NAK2_LEN);
    if (!ret)
        return 1;

    ret = test("Send invalid command", INVALID_REQ, INVALID_REQ_LEN, NAK0, NAK0_LEN);
    if (!ret)
        return 1;

    return 0;
}

bool CHandler::test(const char* title, const uint8_t* inData, uint16_t inLen, const uint8_t* outData, uint16_t outLen)
{
    assert(title != NULL);
    assert(inData != NULL);
    assert(inLen > 0U);

    ::fprintf(stdout, "%s\n", title);

    dump("Write", inData, inLen);

    int16_t ret = m_serial.write(inData, inLen);
    if (ret <= 0) {
        ::fprintf(stderr, "Error writing the data to the transcoder\n\n");
        return false;
    }

    uint8_t buffer[400U];
    uint16_t len = read(buffer, 100U);
    if (len == 0U) {
        printf("\n");
        return false;
    }

    dump("Read", buffer, len);

    if (outData != NULL) {
        if (::memcmp(buffer, outData, outLen) == 0) {
            printf("Data matches\n");
        } else {
            dump("Expected", outData, outLen);
            printf("Data does not match\n");
        }
    }

    printf("\n");

    return true;
}

void CHandler::dump(const char* title, const uint8_t* buffer, uint16_t length) const
{
    assert(title != NULL);
    assert(buffer != NULL);
    assert(length > 0U);

    ::fprintf(stdout, "%s\n", title);

    uint16_t offset = 0U;

    while (offset < length) {
        ::fprintf(stdout, "%04X: ", offset);

        for (uint16_t i = 0U; i < 16U; i++) {
            if ((offset + i) < length)
                ::fprintf(stdout, "%02X ", buffer[offset + i]);
            else
                ::fprintf(stdout, "   ");
        }

        ::fprintf(stdout, "  *");

        for (uint16_t i = 0U; i < 16U; i++) {
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

uint16_t CHandler::read(uint8_t* buffer, uint16_t timeout)
{
    assert(buffer != NULL);
    assert(timeout > 0U);

    uint16_t len = 0U;
    uint16_t ptr = 0U;

    m_stopwatch.start();

    for (;;) {
        uint8_t c = 0U;
        if (m_serial.read(&c, 1U) == 1) {
            if (ptr == 0U) {
                if (c == MARKER) {
                    // Handle the frame start correctly
                    buffer[0U] = c;
                    ptr = 1U;
                    len = 0U;
                } else {
                    ptr = 0U;
                    len = 0U;
                }
            } else if (ptr == 1U) {
                // Handle the frame length LSB
                uint8_t val = buffer[ptr] = c;
                len = (val << 0) & 0x00FFU;
                ptr = 2U;
            } else if (ptr == 2U) {
                // Handle the frame length MSB
                uint8_t val = buffer[ptr] = c;
                len |= (val << 8) & 0xFF00U;
                ptr = 3U;
            } else {
                // Any other bytes are added to the buffer
                buffer[ptr] = c;
                ptr++;

                // The full packet has been received, process it
                if (ptr == len) {
                    // ::fprintf(stdout, "Read has ended after %ld ms\n", millis() - start);
                    return len;
                }
            }
        } else {
            unsigned long elapsed = m_stopwatch.elapsed();
            if (elapsed > timeout) {
                ::fprintf(stderr, "Read has timed out after %u ms\n", timeout);
                return len;
            }
        }
    }
}
