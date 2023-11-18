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

#include "ModeDefines.h"
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

const uint8_t PROTOCOL_VERSION = 1U;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_opMode(OPMODE_NONE),
m_step1(NULL),
m_step2(NULL),
m_step3(NULL)
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

  uint8_t count = 4U;
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
  beginInt(1U, SERIAL_SPEED, false);

#if AMBE_TYPE != 0
  beginInt(3U, DVSI_SPEED, true);
#endif
}

uint8_t CSerialPort::setMode(const uint8_t* buffer, uint8_t length)
{
  if (length != 2U) {
    DEBUG1("Malformed SET_MODE command");
    return 0x02U;
  }

  m_step1 = NULL;
  m_step2 = NULL;
  m_step3 = NULL;

  uint16_t mode = (buffer[0U] << 8) | buffer[1U];

  switch (mode) {
    case ((MODE_DSTAR << 8) | MODE_DSTAR):
      return 0x00U;

    default:
      DEBUG1("Unknown mode combination");
      return 0x02U;
  }
}

uint8_t CSerialPort::sendData(const uint8_t* buffer, uint8_t length)
{
  switch (m_opMode) {
    case OPMODE_NONE:
      DEBUG1("Received data in None mode");
      return 0x03U;

    case OPMODE_PASSTHROUGH:
      // FIXME TODO
      return 0x00U;

    case OPMODE_TRANSCODING:
    default:
      if (m_step1 != NULL)
        return m_step1->input(buffer, length);
      else
        return 0x00U;
  }
}

void CSerialPort::processData()
{
  if (m_opMode == OPMODE_TRANSCODING) {
    uint8_t buffer[250U];
    uint8_t length = 0U;

    if (m_step1 != NULL)
      length = m_step1->output(buffer);

    if ((m_step2 != NULL) && (length > 0U)) {
      m_step2->input(buffer, length);
      length = 0U;
    }

    if (m_step2 != NULL)
      length = m_step2->output(buffer);

    if ((m_step3 != NULL) && (length > 0U)) {
      m_step3->input(buffer, length);
      length = 0U;
    }

    if (m_step3 != NULL)
      length = m_step3->output(buffer);

    if (length > 0U)
      writeData(buffer, length);
  } else if (m_opMode == OPMODE_PASSTHROUGH) {
    // FIXME TODO
  }
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

  processData();
}

void CSerialPort::processMessage(uint8_t type, const uint8_t* buffer, uint8_t length)
{
  uint8_t err;

  switch (type) {
    case MMDVM_GET_VERSION:
      getVersion();
      break;

    case MMDVM_GET_CAPABILITIES:
      getCapabilities();
      break;

    case MMDVM_SET_MODE:
      err = setMode(buffer, length);
      if (err == 0x00U)
        sendACK();
      else
        sendNAK(err);
      break;

    case MMDVM_DATA:
      err = sendData(buffer, length);
      if (err != 0x00U)
        sendNAK(err);
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
  if (m_opMode == OPMODE_NONE)
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
  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[count++] = ' ';
  count += convert(n3, reply + count);

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[count++] = ' ';
  count += convert(n3, reply + count);

  reply[count++] = ' ';
  count += convert(n4, reply + count);

  reply[1U] = count;

  writeInt(1U, reply, count);
}

uint8_t CSerialPort::convert(int16_t num, uint8_t* buffer)
{
  if (num == 0) {
    *buffer = '0';
    return 1U;
  }

  bool isNegative = false;

  if (num < 0) {
    isNegative = true;
    num = -num;
  }

  uint8_t pos = 0U;

  while (num != 0) {
    int16_t rem = num % 10;
    buffer[pos++] = rem + '0';
    num /= 10;
  }

  if (isNegative)
    buffer[pos++] = '-';

  reverse(buffer, pos);

  return pos;
}

void CSerialPort::reverse(uint8_t* buffer, uint8_t length) const
{
  uint8_t start = 0U;
  uint8_t end = length - 1U;

  while (start < end) {
    uint8_t temp  = buffer[start];
    buffer[start] = buffer[end];
    buffer[end]   = temp;

    end--;
    start++;
  }
}

