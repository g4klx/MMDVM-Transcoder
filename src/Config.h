/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2023,2024 by Jonathan Naylor G4KLX
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

#if !defined(CONFIG_H)
#define  CONFIG_H

// Allow for the use of high quality external clock oscillators
// The number is the frequency of the oscillator in Hertz.
//
// The frequency of the TCXO must be an integer multiple of 48000.
// Frequencies such as 12.0 Mhz (48000 * 250) and 14.4 Mhz (48000 * 300) are suitable.
// Frequencies such as 10.0 Mhz (48000 * 208.333) or 20 Mhz (48000 * 416.666) are not suitable.
//
// For 12 MHz
#define EXTERNAL_OSC 12000000
// For 12.288 MHz
// #define EXTERNAL_OSC 12288000
// For 14.4 MHz
// #define EXTERNAL_OSC 14400000
// For 19.2 MHz
// #define EXTERNAL_OSC 19200000

// Baud rate for host communication.
#define SERIAL_SPEED	460800

// Select use of debugging messages
#define	DEBUGGING

// Baud rate for DVSI AMBE3000/3003R chip
#define DVSI_AMBE3000_SPEED	460800

// Baud rate for DVSI AMBE4020 chip
#define DVSI_AMBE4020_SPEED	460800

// Type of DVSI AMBE chip
// 0=None
// 1=AMBE3000R
// 2=AMBE3003R
// 3=AMBE3000R + AMBE4020
#define	AMBE_TYPE	1

// Are LEDs available for status information?
#define HAS_LEDS

#endif
