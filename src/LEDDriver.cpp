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

#include "LEDDriver.h"

#if defined(HAS_LEDS)

#include "Globals.h"

#if defined(NUCLEO_STM32F722ZE)
#if AMBE_TYPE == 2
#define STAT1_LED   PA7       // Arduino D11
#define STAT2_LED   PA6       // Arduino D12
#define STAT3_LED   PD15      // Arduino D9
#define STAT4_LED   PD14      // Arduino D10
#else
#define STAT1_LED   PA7       // Arduino D11
#define STAT2_LED   PA6       // Arduino D12
#endif
#elif defined(NUCLEO_STM32H723ZG)
#if AMBE_TYPE == 2
#define STAT1_LED   PB5       // Arduino D11
#define STAT2_LED   PA6       // Arduino D12
#define STAT3_LED   PD15      // Arduino D9
#define STAT4_LED   PD14      // Arduino D10
#else
#define STAT1_LED   PB5       // Arduino D11
#define STAT2_LED   PA6       // Arduino D12
#endif
#else
#error "Unknown hardware"
#endif

CLEDDriver::CLEDDriver()
{
}

void CLEDDriver::startup()
{
  pinMode(STAT1_LED, OUTPUT);
  pinMode(STAT2_LED, OUTPUT);
#if AMBE_TYPE == 2
  pinMode(STAT3_LED, OUTPUT);
  pinMode(STAT4_LED, OUTPUT);
#endif

  for (uint8_t count = 0U; count < 10U; count++) {
    digitalWrite(STAT1_LED, LOW);
    digitalWrite(STAT2_LED, HIGH);
#if AMBE_TYPE == 2
    digitalWrite(STAT3_LED, LOW);
    digitalWrite(STAT4_LED, HIGH);
#endif
    delay(100);
    digitalWrite(STAT1_LED, HIGH);
    digitalWrite(STAT2_LED, LOW);
#if AMBE_TYPE == 2
    digitalWrite(STAT3_LED, HIGH);
    digitalWrite(STAT4_LED, LOW);
#endif
    delay(100);
  }

  digitalWrite(STAT1_LED, LOW);
  digitalWrite(STAT2_LED, LOW);
#if AMBE_TYPE == 2
  digitalWrite(STAT3_LED, LOW);
  digitalWrite(STAT4_LED, LOW);
#endif
}

void CLEDDriver::setLED1(bool on)
{
  digitalWrite(STAT1_LED, on ? HIGH : LOW);
}

void CLEDDriver::setLED2(bool on)
{
  digitalWrite(STAT2_LED, on ? HIGH : LOW);
}

#if AMBE_TYPE == 2

void CLEDDriver::setLED3(bool on)
{
  digitalWrite(STAT3_LED, on ? HIGH : LOW);
}

void CLEDDriver::setLED4(bool on)
{
  digitalWrite(STAT4_LED, on ? HIGH : LOW);
}

#endif

#endif
