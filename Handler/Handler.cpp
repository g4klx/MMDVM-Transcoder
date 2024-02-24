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

const uint8_t  GET_VERSION[]   = { MARKER, 0x04U, 0x00U, 0x00U};
const uint16_t GET_VERSION_LEN = 4U;

const uint8_t  GET_CAPABILITIES[]   = { MARKER, 0x04U, 0x00U, 0x01U };
const uint16_t GET_CAPABILITIES_LEN = 4U;

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

    ret = test("Get Version", GET_VERSION, GET_VERSION_LEN);
    // if (!ret)
    //    return 1;

    ret = test("Get Capabilities", GET_CAPABILITIES, GET_CAPABILITIES_LEN);
    // if (!ret)
    //    return 1;

    return 0;
}

bool CHandler::test(const char* title, const uint8_t* data, uint16_t length)
{
    assert(title != NULL);
    assert(data != NULL);
    assert(length > 0U);

    ::fprintf(stdout, "%s\n", title);

    dump("Write", data, length);

    int16_t ret = m_serial.write(data, length);
    if (ret <= 0) {
        ::fprintf(stderr, "Error writing the data to the transcoder\n");
        return false;
    }

    uint8_t buffer[400U];
    uint16_t len = read(buffer, 100U);
    if (len == 0U)
        return false;

    dump("Read", buffer, len);

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
            printf("Read: %02X ", c);
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
/*
            unsigned long elapsed = m_stopwatch.elapsed();
            if (elapsed > timeout) {
                ::fprintf(stderr, "Read has timed out after %u ms\n", timeout);
                return len;
            }
*/
        }
    }
}
