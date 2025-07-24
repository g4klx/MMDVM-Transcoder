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

#include "AMBE3000Driver.h"

#if AMBE_TYPE == 1 || AMBE_TYPE == 2

#include "Globals.h"
#include "Debug.h"

const uint8_t DVSI_START_BYTE   = 0x61U;

const uint8_t DVSI_TYPE_CONTROL = 0x00U;
const uint8_t DVSI_TYPE_AMBE    = 0x01U;
const uint8_t DVSI_TYPE_AUDIO   = 0x02U;

const uint8_t DVSI_PKT_RATET    = 0x09U;
const uint8_t DVSI_PKT_RATEP    = 0x0AU;
const uint8_t DVSI_PKT_CHANNEL0 = 0x40U;
const uint8_t DVSI_PKT_CHANNEL1 = 0x41U;
const uint8_t DVSI_PKT_CHANNEL2 = 0x42U;

const uint16_t DVSI_PCM_SAMPLES = 160U;
const uint16_t DVSI_PCM_BYTES   = DVSI_PCM_SAMPLES * sizeof(int16_t);

CAMBE3000Driver::CAMBE3000Driver() :
m_buffer0(),
m_buffer1(),
m_length0(0U),
m_length1(0U),
m_utils()
{
}

void CAMBE3000Driver::startup()
{
  dvsi1.reset();

#if AMBE_TYPE == 2
  dvsi2.reset();
#endif
}

void CAMBE3000Driver::init(uint8_t n, AMBE_MODE mode)
{
  uint8_t buffer[100U];
  uint16_t length = m_utils.createModeChange(mode, buffer);

#if AMBE_TYPE == 2
  if (n == 0U) {
    dvsi1.write(buffer, length);
#if defined(HAS_LEDS)
    leds.setLED1(true);
#endif
  } else {
    dvsi2.write(buffer, length);
#if defined(HAS_LEDS)
    leds.setLED3(true);
#endif
  }
#else
  dvsi1.write(buffer, length);
#if defined(HAS_LEDS)
  leds.setLED1(true);
#endif
#endif
}

void CAMBE3000Driver::process()
{
  process(0U, dvsi1, m_length0, m_buffer0);
#if AMBE_TYPE == 2
  process(1U, dvsi2, m_length1, m_buffer1);
#endif
}

void CAMBE3000Driver::process(uint8_t n, CDVSIDriver& driver, uint16_t& mLength, uint8_t* mBuffer)
{
  uint8_t buffer[500U];
  uint16_t length = driver.read(buffer);
  if (length == 0U)
    return;

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (n == 0U)
    leds.setLED1(false);
  else
    leds.setLED3(false);
#else
  leds.setLED1(false);
#endif
#endif

  uint16_t pos = 0U;

  switch (buffer[3U]) {
    case DVSI_TYPE_CONTROL:
      pos = 4U;
      while (pos < length) {
        switch (buffer[pos]) {
          case DVSI_PKT_RATET:
            DEBUG2("Response AMBE3000 to PKT_RATET is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          case DVSI_PKT_RATEP:
            DEBUG2("Response AMBE3000 to PKT_RATEP is ", buffer[pos + 1U]);
            pos += 2U;
            break;

          default:
            DEBUG2("Unknown AMBE3000 control type response of ", buffer[pos]);
            return;
        }
      }
      break;

    case DVSI_TYPE_AMBE:
      mLength = m_utils.extractAMBEFrame(buffer, mBuffer);
      break;

    case DVSI_TYPE_AUDIO:
      mLength = m_utils.extractPCMFrame(buffer, mBuffer);
      break;

    default:
      DEBUG2("Unknown AMBE3000 type from the AMBE chip ", buffer[3U]);
      break;
  }
}

uint8_t CAMBE3000Driver::writeAMBE(uint8_t n, const uint8_t* ambe)
{
#if AMBE_TYPE == 2
  if (n == 0U)
    return writeAMBE(n, dvsi1, ambe);
  else
    return writeAMBE(n, dvsi2, ambe);
#else
  return writeAMBE(n, dvsi1, ambe);
#endif
}

uint8_t CAMBE3000Driver::writeAMBE(uint8_t n, CDVSIDriver& driver, const uint8_t* ambe)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!driver.ready()) {
    DEBUG1("The AMBE3000 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[50U];
  uint16_t pos = m_utils.createAMBEFrame(ambe, out);

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (n == 0U)
    leds.setLED1(true);
  else
    leds.setLED3(true);
#else
  leds.setLED1(true);
#endif
#endif

  driver.write(out, pos);

  return 0x00U;
}

uint8_t CAMBE3000Driver::writePCM(uint8_t n, const uint8_t* pcm)
{
#if AMBE_TYPE == 2
  if (n == 0U)
    return writePCM(n, dvsi1, pcm);
  else
    return writePCM(n, dvsi2, pcm);
#else
  return writePCM(n, dvsi1, pcm);
#endif
}

uint8_t CAMBE3000Driver::writePCM(uint8_t n, CDVSIDriver& driver, const uint8_t* pcm)
{
  // If the RTS pin is high, then the chip does not expect any more data to be sent through
  if (!driver.ready()) {
    DEBUG1("The AMBE3000 chip is not ready to receive any more data");
    return 0x05U;
  }

  uint8_t out[400U];
  uint16_t pos = m_utils.createPCMFrame(pcm, out);

#if defined(HAS_LEDS)
#if AMBE_TYPE == 2
  if (n == 0U)
    leds.setLED1(true);
  else
    leds.setLED3(true);
#else
  leds.setLED1(true);
#endif
#endif

  driver.write(out, pos);

  return 0x00U;
}

AD_STATE CAMBE3000Driver::readAMBE(uint8_t n, uint8_t* ambe)
{
#if AMBE_TYPE == 2
  if (n == 0U)
    return readAMBE(n, dvsi1, ambe, m_length0, m_buffer0);
  else
    return readAMBE(n, dvsi2, ambe, m_length1, m_buffer1);
#else
  return readAMBE(n, dvsi1, ambe, m_length0, m_buffer0);
#endif
}

AD_STATE CAMBE3000Driver::readAMBE(uint8_t n, CDVSIDriver& driver, uint8_t* ambe, uint16_t& mLength, const uint8_t* mBuffer)
{
  switch (mLength) {
    case 0U:
      return ADS_NO_DATA;

    case DVSI_PCM_BYTES:
      mLength = 0U;
      return ADS_WRONG_TYPE;

    default:
      ::memcpy(ambe, mBuffer, mLength);
      mLength = 0U;
      return ADS_DATA;
  }
}

AD_STATE CAMBE3000Driver::readPCM(uint8_t n, uint8_t* pcm)
{
#if AMBE_TYPE == 2
  if (n == 0U)
    return readPCM(n, dvsi1, pcm, m_length0, m_buffer0);
  else
    return readPCM(n, dvsi2, pcm, m_length1, m_buffer1);
#else
  return readPCM(n, dvsi1, pcm, m_length0, m_buffer0);
#endif
}

AD_STATE CAMBE3000Driver::readPCM(uint8_t n, CDVSIDriver& driver, uint8_t* pcm, uint16_t& mLength, const uint8_t* mBuffer)
{
  switch (mLength) {
    case 0U:
      return ADS_NO_DATA;

    case DVSI_PCM_BYTES:
      ::memcpy(pcm, mBuffer, DVSI_PCM_BYTES);
      mLength = 0U;
      return ADS_DATA;

    default:
      mLength = 0U;
      return ADS_WRONG_TYPE;
  }
}

void CAMBE3000Driver::drain(uint8_t n)
{
  switch (n) {
    case 0U:
      m_length0 = 0U;
      break;
    default:
      m_length1 = 0U;
      break;
  }
}

#endif
