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
} PROCESSOR_TABLE[] = {
  {MODE_DSTAR,       MODE_DSTAR,       &dstarfec,      nullptr},
#if AMBE_TYPE > 0
#if AMBE_TYPE == 2
  {MODE_DSTAR,       MODE_DMR_NXDN,    &dstarpcm,      &pcmdmrnxdn},
  {MODE_DSTAR,       MODE_YSFDN,       &dstarpcm,      &pcmysfdn},
#else
  {MODE_DSTAR,       MODE_DMR_NXDN,    &dstardmrnxdn,  nullptr},
  {MODE_DSTAR,       MODE_YSFDN,       &dstarysfdn,    nullptr},
#endif
  {MODE_DSTAR,       MODE_YSFVW_P25,   &dstarpcm,      &pcmysfvwp25},
  {MODE_DSTAR,       MODE_CODEC2_3200, &dstarpcm,      &pcmcodec23200},
  {MODE_DSTAR,       MODE_PCM,         &dstarpcm,      nullptr},
#endif

#if AMBE_TYPE > 0
#if AMBE_TYPE == 2
  {MODE_DMR_NXDN,    MODE_DSTAR,       &dmrnxdnpcm,    &pcmdstar},
#else
  {MODE_DMR_NXDN,    MODE_DSTAR,       &dmrnxdndstar,  nullptr},
#endif
  {MODE_DMR_NXDN,    MODE_DMR_NXDN,    &dmrnxdnfec,    nullptr},
  {MODE_DMR_NXDN,    MODE_YSFDN,       &dmrnxdnfec,    &dmrnxdnysfdn},
  {MODE_DMR_NXDN,    MODE_YSFVW_P25,   &dmrnxdnpcm,    &pcmysfvwp25},
  {MODE_DMR_NXDN,    MODE_CODEC2_3200, &dmrnxdnpcm,    &pcmcodec23200},
  {MODE_DMR_NXDN,    MODE_PCM,         &dmrnxdnpcm,    nullptr},
#endif

#if AMBE_TYPE > 0
#if AMBE_TYPE == 2
  {MODE_YSFDN,       MODE_DSTAR,       &ysfdnpcm,      &pcmdstar},
#else
  {MODE_YSFDN,       MODE_DSTAR,       &ysfdndstar,    nullptr},
#endif
  {MODE_YSFDN,       MODE_DMR_NXDN,    &ysfdnfec,      &ysfdndmrnxdn},
  {MODE_YSFDN,       MODE_YSFDN,       &ysfdnfec,      nullptr},
  {MODE_YSFDN,       MODE_YSFVW_P25,   &ysfdnpcm,      &pcmysfvwp25},
  {MODE_YSFDN,       MODE_CODEC2_3200, &ysfdnpcm,      &pcmcodec23200},
  {MODE_YSFDN,       MODE_PCM,         &ysfdnpcm,      nullptr},
#endif

#if AMBE_TYPE > 0
  {MODE_YSFVW_P25,   MODE_DSTAR,       &ysfvwp25pcm,   &pcmdstar},
  {MODE_YSFVW_P25,   MODE_DMR_NXDN,    &ysfvwp25pcm,   &pcmdmrnxdn},
  {MODE_YSFVW_P25,   MODE_YSFDN,       &ysfvwp25pcm,   &pcmysfdn},
#endif
  {MODE_YSFVW_P25,   MODE_YSFVW_P25,   &ysfvwp25fec,   nullptr},
  {MODE_YSFVW_P25,   MODE_CODEC2_3200, &ysfvwp25pcm,   &pcmcodec23200},
  {MODE_YSFVW_P25,   MODE_PCM,         &ysfvwp25pcm,   nullptr},

#if AMBE_TYPE > 0
  {MODE_CODEC2_3200, MODE_DSTAR,       &codec23200pcm, &pcmdstar},
  {MODE_CODEC2_3200, MODE_DMR_NXDN,    &codec23200pcm, &pcmdmrnxdn},
  {MODE_CODEC2_3200, MODE_YSFDN,       &codec23200pcm, &pcmysfdn},
#endif
  {MODE_CODEC2_3200, MODE_YSFVW_P25,   &codec23200pcm, &pcmysfvwp25},
  {MODE_CODEC2_3200, MODE_CODEC2_3200, nullptr,           nullptr},
  {MODE_CODEC2_3200, MODE_PCM,         &codec23200pcm, nullptr},

#if AMBE_TYPE > 0
  {MODE_PCM,         MODE_DSTAR,       &pcmdstar,      nullptr},
  {MODE_PCM,         MODE_DMR_NXDN,    &pcmdmrnxdn,    nullptr},
  {MODE_PCM,         MODE_YSFDN,       &pcmysfdn,      nullptr},
#endif
  {MODE_PCM,         MODE_YSFVW_P25,   &pcmysfvwp25,   nullptr},
  {MODE_PCM,         MODE_CODEC2_3200, &pcmcodec23200, nullptr},
  {MODE_PCM,         MODE_PCM,         nullptr,           nullptr}
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

const unsigned long MAX_COMMAND_TIME_MS = 30UL;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_start(0UL),
m_step1(nullptr),
m_step2(nullptr)
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

  if (m_step1 != nullptr) {
    m_step1->finish();
    m_step1 = nullptr;
  }

  if (m_step2 != nullptr) {
    m_step2->finish();
    m_step2 = nullptr;
  }

  opmode = OPMODE_NONE;

#if AMBE_TYPE > 0
  if ((buffer[0U] == MODE_PASS_THROUGH) && (buffer[1U] == MODE_PASS_THROUGH)) {
    opmode = OPMODE_PASSTHROUGH;
    dvsi.reset();
    return 0x00U;
  }
#endif

  for (uint8_t i = 0U; i < PROCESSOR_LENGTH; i++) {
    if ((PROCESSOR_TABLE[i].m_input == buffer[0U]) && (PROCESSOR_TABLE[i].m_output == buffer[1U])) {
      opmode  = OPMODE_TRANSCODING;
      m_step1 = PROCESSOR_TABLE[i].m_step1;
      m_step2 = PROCESSOR_TABLE[i].m_step2;

      if (m_step1 != nullptr) {
        int8_t ret = m_step1->init(0U);
        if (ret != 0x00U)
          return ret;
      }

      if (m_step2 != nullptr) {
        int8_t ret = m_step2->init(1U);
        if (ret != 0x00U)
          return ret;
      }

      return 0x00U;
    }
  }

  DEBUG3("Unknown SET_MODE command", buffer[0U], buffer[1U]);

  return 0x02U;
}

uint8_t CSerialPort::sendData(const uint8_t* buffer, uint16_t length)
{
  switch (opmode) {
    case OPMODE_NONE:
      DEBUG1("Received data in None mode");
      return 0x03U;

    case OPMODE_PASSTHROUGH:
      // If the RTS pin is high, then the chip does not expect any more data to be sent through
      // if (!dvsi.ready3000()) {
      //   DEBUG1("The AMBE3003/3000 chip is not ready to receive any more data");
      //   return 0x05U;
      // }
      dvsi.write(buffer, length);
      return 0x00U;

    case OPMODE_TRANSCODING:
    default:
      if (m_step1 != nullptr) {
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
  uint8_t buffer[500U];
  int16_t length = 0;

  if (opmode == OPMODE_TRANSCODING) {
    if (m_step1 != nullptr) {
      uint8_t err = m_step1->process();
      if (err != 0x00U) {
        sendNAK(err);
        return;
      }

      length = m_step1->output(buffer);
      if (length < 0) {
        sendNAK(-length);
        return;
      }
    }

    if ((m_step2 != nullptr) && (length > 0)) {
      m_step2->input(buffer, length);
      length = 0U;
    }

    if (m_step2 != nullptr) {
      uint8_t err = m_step2->process();
      if (err != 0x00U) {
        sendNAK(err);
        return;
      }

      length = m_step2->output(buffer);
      if (length < 0) {
        sendNAK(-length);
        return;
      }
    }

    if (length > 0)
      writeData(buffer, length);
  } else if (opmode == OPMODE_PASSTHROUGH) {
    length = dvsi.read(buffer);

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
        m_start = millis();
        m_ptr   = 1U;
        m_len   = 0U;
      } else {
        m_ptr   = 0U;
        m_len   = 0U;
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
        m_start = 0UL;
        processMessage(m_buffer[3U], m_buffer + 4U, m_len - 4U);
      }
    }
  }

  if (m_start > 0UL) {
    unsigned long now = millis();
    if ((now - m_start) >= MAX_COMMAND_TIME_MS) {
      DEBUG1("Command took too long to be completed");
      m_ptr   = 0U;
      m_len   = 0U;
      m_start = 0UL;
      sendNAK(0x04U);
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
  if (opmode == OPMODE_NONE)
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

#if defined(DEBUGGING)
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
#endif

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
