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

const struct {
  uint16_t    m_input;
  uint16_t    m_output;
  uint16_t    m_length;
  IProcessor* m_step1;
  IProcessor* m_step2;
  IProcessor* m_step3;
} PROCESSOR_TABLE[] = {
  {MODE_DSTAR,       MODE_DSTAR,       9U, &dstarfec,      NULL,           NULL},
#if AMBE_TYPE > 1
  {MODE_DSTAR,       MODE_DMR_NXDN,    9U, &dstarpcm,      &pcmdmrnxdn,    NULL},
  {MODE_DSTAR,       MODE_YSFDN,       9U, &dstarpcm,      &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
#if AMBE_TYPE > 0
  {MODE_DSTAR,       MODE_YSFVW_P25,   9U, &dstarpcm,      &pcmysfvwp25,   NULL},
  {MODE_DSTAR,       MODE_CODEC2_3200, 9U, &dstarpcm,      &pcmcodec23200, NULL},
  {MODE_DSTAR,       MODE_CODEC2_1600, 9U, &dstarpcm,      &pcmcodec21600, NULL},
  {MODE_DSTAR,       MODE_PCM,         9U, &dstarpcm,      NULL,           NULL},
#endif

#if AMBE_TYPE > 1
  {MODE_DMR_NXDN,    MODE_DSTAR,       9U, &dmrnxdnpcm,    &pcmdstar,      NULL},
#endif
  {MODE_DMR_NXDN,    MODE_DMR_NXDN,    9U, &dmrnxdnfec,    NULL,           NULL},
  {MODE_DMR_NXDN,    MODE_YSFDN,       9U, &dmrnxdnysfdn,  NULL,           NULL},
#if AMBE_TYPE > 0
  {MODE_DMR_NXDN,    MODE_YSFVW_P25,   9U, &dmrnxdnpcm,    &pcmysfvwp25,   NULL},
  {MODE_DMR_NXDN,    MODE_CODEC2_3200, 9U, &dmrnxdnpcm,    &pcmcodec23200, NULL},
  {MODE_DMR_NXDN,    MODE_CODEC2_1600, 9U, &dmrnxdnpcm,    &pcmcodec21600, NULL},
  {MODE_DMR_NXDN,    MODE_PCM,         9U, &dmrnxdnpcm,    NULL,           NULL},
#endif

#if AMBE_TYPE > 1
  {MODE_YSFDN,       MODE_DSTAR,       9U, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmdstar},
#endif
  {MODE_YSFDN,       MODE_DMR_NXDN,    9U, &ysfdndmrnxdn,  NULL,           NULL},
  {MODE_YSFDN,       MODE_YSFDN,       9U, &ysfdnfec,      NULL,           NULL},
#if AMBE_TYPE > 0
  {MODE_YSFDN,       MODE_YSFVW_P25,   9U, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmysfvwp25},	// FIXME TODO length
  {MODE_YSFDN,       MODE_CODEC2_3200, 9U, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmcodec23200},
  {MODE_YSFDN,       MODE_CODEC2_1600, 9U, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmcodec21600},
  {MODE_YSFDN,       MODE_PCM,         9U, &ysfdndmrnxdn,  &dmrnxdnpcm,    NULL},
#endif

#if AMBE_TYPE > 0
  {MODE_YSFVW_P25,   MODE_DSTAR,      11U, &ysfvwp25pcm,   &pcmdstar,      NULL},
  {MODE_YSFVW_P25,   MODE_DMR_NXDN,   11U, &ysfvwp25pcm,   &pcmdmrnxdn,    NULL},
  {MODE_YSFVW_P25,   MODE_YSFDN,      11U, &ysfvwp25pcm,   &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_YSFVW_P25,   MODE_YSFVW_P25,  11U, &ysfvwp25fec,   NULL,           NULL},
  {MODE_YSFVW_P25,   MODE_CODEC2_3200, 9U, &ysfvwp25pcm,   &pcmcodec23200, NULL},	// FIXME TODO length
  {MODE_YSFVW_P25,   MODE_CODEC2_1600, 9U, &ysfvwp25pcm,   &pcmcodec21600, NULL},
  {MODE_YSFVW_P25,   MODE_PCM,         9U, &ysfvwp25pcm,   NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_CODEC2_3200, MODE_DSTAR,       9U, &codec23200pcm, &pcmdstar,      NULL},
  {MODE_CODEC2_3200, MODE_DMR_NXDN,    9U, &codec23200pcm, &pcmdmrnxdn,    NULL},
  {MODE_CODEC2_3200, MODE_YSFDN,       9U, &codec23200pcm, &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_CODEC2_3200, MODE_YSFVW_P25,   9U, &codec23200pcm, &pcmysfvwp25,   NULL},
  {MODE_CODEC2_3200, MODE_CODEC2_3200, 9U, NULL,           NULL,           NULL},	// FIXME TODO length
  {MODE_CODEC2_3200, MODE_CODEC2_1600, 9U, &codec23200pcm, &pcmcodec21600, NULL},
  {MODE_CODEC2_3200, MODE_PCM,         9U, &codec23200pcm, NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_CODEC2_1600, MODE_DSTAR,       9U, &codec21600pcm, &pcmdstar,      NULL},
  {MODE_CODEC2_1600, MODE_DMR_NXDN,    9U, &codec21600pcm, &pcmdmrnxdn,    NULL},
  {MODE_CODEC2_1600, MODE_YSFDN,       9U, &codec21600pcm, &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_CODEC2_1600, MODE_YSFVW_P25,   9U, &codec21600pcm, &pcmysfvwp25,   NULL},
  {MODE_CODEC2_1600, MODE_CODEC2_3200, 9U, &codec21600pcm, &pcmcodec23200, NULL},
  {MODE_CODEC2_1600, MODE_CODEC2_1600, 9U, NULL,           NULL,           NULL},	// FIXME TODO length
  {MODE_CODEC2_1600, MODE_PCM,         9U, &codec21600pcm, NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_PCM,         MODE_DSTAR,       320U, &pcmdstar,      NULL,           NULL},
  {MODE_PCM,         MODE_DMR_NXDN,    320U, &pcmdmrnxdn,    NULL,           NULL},
  {MODE_PCM,         MODE_YSFDN,       320U, &pcmdmrnxdn,    &dmrnxdnysfdn,  NULL},
#endif
  {MODE_PCM,         MODE_YSFVW_P25,   320U, &pcmysfvwp25,   NULL,           NULL},
  {MODE_PCM,         MODE_CODEC2_3200, 320U, &pcmcodec23200, NULL,           NULL},
  {MODE_PCM,         MODE_CODEC2_1600, 320U, &pcmcodec21600, NULL,           NULL},	// FIXME TODO length
  {MODE_PCM,         MODE_PCM,         320U, NULL,           NULL,           NULL}
};

const uint8_t PROCESSOR_LENGTH = sizeof(PROCESSOR_TABLE) / sizeof(PROCESSOR_TABLE[0U]);

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

#if defined(GITVERSION)
#define concat(a, b, c) a " " b " GitID #" c ""
const char HARDWARE[] = concat(VERSION, TCXO, GITVERSION);
#else
#define concat(a, b, c, d) a " " b " (Build: " c " " d ")"
const char HARDWARE[] = concat(VERSION, TCXO, __TIME__, __DATE__);
#endif

const uint8_t PROTOCOL_VERSION = 1U;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_opMode(OPMODE_NONE),
m_length(0U),
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

uint8_t CSerialPort::setMode(const uint8_t* buffer, uint16_t length)
{
  if (length != 2U) {
    DEBUG1("Malformed SET_MODE command");
    return 0x02U;
  }

  m_opMode = OPMODE_NONE;
  m_length = 0U;
  m_step1  = NULL;
  m_step2  = NULL;
  m_step3  = NULL;

  if ((buffer[0U] == MODE_PASS_THROUGH) && (buffer[1U] == MODE_PASS_THROUGH)) {
    m_opMode = OPMODE_PASSTHROUGH;
    return 0x00U;
  }

  for (uint8_t i = 0U; i < PROCESSOR_LENGTH; i++) {
    if ((PROCESSOR_TABLE[i].m_input == buffer[0U]) && (PROCESSOR_TABLE[i].m_output == buffer[1U])) {
      m_opMode = OPMODE_TRANSCODING;
      m_length = PROCESSOR_TABLE[i].m_length;
      m_step1  = PROCESSOR_TABLE[i].m_step1;
      m_step2  = PROCESSOR_TABLE[i].m_step2;
      m_step3  = PROCESSOR_TABLE[i].m_step3;
      return 0x00U;
    }
  }

  DEBUG3("Unknown SET_MODE command", buffer[0U], buffer[1U]);

  return 0x02U;
}

uint8_t CSerialPort::sendData(const uint8_t* buffer, uint16_t length)
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
      if (length != m_length) {
        DEBUG3("Invalid data length for the mode", length, m_length);
        return 0x04U;
      }

      if (m_step1 != NULL) {
        // Start the pipeline
        return m_step1->input(buffer, length);
      } else {
        // Nothing to do, just send back out
        writeData(buffer, length);
        return 0x00U;
      }
  }
}

void CSerialPort::processData()
{
  if (m_opMode == OPMODE_TRANSCODING) {
    uint8_t  buffer[250U];
    uint16_t length = 0U;

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

void CSerialPort::processMessage(uint8_t type, const uint8_t* buffer, uint16_t length)
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

void CSerialPort::writeData(const uint8_t* data, uint16_t length)
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

  uint16_t pos = 0U;

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

void CSerialPort::reverse(uint8_t* buffer, uint16_t length) const
{
  uint16_t start = 0U;
  uint16_t end = length - 1U;

  while (start < end) {
    uint8_t temp  = buffer[start];
    buffer[start] = buffer[end];
    buffer[end]   = temp;

    end--;
    start++;
  }
}

