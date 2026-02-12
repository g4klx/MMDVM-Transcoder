/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2023,2024,2025 by Jonathan Naylor G4KLX
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

// Baud rate for host communication.
#define SERIAL_SPEED	    921600

// Select use of debugging messages
#define	DEBUGGING

// Swap the use of two AMBE chips
// #define SWAP_AMBE_CHIPS

// Baud rate for the DVSI chip(s)
#define DVSI_SPEED          460800

// Number of DVSI AMBE chip(s)
// 0=None
// 1=One AMBE3000
// 2=Two AMBE3000s
// 3=One AMBE3003
#define AMBE_TYPE       1

// Are LEDs available for status information?
#define HAS_LEDS

// Do we have an ST-Link interface for debugging?
#define HAS_STLINK

#endif
