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

const uint8_t  GET_VERSION_REQ[]   = { MARKER, 0x04U, 0x00U, 0x00U};
const uint16_t GET_VERSION_REQ_LEN = 4U;
const uint8_t  GET_VERSION_REP[]   = { MARKER, 0x2BU, 0x00U, 0x00U, 0x01U, 0x32U, 0x30U, 0x32U, 0x34U };
const uint16_t GET_VERSION_REP_LEN = 9U;

const uint8_t  GET_CAPABILITIES_REQ[]   = { MARKER, 0x04U, 0x00U, 0x01U };
const uint16_t GET_CAPABILITIES_REQ_LEN = 4U;
const uint8_t  GET_CAPABILITIES_REP[]   = { MARKER, 0x05U, 0x00U, 0x01U, 0x01U };
const uint16_t GET_CAPABILITIES_REP_LEN = 5U;

const uint8_t  PCM_DATA[] = { 0xE1U, 0x44U, 0x01U, 0x05U, 0x44U, 0xE3U, 0x1EU, 0xE3U, 0x1EU, 0xE5U, 0x97U, 0xF1U, 0x61U, 0x11U,
                                0xC7U, 0x38U, 0xFAU, 0x58U, 0x31U, 0x5CU, 0xC5U, 0x42U, 0x9AU, 0x18U, 0xCFU, 0xF6U, 0x69U, 0xEAU,
                                0xC4U, 0xF3U, 0x20U, 0x03U, 0x51U, 0x0AU, 0x50U, 0x02U, 0x9CU, 0xF1U, 0x04U, 0xE2U, 0x96U, 0xDBU,
                                0xADU, 0xDDU, 0xCEU, 0xE3U, 0xE1U, 0xE8U, 0xB0U, 0xECU, 0xADU, 0xF0U, 0xE5U, 0xF6U, 0xC3U, 0xFDU,
                                0x3BU, 0x03U, 0x0BU, 0x05U, 0xBEU, 0x04U, 0x85U, 0x04U, 0x19U, 0x07U, 0xBCU, 0x0BU, 0xA6U, 0x10U,
                                0xD8U, 0x12U, 0x1FU, 0x12U, 0x28U, 0x0FU, 0x6FU, 0x0CU, 0x82U, 0x0AU, 0xADU, 0x09U, 0x98U, 0x08U,
                                0x60U, 0x07U, 0xC7U, 0x05U, 0x82U, 0x04U, 0xA4U, 0x02U, 0xF4U, 0xFFU, 0x5DU, 0xFBU, 0x36U, 0xF6U,
                                0x36U, 0xF1U, 0x71U, 0xEEU, 0x23U, 0xEDU, 0x2CU, 0xEDU, 0x23U, 0xECU, 0xAEU, 0xEAU, 0xC6U, 0xE7U,
                                0xBCU, 0xE5U, 0x25U, 0xE3U, 0x63U, 0xE3U, 0x98U, 0xE5U, 0xE9U, 0xF3U, 0xB7U, 0x14U, 0x2BU, 0x3CU,
                                0x44U, 0x5AU, 0x9CU, 0x5AU, 0x06U, 0x3FU, 0xB4U, 0x14U, 0xADU, 0xF4U, 0x87U, 0xEAU, 0x41U, 0xF5U,
                                0x55U, 0x04U, 0x42U, 0x0AU, 0xFAU, 0x00U, 0xF6U, 0xEFU, 0xE7U, 0xE0U, 0x6AU, 0xDBU, 0x13U, 0xDEU,
                                0x4EU, 0xE4U, 0x25U, 0xE9U, 0xC9U, 0xECU, 0xC4U, 0xF0U, 0xFAU, 0xF6U, 0xEFU, 0xFDU, 0x72U, 0x03U,
                                0x2EU, 0x05U, 0xE9U, 0x04U, 0xDAU, 0x04U, 0xB9U, 0x07U, 0x86U, 0x0CU, 0x69U, 0x11U, 0x60U, 0x13U,
                                0x6FU, 0x12U, 0x54U, 0x0FU, 0xA2U, 0x0CU, 0xBCU, 0x0AU, 0xECU, 0x09U, 0xBFU, 0x08U, 0x76U, 0x07U,
                                0xBCU, 0x05U, 0x5CU, 0x04U, 0x4DU, 0x02U, 0x71U, 0xFFU, 0xADU, 0xFAU, 0x80U, 0xF5U, 0x92U, 0xF0U,
                                0xFCU, 0xEDU, 0xC2U, 0xECU, 0xD2U, 0xECU, 0xA7U, 0xEBU, 0x22U, 0xEAU, 0x1EU, 0xE7U, 0x24U, 0xE5U,
                                0x83U, 0xE2U, 0x1FU, 0xE3U, 0xB2U, 0xE5U, 0x42U, 0xF6U, 0xA2U, 0x18U, 0xBEU, 0x40U, 0xF5U, 0x5CU,
                                0x55U, 0x5AU, 0x17U, 0x3CU, 0x28U, 0x11U, 0x81U, 0xF2U, 0x84U, 0xEAU, 0x9DU, 0xF6U, 0x7AU, 0x05U,
                                0x1BU, 0x0AU, 0x84U, 0xFFU, 0x18U, 0xEEU, 0x8AU, 0xDFU, 0xF2U, 0xDAU, 0x30U, 0xDEU, 0x89U, 0xE4U,
                                0x50U, 0xE9U, 0x19U, 0xEDU, 0x6FU, 0xF1U, 0x0FU, 0xF8U, 0xE3U, 0xFEU, 0xE0U, 0x03U, 0x20U, 0x05U,
                                0xB7U, 0x04U, 0xDCU, 0x04U, 0x09U, 0x08U, 0xF1U, 0x0CU, 0xA9U, 0x11U, 0x4AU, 0x13U, 0x1CU, 0x12U,
                                0xF5U, 0x0EU, 0x64U, 0x0CU, 0x9DU, 0x0AU, 0xDCU, 0x09U, 0xAAU, 0x08U, 0x61U, 0x07U, 0xA6U, 0x05U,
                                0x36U, 0x04U, 0xFAU, 0x01U, 0xE4U, 0xFEU, 0xFAU, 0xF9U, 0xDBU, 0xF4U, 0x21U, 0xF0U, 0xC2U, 0xEDU,
                                0xCAU, 0xECU };
const uint16_t PCM_DATA_REQ_LEN = 324U;
const uint16_t PCM_DATA_REP_LEN = 4U;

const uint8_t  DSTAR_DATA[] = { MARKER, 0x0DU, 0x00U, 0x05U, 0x02U, 0x19U, 0x17U, 0xE4U, 0xB3U, 0xE2U, 0x00U, 0xA2U, 0x20U };
const uint16_t DSTAR_DATA_REQ_LEN = 13U;
const uint16_t DSTAR_DATA_REP_LEN = 4U;

const uint8_t  DMRNXDN_DATA[] = { MARKER, 0x0DU, 0x00U, 0x05U, 0xA6U, 0xCBU, 0x80U, 0x27U, 0x20U, 0x4FU, 0x9BU, 0xCBU, 0xF3U };
const uint16_t DMRNXDN_DATA_REQ_LEN = 13U;
const uint16_t DMRNXDN_DATA_REP_LEN = 4U;

const uint8_t  YSFDN_DATA[] = { MARKER, 0x11U, 0x00U, 0x05U, 0xE0U, 0x01U, 0xC0U, 0xFFU, 0xFFU, 0xC0U, 0x00U, 0x70U, 0x3FU, 0xFCU, 0x40U, 0x62U, 0x8AU };
const uint16_t YSFDN_DATA_REQ_LEN = 17U;
const uint16_t YSFDN_DATA_REP_LEN = 4U;

const uint8_t  IMBE_FEC_DATA[] = { MARKER, 0x16U, 0x00U, 0x05U, 0x0AU, 0x02U, 0x25U, 0x32U, 0x21U, 0xE6U, 0x77U, 0x0AU, 0x5DU, 0xAFU, 0xF7U, 0xAEU, 0x44U, 0xC0U, 0xB8U, 0xBEU, 0x97U, 0x97U };
const uint16_t IMBE_FEC_DATA_REQ_LEN = 22U;
const uint16_t IMBE_FEC_DATA_REP_LEN = 4U;

const uint8_t  CODEC23200_DATA[] = { MARKER, 0x0CU, 0x00U, 0x05U, 0xD9U, 0x49U, 0xA5U, 0x59U, 0x08U, 0x7DU, 0x4EU, 0x99U };
const uint16_t CODEC23200_DATA_REQ_LEN = 12U;
const uint16_t CODEC23200_DATA_REP_LEN = 4U;

/* D-Star */

// D-Star to PCM Mode Set
const uint8_t  SET_MODE1A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0xFFU };
const uint16_t SET_MODE1A_REQ_LEN = 6U;

// D-Star to D-Star Mode Set
const uint8_t  SET_MODE1B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0x01U };
const uint16_t SET_MODE1B_REQ_LEN = 6U;

// D-Star to IMBE FEC Mode Set
const uint8_t  SET_MODE1C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0x05U };
const uint16_t SET_MODE1C_REQ_LEN = 6U;

// D-Star to Codec2 3200 Mode Set
const uint8_t  SET_MODE1D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0x06U };
const uint16_t SET_MODE1D_REQ_LEN = 6U;

// D-Star to DMR/NXDN Mode Set
const uint8_t  SET_MODE1F_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0x02U };
const uint16_t SET_MODE1F_REQ_LEN = 6U;

// D-Star to YSF DN Mode Set
const uint8_t  SET_MODE1G_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x01U, 0x03U };
const uint16_t SET_MODE1G_REQ_LEN = 6U;

/* DMR/NXDN */

// DMR/NXDN to PCM Mode Set
const uint8_t  SET_MODE2A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0xFFU };
const uint16_t SET_MODE2A_REQ_LEN = 6U;

// DMR/NXDN to DMR/NXDN Mode Set
const uint8_t  SET_MODE2B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x02U };
const uint16_t SET_MODE2B_REQ_LEN = 6U;

// DMR/NXDN to YSF DN Mode Set
const uint8_t  SET_MODE2C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x03U };
const uint16_t SET_MODE2C_REQ_LEN = 6U;

// DMR/NXDN to IMBE FEC Mode Set
const uint8_t  SET_MODE2D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x05U };
const uint16_t SET_MODE2D_REQ_LEN = 6U;

// DMR/NXDN to Codec2 3200 Mode Set
const uint8_t  SET_MODE2E_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x06U };
const uint16_t SET_MODE2E_REQ_LEN = 6U;

// DMR/NXDN to D-Star Mode Set
const uint8_t  SET_MODE2G_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x01U };
const uint16_t SET_MODE2G_REQ_LEN = 6U;

/* YSF DN */

// YSF DN to PCM Mode Set
const uint8_t  SET_MODE3A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0xFFU };
const uint16_t SET_MODE3A_REQ_LEN = 6U;

// YSF DN to YSF DN Mode Set
const uint8_t  SET_MODE3B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0x03U };
const uint16_t SET_MODE3B_REQ_LEN = 6U;

// YSF DN to DMR/NXDN Mode Set
const uint8_t  SET_MODE3C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0x02U };
const uint16_t SET_MODE3C_REQ_LEN = 6U;

// YSF DN to IMBE FEC Mode Set
const uint8_t  SET_MODE3D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0x05U };
const uint16_t SET_MODE3D_REQ_LEN = 6U;

// YSF DN to Codec2 3200 Mode Set
const uint8_t  SET_MODE3E_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0x06U };
const uint16_t SET_MODE3E_REQ_LEN = 6U;

// YSF DN to D-Star Mode Set
const uint8_t  SET_MODE3G_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x03U, 0x01U };
const uint16_t SET_MODE3G_REQ_LEN = 6U;

/* IMBE FEC */

// IMBE FEC to PCM Mode Set
const uint8_t  SET_MODE4A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0xFFU };
const uint16_t SET_MODE4A_REQ_LEN = 6U;

// IMBE FEC to YSF/VW P25 FEC Mode Set
const uint8_t  SET_MODE4B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0x05U };
const uint16_t SET_MODE4B_REQ_LEN = 6U;

// IMBE FEC to D-Star Mode Set
const uint8_t  SET_MODE4C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0x01U };
const uint16_t SET_MODE4C_REQ_LEN = 6U;

// IMBE FEC to DMR/NXDN Mode Set
const uint8_t  SET_MODE4D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0x02U };
const uint16_t SET_MODE4D_REQ_LEN = 6U;

// IMBE FEC to YSF DN Mode Set
const uint8_t  SET_MODE4E_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0x03U };
const uint16_t SET_MODE4E_REQ_LEN = 6U;

// IMBE FEC to Codec2 3200 Mode Set
const uint8_t  SET_MODE4F_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x05U, 0x06U };
const uint16_t SET_MODE4F_REQ_LEN = 6U;

/* Codec2 3200 */

// Codec2 3200 to PCM Mode Set
const uint8_t  SET_MODE5A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0xFFU };
const uint16_t SET_MODE5A_REQ_LEN = 6U;

// Codec2 3200 to Codec2 3200 Mode Set
const uint8_t  SET_MODE5B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0x06U };
const uint16_t SET_MODE5B_REQ_LEN = 6U;

// Codec2 3200 to D-Star Mode Set
const uint8_t  SET_MODE5C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0x01U };
const uint16_t SET_MODE5C_REQ_LEN = 6U;

// Codec2 3200 to DMR/NXDN Mode Set
const uint8_t  SET_MODE5D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0x02U };
const uint16_t SET_MODE5D_REQ_LEN = 6U;

// Codec2 3200 to YSF DN Mode Set
const uint8_t  SET_MODE5E_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0x03U };
const uint16_t SET_MODE5E_REQ_LEN = 6U;

// Codec2 3200 to IMBE FEC Mode Set
const uint8_t  SET_MODE5F_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x06U, 0x05U };
const uint16_t SET_MODE5F_REQ_LEN = 6U;

/* PCM */

// PCM to D-Star Mode Set
const uint8_t  SET_MODE9A_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0x01U };
const uint16_t SET_MODE9A_REQ_LEN = 6U;

// PCM to DMR/NXDN Mode Set
const uint8_t  SET_MODE9B_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0x02U };
const uint16_t SET_MODE9B_REQ_LEN = 6U;

// PCM to YSF DN Mode Set
const uint8_t  SET_MODE9C_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0x03U };
const uint16_t SET_MODE9C_REQ_LEN = 6U;

// PCM to IMBE FEC Mode Set
const uint8_t  SET_MODE9D_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0x05U };
const uint16_t SET_MODE9D_REQ_LEN = 6U;

// PCM to Codec2 3200 Mode Set
const uint8_t  SET_MODE9E_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0x06U };
const uint16_t SET_MODE9E_REQ_LEN = 6U;

// PCM to PCM Mode Set
const uint8_t  SET_MODE9G_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0xFFU, 0xFFU };
const uint16_t SET_MODE9G_REQ_LEN = 6U;

/* Passthrough */

// Passthrough Mode Set
const uint8_t  SET_MODEPA_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x00U, 0x00U };
const uint16_t SET_MODEPA_REQ_LEN = 6U;

/* Get the AMBE3000 Product Id */
const uint8_t  MODEPB_DATA_REQ[]   = { MARKER, 0x09U, 0x00U, 0x05U, 0x61U, 0x00U, 0x01U, 0x00U, 0x30U };
const uint16_t MODEPB_DATA_REQ_LEN = 9U;

const uint8_t  MODEPB_DATA_REP[]   = { MARKER, 0x13U, 0x00U, 0x05U, 0x61U, 0x00U, 0x0BU, 0x00U, 0x30U };
const uint16_t MODEPB_DATA_REP_LEN = 9U;

/* Get the AMBE3000 Version */
const uint8_t  MODEPC_DATA_REQ[]   = { MARKER, 0x09U, 0x00U, 0x05U, 0x61U, 0x00U, 0x01U, 0x00U, 0x31U };
const uint16_t MODEPC_DATA_REQ_LEN = 9U;

const uint8_t  MODEPC_DATA_REP[]   = { MARKER, 0x39U, 0x00U, 0x05U, 0x61U, 0x00U, 0x31U, 0x00U, 0x31U };
const uint16_t MODEPC_DATA_REP_LEN = 9U;

/* Error Cases */

// DMR to unknown Mode Set
const uint8_t  SET_MODEN_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U, 0x02U, 0x7FU };
const uint16_t SET_MODEN_REQ_LEN = 6U;

// Invalid command
const uint8_t  INVALID_REQ[]   = { MARKER, 0x04U, 0x00U, 0xA0U };
const uint16_t INVALID_REQ_LEN = 4U;

// Malformed command
const uint8_t  MALFORMED_REQ[]   = { MARKER, 0x06U, 0x00U, 0x02U };
const uint16_t MALFORMED_REQ_LEN = 4U;


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
m_stopwatch(),
m_count(0U),
m_ok(0U),
m_failed(0U)
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

    printf("Basic Functionality\n");

    ret = test("Get Version", GET_VERSION_REQ, GET_VERSION_REQ_LEN, GET_VERSION_REP, GET_VERSION_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Get Capabilities", GET_CAPABILITIES_REQ, GET_CAPABILITIES_REQ_LEN, GET_CAPABILITIES_REP, GET_CAPABILITIES_REP_LEN);
    if (!ret)
        return 1;

    printf("\nD-Star\n");

    ret = test("Set Mode D-Star to PCM", SET_MODE1A_REQ, SET_MODE1A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to PCM", DSTAR_DATA, DSTAR_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to D-Star", SET_MODE1B_REQ, SET_MODE1B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to D-Star", DSTAR_DATA, DSTAR_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to DMR/NXDN", SET_MODE1F_REQ, SET_MODE1F_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to DMR/NXDN", DSTAR_DATA, DSTAR_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to YSF DN", SET_MODE1G_REQ, SET_MODE1G_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to YSF DN", DSTAR_DATA, DSTAR_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to IMBE FEC", SET_MODE1C_REQ, SET_MODE1C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to IMBE FEC", DSTAR_DATA, DSTAR_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode D-Star to Codec2 3200", SET_MODE1D_REQ, SET_MODE1D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode D-Star to Codec2 3200", DSTAR_DATA, DSTAR_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nDMR/NXDN\n");

    ret = test("Set Mode DMR/NXDN to PCM", SET_MODE2A_REQ, SET_MODE2A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to PCM", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR/NXDN to DMR/NXDN", SET_MODE2B_REQ, SET_MODE2B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to DMR/NXDN", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR/NXDN to D-Star", SET_MODE2G_REQ, SET_MODE2G_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to D-Star", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR/NXDN to YSF DN", SET_MODE2C_REQ, SET_MODE2C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to YSF DN", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR/NXDN to IMBE FEC", SET_MODE2D_REQ, SET_MODE2D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to IMBE FEC", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode DMR/NXDN to Codec2 3200", SET_MODE2E_REQ, SET_MODE2E_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode DMR/NXDN to Codec2 3200", DMRNXDN_DATA, DMRNXDN_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nYSF DN\n");

    ret = test("Set Mode YSF DN to PCM", SET_MODE3A_REQ, SET_MODE3A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to PCM", YSFDN_DATA, YSFDN_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode YSF DN to YSF DN", SET_MODE3B_REQ, SET_MODE3B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to YSF DN", YSFDN_DATA, YSFDN_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode YSF DN to D-Star", SET_MODE3G_REQ, SET_MODE3G_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to D-Star", YSFDN_DATA, YSFDN_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode YSF DN to DMR/NXDN", SET_MODE3C_REQ, SET_MODE3C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to DMR/NXDN", YSFDN_DATA, YSFDN_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode YSF DN to IMBE FEC", SET_MODE3D_REQ, SET_MODE3D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to IMBE FEC", YSFDN_DATA, YSFDN_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode YSF DN to Codec2 3200", SET_MODE3E_REQ, SET_MODE3E_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode YSF DN to Codec2 3200", YSFDN_DATA, YSFDN_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nIMBE FEC\n");

    ret = test("Set Mode IMBE FEC to PCM", SET_MODE4A_REQ, SET_MODE4A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to PCM", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode IMBE FEC to IMBE FEC", SET_MODE4B_REQ, SET_MODE4B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to IMBE FEC", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode IMBE FEC to D-Star", SET_MODE4C_REQ, SET_MODE4C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to D-Star", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode IMBE FEC to DMR/NXDN", SET_MODE4D_REQ, SET_MODE4D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to DMR/NXDN", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode IMBE FEC to YSF DN", SET_MODE4E_REQ, SET_MODE4E_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to YSF DN", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode IMBE FEC to Codec2 3200", SET_MODE4F_REQ, SET_MODE4F_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode IMBE FEC to Codec2 3200", IMBE_FEC_DATA, IMBE_FEC_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nCodec2 3200\n");

    ret = test("Set Mode Codec2 3200 to PCM", SET_MODE5A_REQ, SET_MODE5A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to PCM", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode Codec2 3200 to Codec2 3200", SET_MODE5B_REQ, SET_MODE5B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to Codec2 3200", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REQ_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode Codec2 3200 to D-Star", SET_MODE5C_REQ, SET_MODE5C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to D-Star", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode Codec2 3200 to DMR/NXDN", SET_MODE5D_REQ, SET_MODE5D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to DMR/NXDN", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode Codec2 3200 to YSF DN", SET_MODE5E_REQ, SET_MODE5E_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to YSF DN", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode Codec2 3200 to IMBE FEC", SET_MODE5F_REQ, SET_MODE5F_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode Codec2 3200 to IMBE FEC", CODEC23200_DATA, CODEC23200_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nPCM\n");

    ret = test("Set Mode PCM to D-Star", SET_MODE9A_REQ, SET_MODE9A_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to D-Star", PCM_DATA, PCM_DATA_REQ_LEN, DSTAR_DATA, DSTAR_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode PCM to DMR/NXDN", SET_MODE9B_REQ, SET_MODE9B_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to DMR/NXDN", PCM_DATA, PCM_DATA_REQ_LEN, DMRNXDN_DATA, DMRNXDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode PCM to YSF DN", SET_MODE9C_REQ, SET_MODE9C_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to YSF DN", PCM_DATA, PCM_DATA_REQ_LEN, YSFDN_DATA, YSFDN_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode PCM to IMBE FEC", SET_MODE9D_REQ, SET_MODE9D_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to IMBE FEC", PCM_DATA, PCM_DATA_REQ_LEN, IMBE_FEC_DATA, IMBE_FEC_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode PCM to Codec2 3200", SET_MODE9E_REQ, SET_MODE9E_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to Codec2 3200", PCM_DATA, PCM_DATA_REQ_LEN, CODEC23200_DATA, CODEC23200_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Set Mode PCM to PCM", SET_MODE9G_REQ, SET_MODE9G_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Transcode PCM to PCM", PCM_DATA, PCM_DATA_REQ_LEN, PCM_DATA, PCM_DATA_REQ_LEN);
    if (!ret)
        return 1;

    printf("\nPassthrough\n");

    ret = test("Set Passthrough Mode", SET_MODEPA_REQ, SET_MODEPA_REQ_LEN, ACK, ACK_LEN);
    if (!ret)
        return 1;

    ret = test("Get AMBE3000 Product Id", MODEPB_DATA_REQ, MODEPB_DATA_REQ_LEN, MODEPB_DATA_REP, MODEPB_DATA_REP_LEN);
    if (!ret)
        return 1;

    ret = test("Get AMBE3000 Version", MODEPC_DATA_REQ, MODEPC_DATA_REQ_LEN, MODEPC_DATA_REP, MODEPC_DATA_REP_LEN);
    if (!ret)
        return 1;

    printf("\nError Cases\n");

    ret = test("Set Mode DMR to unknown", SET_MODEN_REQ, SET_MODEN_REQ_LEN, NAK2, NAK2_LEN);
    if (!ret)
        return 1;

    ret = test("Send invalid command", INVALID_REQ, INVALID_REQ_LEN, NAK0, NAK0_LEN);
    if (!ret)
        return 1;

    ret = test("Send malformed command", MALFORMED_REQ, MALFORMED_REQ_LEN, NAK4, NAK4_LEN);
    if (!ret)
        return 1;

    printf("\nNo tests: %u, ok: %u (%.1f%%), failed: %u (%.1f%%)\n", m_count, m_ok, 100.0F * float(m_ok) / float(m_count), m_failed, 100.0F * float(m_failed) / float(m_count));

    return 0;
}

bool CHandler::test(const char* title, const uint8_t* inData, uint16_t inLen, const uint8_t* outData, uint16_t outLen)
{
    assert(title != NULL);
    assert(inData != NULL);
    assert(inLen > 0U);

    m_count++;

    CStopWatch stopwatch;

    ::fprintf(stdout, "%s", title);

    // dump("Write", inData, inLen);

    stopwatch.start();

    int16_t ret = m_serial.write(inData, inLen);
    if (ret <= 0) {
        ::fprintf(stderr, "Error writing the data to the transcoder\n\n");
        m_failed++;
        return false;
    }

    uint8_t buffer[400U];
    uint16_t len = read(buffer, 200U);
    if (len == 0U) {
        printf(", Timeout (200 me)\n");
        m_failed++;
        return true;
    }

    unsigned int elapsed = stopwatch.elapsed();

    //  dump("Read", buffer, len);

    if (outData != NULL) {
        if (::memcmp(buffer, outData, outLen) == 0) {
            printf(", OK (%.1f ms)\n", float(elapsed) / 1000.0F);
            m_ok++;
        } else {
            printf(", Failed (%.1f ms)\n", float(elapsed) / 1000.0F);
            dump("Expected", outData, outLen);
            dump("Read", buffer, len);
            printf("\n");
            m_failed++;
        }
    }

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
            unsigned long elapsed = m_stopwatch.elapsed() / 1000U;
            if (elapsed > timeout) {
                ::fprintf(stderr, "Read has timed out after %u ms\n", timeout);
                return len;
            }
        }
    }
}
