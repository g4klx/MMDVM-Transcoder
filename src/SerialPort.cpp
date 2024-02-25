/*
 *   Copyright (C) 2013,2015-2021,2023,2024 by Jonathan Naylor G4KLX
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

#include "ModeDefines.h"
#include "SerialPort.h"
#include "Version.h"

const struct {
  uint16_t    m_input;
  uint16_t    m_output;
  IProcessor* m_step1;
  IProcessor* m_step2;
  IProcessor* m_step3;
} PROCESSOR_TABLE[] = {
  {MODE_DSTAR,       MODE_DSTAR,       &dstarfec,      NULL,           NULL},
#if AMBE_TYPE > 1
  {MODE_DSTAR,       MODE_DMR_NXDN,    &dstarpcm,      &pcmdmrnxdn,    NULL},
  {MODE_DSTAR,       MODE_YSFDN,       &dstarpcm,      &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
#if AMBE_TYPE > 0
  {MODE_DSTAR,       MODE_YSFVW_P25,   &dstarpcm,      &pcmysfvwp25,   NULL},
  {MODE_DSTAR,       MODE_CODEC2_3200, &dstarpcm,      &pcmcodec23200, NULL},
  {MODE_DSTAR,       MODE_CODEC2_1600, &dstarpcm,      &pcmcodec21600, NULL},
  {MODE_DSTAR,       MODE_PCM,         &dstarpcm,      NULL,           NULL},
#endif

#if AMBE_TYPE > 1
  {MODE_DMR_NXDN,    MODE_DSTAR,       &dmrnxdnpcm,    &pcmdstar,      NULL},
#endif
  {MODE_DMR_NXDN,    MODE_DMR_NXDN,    &dmrnxdnfec,    NULL,           NULL},
  {MODE_DMR_NXDN,    MODE_YSFDN,       &dmrnxdnfec,    &dmrnxdnysfdn,  NULL},
#if AMBE_TYPE > 0
  {MODE_DMR_NXDN,    MODE_YSFVW_P25,   &dmrnxdnpcm,    &pcmysfvwp25,   NULL},
  {MODE_DMR_NXDN,    MODE_CODEC2_3200, &dmrnxdnpcm,    &pcmcodec23200, NULL},
  {MODE_DMR_NXDN,    MODE_CODEC2_1600, &dmrnxdnpcm,    &pcmcodec21600, NULL},
  {MODE_DMR_NXDN,    MODE_PCM,         &dmrnxdnpcm,    NULL,           NULL},
#endif

#if AMBE_TYPE > 1
  {MODE_YSFDN,       MODE_DSTAR,       &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmdstar},
#endif
  {MODE_YSFDN,       MODE_DMR_NXDN,    &ysfdnfec,      &ysfdndmrnxdn,  NULL},
  {MODE_YSFDN,       MODE_YSFDN,       &ysfdnfec,      NULL,           NULL},
#if AMBE_TYPE > 0
  {MODE_YSFDN,       MODE_YSFVW_P25,   &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmysfvwp25},
  {MODE_YSFDN,       MODE_CODEC2_3200, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmcodec23200},
  {MODE_YSFDN,       MODE_CODEC2_1600, &ysfdndmrnxdn,  &dmrnxdnpcm,    &pcmcodec21600},
  {MODE_YSFDN,       MODE_PCM,         &ysfdndmrnxdn,  &dmrnxdnpcm,    NULL},
#endif

#if AMBE_TYPE > 0
  {MODE_YSFVW_P25,   MODE_DSTAR,       &ysfvwp25pcm,   &pcmdstar,      NULL},
  {MODE_YSFVW_P25,   MODE_DMR_NXDN,    &ysfvwp25pcm,   &pcmdmrnxdn,    NULL},
  {MODE_YSFVW_P25,   MODE_YSFDN,       &ysfvwp25pcm,   &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_YSFVW_P25,   MODE_YSFVW_P25,   &ysfvwp25fec,   NULL,           NULL},
  {MODE_YSFVW_P25,   MODE_CODEC2_3200, &ysfvwp25pcm,   &pcmcodec23200, NULL},
  {MODE_YSFVW_P25,   MODE_CODEC2_1600, &ysfvwp25pcm,   &pcmcodec21600, NULL},
  {MODE_YSFVW_P25,   MODE_PCM,         &ysfvwp25pcm,   NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_CODEC2_3200, MODE_DSTAR,       &codec23200pcm, &pcmdstar,      NULL},
  {MODE_CODEC2_3200, MODE_DMR_NXDN,    &codec23200pcm, &pcmdmrnxdn,    NULL},
  {MODE_CODEC2_3200, MODE_YSFDN,       &codec23200pcm, &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_CODEC2_3200, MODE_YSFVW_P25,   &codec23200pcm, &pcmysfvwp25,   NULL},
  {MODE_CODEC2_3200, MODE_CODEC2_3200, NULL,           NULL,           NULL},
  {MODE_CODEC2_3200, MODE_CODEC2_1600, &codec23200pcm, &pcmcodec21600, NULL},
  {MODE_CODEC2_3200, MODE_PCM,         &codec23200pcm, NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_CODEC2_1600, MODE_DSTAR,       &codec21600pcm, &pcmdstar,      NULL},
  {MODE_CODEC2_1600, MODE_DMR_NXDN,    &codec21600pcm, &pcmdmrnxdn,    NULL},
  {MODE_CODEC2_1600, MODE_YSFDN,       &codec21600pcm, &pcmdmrnxdn,    &dmrnxdnysfdn},
#endif
  {MODE_CODEC2_1600, MODE_YSFVW_P25,   &codec21600pcm, &pcmysfvwp25,   NULL},
  {MODE_CODEC2_1600, MODE_CODEC2_3200, &codec21600pcm, &pcmcodec23200, NULL},
  {MODE_CODEC2_1600, MODE_CODEC2_1600, NULL,           NULL,           NULL},
  {MODE_CODEC2_1600, MODE_PCM,         &codec21600pcm, NULL,           NULL},

#if AMBE_TYPE > 0
  {MODE_PCM,         MODE_DSTAR,       &pcmdstar,      NULL,           NULL},
  {MODE_PCM,         MODE_DMR_NXDN,    &pcmdmrnxdn,    NULL,           NULL},
  {MODE_PCM,         MODE_YSFDN,       &pcmdmrnxdn,    &dmrnxdnysfdn,  NULL},
#endif
  {MODE_PCM,         MODE_YSFVW_P25,   &pcmysfvwp25,   NULL,           NULL},
  {MODE_PCM,         MODE_CODEC2_3200, &pcmcodec23200, NULL,           NULL},
  {MODE_PCM,         MODE_CODEC2_1600, &pcmcodec21600, NULL,           NULL},
  {MODE_PCM,         MODE_PCM,         NULL,           NULL,           NULL}
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

// These pins connect to the STLink chip and show up as a VCP to the host over USB working ok
#define USART3_TX PD8
#define USART3_RX PD9

#if defined(HAS_STLINK)
HardwareSerial SerialSTLink(USART3_RX, USART3_TX);
#endif

#define concat(a, b, c) a " (Build: " b " " c ")"
const char HARDWARE[] = concat(VERSION, __TIME__, __DATE__);

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
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_ACK;

  SerialUSB.write(reply, 4);
}

void CSerialPort::sendNAK(uint8_t err)
{
  uint8_t reply[5U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 5U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_NAK;
  reply[4U] = err;

  SerialUSB.write(reply, 5);
}

void CSerialPort::getVersion()
{
  uint8_t reply[200U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_RETURN_VERSION;

  reply[4U] = PROTOCOL_VERSION;

  uint16_t count = 5U;
  for (uint16_t i = 0U; HARDWARE[i] != 0x00U; i++, count++)
    reply[count] = HARDWARE[i];

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
}

void CSerialPort::getCapabilities()
{
  uint8_t reply[5U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 5U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_RETURN_CAPABILITIES;

  reply[4U] = AMBE_TYPE;

  SerialUSB.write(reply, 5);
}

void CSerialPort::start()
{
  SerialUSB.begin(SERIAL_SPEED);

#if defined(HAS_STLINK)
	// This is the uart that goes through the STLink chip and becomes a VCP 
	SerialSTLink.begin(460800);
#endif
}

uint8_t CSerialPort::setMode(const uint8_t* buffer, uint16_t length)
{
  if (length != 2U) {
    DEBUG1("Malformed SET_MODE command");
    return 0x02U;
  }

  m_opMode = OPMODE_NONE;
  m_step1  = NULL;
  m_step2  = NULL;
  m_step3  = NULL;

#if AMBE_TYPE > 0
  if ((buffer[0U] == MODE_PASS_THROUGH) && (buffer[1U] == MODE_PASS_THROUGH)) {
    m_opMode = OPMODE_PASSTHROUGH;
    dvsi.reset3000();
#if AMBE_TYPE == 3
    dvsi.reset4020();
#endif
    return 0x00U;
  }
#endif

  for (uint8_t i = 0U; i < PROCESSOR_LENGTH; i++) {
    if ((PROCESSOR_TABLE[i].m_input == buffer[0U]) && (PROCESSOR_TABLE[i].m_output == buffer[1U])) {
      m_opMode = OPMODE_TRANSCODING;
      m_step1  = PROCESSOR_TABLE[i].m_step1;
      m_step2  = PROCESSOR_TABLE[i].m_step2;
      m_step3  = PROCESSOR_TABLE[i].m_step3;

      if (m_step1 != NULL)
        m_step1->init(0U);
      if (m_step2 != NULL)
        m_step2->init(1U);
      if (m_step3 != NULL)
        m_step3->init(2U);

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
      // If the RTS pin is high, then the chip does not expect any more data to be sent through
      if (dvsi.RTS3000()) {
        DEBUG1("The AMBE3000 chip is not ready to receive any more data");
        return 0x05U;
      }
      dvsi.write3000(buffer, length);
      return 0x00U;

    case OPMODE_TRANSCODING:
    default:
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
  uint8_t  buffer[500U];
  uint16_t length = 0U;

  if (m_opMode == OPMODE_TRANSCODING) {
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
    length = dvsi.read3000(buffer);

    if (length > 0U)
      writeData(buffer, length);
  }
}

void CSerialPort::process()
{
  while (SerialUSB.available() > 0) {
    uint8_t c = SerialUSB.read();

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
      uint8_t val = m_buffer[m_ptr] = c;
      m_len = (val << 0) & 0x00FFU;
      m_ptr = 2U;
    } else if (m_ptr == 2U) {
      // Handle the frame length
      uint8_t val = m_buffer[m_ptr] = c;
      m_len |= (val << 8) & 0xFF00U;
      m_ptr  = 3U;
    } else {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // The full packet has been received, process it
      if (m_ptr == m_len) {
        dump("Command", m_buffer, m_len);
        processMessage(m_buffer[3U], m_buffer + 4U, m_len - 4U);
      }
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
      DEBUG2("Invalid command received", type);
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

  uint8_t reply[500U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DATA;

  uint16_t count = 4U;
  for (uint16_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
}

void CSerialPort::writeDebug(const char* text)
{
#if defined(HAS_STLINK)
  SerialSTLink.printf("Debug: \"%s\"\r\n", text);
#else
  uint8_t reply[300U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DEBUG;

  uint16_t count = 4U;
  for (uint16_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
#endif
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
#if defined(HAS_STLINK)
  SerialSTLink.printf("Debug: \"%s\" %u\r\n", text, n1);
#else
  uint8_t reply[300U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DEBUG;

  uint16_t count = 4U;
  for (uint16_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
#endif
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
#if defined(HAS_STLINK)
  SerialSTLink.printf("Debug: \"%s\" %u %u\r\n", text, n1, n2);
#else
  uint8_t reply[300U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DEBUG;

  uint16_t count = 4U;
  for (uint16_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
#endif
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
#if defined(HAS_STLINK)
  SerialSTLink.printf("Debug: \"%s\" %u %u %u\r\n", text, n1, n2, n3);
#else
  uint8_t reply[300U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DEBUG;

  uint16_t count = 4U;
  for (uint16_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[count++] = ' ';
  count += convert(n3, reply + count);

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
#endif
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
#if defined(HAS_STLINK)
  SerialSTLink.printf("Debug: \"%s\" %u %u %u %u\r\n", text, n1, n2, n3, n4);
#else
  uint8_t reply[300U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = 0U;
  reply[3U] = MMDVM_DEBUG;

  uint16_t count = 4U;
  for (uint16_t i = 0U; text[i] != '\x0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  count += convert(n1, reply + count);

  reply[count++] = ' ';
  count += convert(n2, reply + count);

  reply[count++] = ' ';
  count += convert(n3, reply + count);

  reply[count++] = ' ';
  count += convert(n4, reply + count);

  reply[1U] = (count >> 0) & 0xFFU;
  reply[2U] = (count >> 8) & 0xFFU;

  SerialUSB.write(reply, count);
#endif
}

uint16_t CSerialPort::convert(int16_t num, uint8_t* buffer)
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

#if defined(HAS_STLINK)
void CSerialPort::dump(const char* title, const uint8_t* buffer, uint16_t length) const
{
	uint16_t offset = 0U;

  SerialSTLink.println(title);

  while (offset < length) {
		SerialSTLink.printf("%04X: ", offset);

		for (uint16_t i = 0U; i < 16U; i++) {
			if ((offset + i) < length)
				SerialSTLink.printf("%02X ", buffer[offset + i]);
			else
				SerialSTLink.print("   ");
		}

		SerialSTLink.print("  *");

		for (uint16_t i = 0U; i < 16U; i++) {
			if ((offset + i) < length) {
				if (isprint(buffer[offset + i]))
					SerialSTLink.printf("%c", buffer[offset + i]);
				else
					SerialSTLink.print(".");
			}
		}

		SerialSTLink.println("*");

		offset += 16U;
	}
}
#endif
