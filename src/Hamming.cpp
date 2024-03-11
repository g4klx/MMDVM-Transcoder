/*
 *   Copyright (C) 2015,2016,2023 by Jonathan Naylor G4KLX
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

#include "Hamming.h"

 // Hamming (15,11,3) check a boolean data array
bool CHamming::decode15113(bool* d)
{
	// Calculate the parity it should have
	bool c0 = d[0] ^ d[1] ^ d[2] ^ d[3] ^ d[4] ^ d[5] ^ d[6];
	bool c1 = d[0] ^ d[1] ^ d[2] ^ d[3] ^ d[7] ^ d[8] ^ d[9];
	bool c2 = d[0] ^ d[1] ^ d[4] ^ d[5] ^ d[7] ^ d[8] ^ d[10];
	bool c3 = d[0] ^ d[2] ^ d[4] ^ d[6] ^ d[7] ^ d[9] ^ d[10];

	unsigned char n = 0U;
	n |= (c0 != d[11]) ? 0x01U : 0x00U;
	n |= (c1 != d[12]) ? 0x02U : 0x00U;
	n |= (c2 != d[13]) ? 0x04U : 0x00U;
	n |= (c3 != d[14]) ? 0x08U : 0x00U;

	switch (n)
	{
		// Parity bit errors
		case 0x01U: d[11] = !d[11]; return true;
		case 0x02U: d[12] = !d[12]; return true;
		case 0x04U: d[13] = !d[13]; return true;
		case 0x08U: d[14] = !d[14]; return true;

		// Data bit errors
		case 0x0FU: d[0]  = !d[0];  return true;
		case 0x07U: d[1]  = !d[1];  return true;
		case 0x0BU: d[2]  = !d[2];  return true;
		case 0x03U: d[3]  = !d[3];  return true;
		case 0x0DU: d[4]  = !d[4];  return true;
		case 0x05U: d[5]  = !d[5];  return true;
		case 0x09U: d[6]  = !d[6];  return true;
		case 0x0EU: d[7]  = !d[7];  return true;
		case 0x06U: d[8]  = !d[8];  return true;
		case 0x0AU: d[9]  = !d[9];  return true;
		case 0x0CU: d[10] = !d[10]; return true;

		// No bit errors
		default: return false;
	}
}

void CHamming::encode15113(bool* d)
{
	// Calculate the checksum this row should have
	d[11] = d[0] ^ d[1] ^ d[2] ^ d[3] ^ d[4] ^ d[5] ^ d[6];
	d[12] = d[0] ^ d[1] ^ d[2] ^ d[3] ^ d[7] ^ d[8] ^ d[9];
	d[13] = d[0] ^ d[1] ^ d[4] ^ d[5] ^ d[7] ^ d[8] ^ d[10];
	d[14] = d[0] ^ d[2] ^ d[4] ^ d[6] ^ d[7] ^ d[9] ^ d[10];
}
