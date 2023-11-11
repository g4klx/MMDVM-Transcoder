/*
 *   Copyright (C) 2013,2015-2021,2023 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

#include "Config.h"
#include "Globals.h"

#if defined(MADEBYMAKEFILE)
#include "GitVersion.h"
#endif

#include "SerialPort.h"
#include "Version.h"

const uint8_t MMDVM_FRAME_START         = 0xE1U;

const uint8_t MMDVM_GET_VERSION         = 0x00U;
const uint8_t MMDVM_RETURN_VERSION      = 0x00U;
const uint8_t MMDVM_GET_CAPABILITIES    = 0x01U;
const uint8_t MMDVM_RETURN_CAPABILITIES = 0x01U;
const uint8_t MMDVM_SET_MODE            = 0x02U;
const uint8_t MMDVM_ACK                 = 0x03U;
const uint8_t MMDVM_NAK                 = 0x04U;
const uint8_t MMDVM_DATA                = 0x05U;

const uint8_t MMDVM_DEBUG               = 0xFFU;

#if EXTERNAL_OSC == 12000000
#define TCXO "12.0000 MHz"
#elif EXTERNAL_OSC == 12288000
#define TCXO "12.2880 MHz"
#elif EXTERNAL_OSC == 14400000
#define TCXO "14.4000 MHz"
#elif EXTERNAL_OSC == 19200000
#define TCXO "19.2000 Mhz"
#else
#define TCXO "NO TCXO"
#endif

#if defined(DRCC_DVM_NQF)
#define	HW_TYPE	"MMDVM DRCC_DVM_NQF"
#elif defined(DRCC_DVM_HHP446)
#define	HW_TYPE	"MMDVM DRCC_DVM_HHP(446)"
#elif defined(DRCC_DVM_722)
#define HW_TYPE "MMDVM RB_STM32_DVM(722)"
#elif defined(DRCC_DVM_446)
#define HW_TYPE "MMDVM RB_STM32_DVM(446)"
#else
#define	HW_TYPE	"MMDVM"
#endif

#if defined(GITVERSION)
#define concat(h, a, b, c) h " " a " " b " GitID #" c ""
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, GITVERSION);
#else
#define concat(h, a, b, c, d) h " " a " " b " (Build: " c " " d ")"
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, __TIME__, __DATE__);
#endif

const uint8_t PROTOCOL_VERSION   = 1U;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_debug(false)
{
}

void CSerialPort::sendACK()
{
  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_ACK;

  writeInt(1U, reply, 3);
}

void CSerialPort::sendNAK(uint8_t err)
{
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_NAK;
  reply[3U] = err;

  writeInt(1U, reply, 4);
}

void CSerialPort::getVersion()
{
  uint8_t reply[200U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_RETURN_VERSION;

  reply[3U] = PROTOCOL_VERSION;

  // CPU type/manufacturer. 0=Atmel ARM, 1=NXP ARM, 2=St-Micro ARM
  reply[4U] = io.getCPU();

  // Reserve 16 bytes for the UDID
  ::memset(reply + 5U, 0x00U, 16U);
  io.getUDID(reply + 5U);

  uint8_t count = 21U;
  for (uint8_t i = 0U; HARDWARE[i] != 0x00U; i++, count++)
    reply[count] = HARDWARE[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::getCapabilities()
{
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_RETURN_CAPABILITIES;

  reply[3U] = AMBE_TYPE;

  writeInt(1U, reply, 4);
}

void CSerialPort::start()
{
  beginInt(1U, SERIAL_SPEED);

#if AMBE_TYPE != 0
  beginInt(3U, DVSI_SPEED);
#endif
}

void CSerialPort::process()
{
  while (availableForReadInt(1U)) {
    uint8_t c = readInt(1U);

    if (m_ptr == 0U) {
      if (c == MMDVM_FRAME_START) {
        // Handle the frame start correctly
        m_buffer[0U] = c;
        m_ptr = 1U;
        m_len = 0U;
      } else {
        m_ptr = 0U;
        m_len = 0U;
      }
    } else if (m_ptr == 1U) {
      // Handle the frame length
      m_len = m_buffer[m_ptr] = c;
      m_ptr = 2U;
    } else {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // The full packet has been received, process it
      if (m_ptr == m_len)
        processMessage(m_buffer[2U], m_buffer + 3U, m_len - 3U);
    }
  }
}

void CSerialPort::processMessage(uint8_t type, const uint8_t* buffer, uint16_t length)
{
  uint8_t err = 0x00U;

  switch (type) {
    case MMDVM_GET_VERSION:
      getVersion();
      break;

    case MMDVM_GET_CAPABILITIES:
      getCapabilities();
      break;

    case MMDVM_SET_MODE:
      err = setMode(buffer, length);
      if (err == 0U)
        sendACK();
      else
        sendNAK(err);
      break;

    case MMDVM_DATA:
      if (err != 0U) {
        DEBUG2("Received invalid data", err);
        sendNAK(err);
      }
      break;

    default:
      // Handle this, send a NAK back
      sendNAK(0x00U);
      break;
  }

  m_ptr = 0U;
  m_len = 0U;
}

void CSerialPort::writeData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_NXDN && m_modemState != STATE_IDLE)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG2;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG3;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG4;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG5;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[count++] = (n4 >> 8) & 0xFF;
  reply[count++] = (n4 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebugDump(const uint8_t* data, uint16_t length)
{
  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}
