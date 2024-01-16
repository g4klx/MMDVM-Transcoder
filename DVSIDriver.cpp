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

#include "Globals.h"
#include "Config.h"

// Reset   AMBE3000	PA8    output
// Reset   AMBE4020	       output

const uint8_t DVSI_START_BYTE = 0x61U;

CDVSIDriver::CDVSIDriver() :
m_buffer3000(),
m_len3000(0U),
m_ptr3000(0U),
m_buffer4020(),
m_len4020(0U),
m_ptr4020(0U)
{
}

void CDVSIDriver::startup3000()
{
  serial.beginInt(2U, DVSI_AMBE3000_SPEED);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_StructInit(&GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // The reset pin
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;

  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // The RTS input, ignored
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;

  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
}

void CDVSIDriver::startup4020()
{
  serial.beginInt(3U, DVSI_AMBE4020_SPEED);

  // Setup AMBE4020 RTS and Reset pins
}

void CDVSIDriver::reset3000()
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);

  // FIXME TODO Wait for approximately 100ms
  for (uint32_t i = 0U; i < 100U; i++) {
    for (uint32_t j = 0U; j < 100000U; j++)
      __NOP();
  }

  GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
}

void CDVSIDriver::reset4020()
{
}

bool CDVSIDriver::RTS3000() const
{
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET;
}

bool CDVSIDriver::RTS4020() const
{
  return false;
}

void CDVSIDriver::write3000(const uint8_t* buffer, uint16_t length)
{
  serial.writeInt(2U, buffer, length);
}

void CDVSIDriver::write4020(const uint8_t* buffer, uint16_t length)
{
  serial.writeInt(3U, buffer, length);
}

uint16_t CDVSIDriver::read3000(uint8_t* buffer)
{
  while (serial.availableForReadInt(2U)) {
    uint8_t c = serial.readInt(2U);

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

