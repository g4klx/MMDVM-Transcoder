/*
 *   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
 *   Copyright (c) 2017,2023 by Jonathan Naylor G4KLX
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

#include "SerialPort.h"

/*
Pin definitions:

- Host communication:
USART1 - TXD PA9  - RXD PA10

- AMBE communication:
USART2 - TXD PA2  - RXD PA3  - RTS PA1
*/

#include "STMUART.h"

extern "C" {
   void USART1_IRQHandler();
   void USART2_IRQHandler();
}

// FIXME TODO Check the function of the RTS pin

/* ************* USART1 ***************** */

static CSTMUART m_USART1;

void USART1_IRQHandler()
{
  m_USART1.handleIRQ();
}

void InitUSART1(int speed, bool flowControl)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10; //  Tx | Rx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);

   USART_Cmd(USART1, ENABLE);

   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

   m_USART1.init(USART1);
}

/* ************* USART2 ***************** */

static CSTMUART m_USART2;

void USART2_IRQHandler()
{
   m_USART2.handleIRQ();
}

void InitUSART2(int speed, bool flowControl)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
   if (flowControl)
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_USART2);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = USART2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   if (flowControl)
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //  RTS | Tx | Rx
   else
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //  Tx | Rx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = flowControl ? USART_HardwareFlowControl_RTS : USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);

   USART_Cmd(USART2, ENABLE);

   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

   m_USART2.init(USART2);
}

/////////////////////////////////////////////////////////////////

void CSerialPort::beginInt(uint8_t n, int speed, bool flowControl)
{
   switch (n) {
      case 1U:
         InitUSART1(speed, flowControl);
         break;
      case 3U:
         InitUSART2(speed, flowControl);
         break;
      default:
         break;
   }
}

int CSerialPort::availableForReadInt(uint8_t n)
{
   switch (n) {
      case 1U:
         return m_USART1.available();
      case 3U:
         return m_USART2.available();
      default:
         return 0;
   }
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
   switch (n) {
      case 1U:
         return m_USART1.availableForWrite();
      case 3U:
         return m_USART2.availableForWrite();
      default:
         return 0;
   }
}

uint8_t CSerialPort::readInt(uint8_t n)
{
   switch (n) {
      case 1U:
         return m_USART1.read();
      case 3U:
         return m_USART2.read();
      default:
         return 0U;
   }
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
   switch (n) {
      case 1U:
         m_USART1.write(data, length);
         if (flush)
            m_USART1.flush();
         break;
      case 3U:
         m_USART2.write(data, length);
         if (flush)
            m_USART2.flush();
         break;
      default:
         break;
   }
}

