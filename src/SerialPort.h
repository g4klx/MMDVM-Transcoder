/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021,2023,2024 by Jonathan Naylor G4KLX
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

#if !defined(SERIALPORT_H)
#define  SERIALPORT_H

#include "Config.h"
#include "Globals.h"
#include "Processor.h"

#if !defined(SERIAL_SPEED)
#define SERIAL_SPEED 460800
#endif

class CSerialPort {
public:
  CSerialPort();

  void start();

  void process();

  void writeData(const uint8_t* data, uint16_t length);

#if defined(DEBUGGING)
  void writeDebug(const char* text);
  void writeDebug(const char* text, int16_t n1);
  void writeDebug(const char* text, int16_t n1, int16_t n2);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4);
#endif

#if defined(HAS_STLINK)
  void dump(const char* title, const uint8_t* buffer, uint16_t length) const;
#endif

private:
  uint8_t       m_buffer[512U];
  uint16_t      m_ptr;
  uint16_t      m_len;
  unsigned long m_start;

  IProcessor* m_step1;
  IProcessor* m_step2;

  void    sendACK();
  void    sendNAK(uint8_t err);
  void    getVersion();
  void    getCapabilities();
  uint8_t setMode(const uint8_t* data, uint16_t length);
  uint8_t sendData(const uint8_t* data, uint16_t length);
  void    processMessage(uint8_t type, const uint8_t* data, uint16_t length);
  void    processData();

#if defined(DEBUGGING)
  uint16_t convert(int16_t num, uint8_t* buffer);
  void     reverse(uint8_t* buffer, uint16_t length) const;
#endif
};

#endif
