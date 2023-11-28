/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021,2023 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Mathis Schmieder DB9MAT
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

CSerialPort serial;

CDStarFEC    dstarfec;
CDMRNXDNFEC  dmrnxdnfec;
CYSFDNFEC    ysfdnfec;
CYSFVWP25FEC ysfvwp25fec;

CDStarPCM      dstarpcm;
CDMRNXDNPCM    dmrnxdnpcm;
CYSFVWP25PCM   ysfvwp25pcm;
CCodec23200PCM codec23200pcm;
CCodec21600PCM codec21600pcm;

CPCMDStar      pcmdstar;
CPCMDMRNXDN    pcmdmrnxdn;
CPCMYSFVWP25   pcmysfvwp25;
CPCMCodec23200 pcmcodec23200;
CPCMCodec21600 pcmcodec21600;

CYSFDNDMRNXDN  ysfdndmrnxdn;
CDMRNXDNYSFDN  dmrnxdnysfdn;

imbe_vocoder   imbe;
CCodec2        codec23200(true);
CCodec2        codec21600(false);

CDVSIDriver    dvsi;
CAMBEDriver    ambe;

void setup()
{
  serial.start();

#if AMBE_TYPE > 0
  ambe.startup();
#endif
}

void loop()
{
  serial.process();

#if AMBE_TYPE > 0
  ambe.process();
#endif
}

int main()
{
  setup();

  for (;;)
    loop();
}

