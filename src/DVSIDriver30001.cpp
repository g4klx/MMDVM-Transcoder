/*
 *   Copyright (C) 2023,2024,2025 by Jonathan Naylor G4KLX
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
#include "DVSIDriver30001.h"

#if AMBE_TYPE == 1 || AMBE_TYPE == 2

#include "Globals.h"

#if defined(NUCLEO_STM32F722ZE)
#define USART_TX        PG14     // Arduino D1
#define USART_RX        PG9      // Arduino D0
#define AMBE_RESET      PF13     // Arduino D7
#define AMBE_RTS        PA3      // Arduino A0
#elif defined(NUCLEO_STM32H723ZG)
#define USART_TX        PB6      // Arduino D1
#define USART_RX        PB7      // Arduino D0
#define AMBE_RESET      PG12     // Arduino D7
#define AMBE_RTS        PA3      // Arduino A0
#else
#error "Unknown hardware"
#endif

CDVSIDriver30001::CDVSIDriver30001() :
CDVSIDriver(USART_RX, USART_TX, AMBE_RESET, AMBE_RTS)
{
}

#endif
