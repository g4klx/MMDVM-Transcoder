/*
 *   Copyright (C) 2023,2024 by Jonathan Naylor G4KLX
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
#include "DVSIDriver.h"

#if AMBE_TYPE > 0

#include "Globals.h"

#define USART6_TX PG14      // Arduino D1
#define USART6_RX PG9       // Arduino D0

#define AMBE3000_RESET  PF13    // Arduino D7
#define AMBE3000_RTS    PA3     // Arduino A0

const uint8_t DVSI_START_BYTE = 0x61U;

HardwareSerial SerialAMBE(USART6_RX, USART6_TX);

CDVSIDriver::CDVSIDriver() :
m_buffer3000(),
m_len3000(0U),
m_ptr3000(0U)
#if AMBE_TYPE == 3
,m_buffer4020(),
m_len4020(0U),
m_ptr4020(0U)
#endif
{
}

void CDVSIDriver::startup3000()
{
  SerialAMBE.begin(DVSI_AMBE3000_SPEED);

  pinMode(AMBE3000_RESET, OUTPUT);
  pinMode(AMBE3000_RTS, INPUT);
}

#if AMBE_TYPE == 3
void CDVSIDriver::startup4020()
{
}
#endif

void CDVSIDriver::reset3000()
{
  digitalWrite(AMBE3000_RESET, LOW);

  delay(100);

  digitalWrite(AMBE3000_RESET, HIGH);
}

#if AMBE_TYPE == 3
void CDVSIDriver::reset4020()
{
}
#endif

bool CDVSIDriver::RTS3000() const
{
  return digitalRead(AMBE3000_RTS) == HIGH;
}

#if AMBE_TYPE == 3
bool CDVSIDriver::RTS4020() const
{
  return false;
}
#endif

void CDVSIDriver::write3000(const uint8_t* buffer, uint16_t length)
{
  SerialAMBE.write(buffer, length);
}

#if AMBE_TYPE == 3
void CDVSIDriver::write4020(const uint8_t* buffer, uint16_t length)
{
}
#endif

uint16_t CDVSIDriver::read3000(uint8_t* buffer)
{
  while (SerialAMBE.available()) {
    uint8_t c = SerialAMBE.read();

    if (m_ptr3000 == 0U) {
      if (c == DVSI_START_BYTE) {
        // Handle the frame start correctly
        m_buffer3000[0U] = c;
        m_ptr3000 = 1U;
        m_len3000 = 0U;
      } else {
        m_ptr3000 = 0U;
        m_len3000 = 0U;
      }
    } else if (m_ptr3000 == 1U) {
      // Handle the frame length MSB
      uint8_t val = m_buffer3000[m_ptr3000] = c;
      m_len3000 = (val << 8) & 0xFF00U;
      m_ptr3000 = 2U;
    } else if (m_ptr3000 == 2U) {
      // Handle the frame length LSB
      uint8_t val = m_buffer3000[m_ptr3000] = c;
      m_len3000 |= (val << 0) & 0x00FFU;
      m_len3000 += 4U;	// The length in the DVSI message doesn't include the first four bytes
      m_ptr3000  = 3U;
    } else {
      // Any other bytes are added to the buffer
      m_buffer3000[m_ptr3000] = c;
      m_ptr3000++;

      // The full packet has been received, process it
      if (m_ptr3000 == m_len3000) {
        ::memcpy(buffer, m_buffer3000, m_len3000);
        uint16_t length = m_len3000;

        m_ptr3000 = 0U;
        m_len3000 = 0U;

        return length;
      }
    }
  }

  return 0U;
}

#if AMBE_TYPE == 3
uint16_t CDVSIDriver::read4020(uint8_t* buffer)
{
  while (serial.availableForReadInt(3U)) {
    uint8_t c = serial.readInt(3U);

    if (m_ptr4020 == 0U) {
      if (c == DVSI_START_BYTE) {
        // Handle the frame start correctly
        m_buffer4020[0U] = c;
        m_ptr4020 = 1U;
        m_len4020 = 0U;
      } else {
        m_ptr4020 = 0U;
        m_len4020 = 0U;
      }
    } else if (m_ptr4020 == 1U) {
      // Handle the frame length MSB
      uint8_t val = m_buffer4020[m_ptr4020] = c;
      m_len4020 = (val << 8) & 0xFF00U;
      m_ptr4020 = 2U;
    } else if (m_ptr4020 == 2U) {
      // Handle the frame length LSB
      uint8_t val = m_buffer4020[m_ptr4020] = c;
      m_len4020 |= (val << 0) & 0x00FFU;
      m_len4020 += 4U;	// The length in the DVSI message doesn't include the first four bytes
      m_ptr4020  = 3U;
    } else {
      // Any other bytes are added to the buffer
      m_buffer4020[m_ptr4020] = c;
      m_ptr4020++;

      // The full packet has been received, process it
      if (m_ptr4020 == m_len4020) {
        ::memcpy(buffer, m_buffer4020, m_len4020);
        uint16_t length = m_len4020;

        m_ptr4020 = 0U;
        m_len4020 = 0U;

        return length;
      }
    }
  }

  return 0U;
}
#endif

#endif
