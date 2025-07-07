/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021,2023,2024,2025 by Jonathan Naylor G4KLX
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

CSerialPort     serial;

CDStarFEC       dstarfec;
CDMRNXDNFEC     dmrnxdnfec;
CYSFDNFEC       ysfdnfec;
CIMBEFEC        imbefec;

CYSFDNPCM       ysfdnpcm;
CDStarPCM       dstarpcm;
CDMRNXDNPCM     dmrnxdnpcm;

CIMBEPCM        imbepcm;
CIMBEFECPCM     imbefecpcm;
CCodec23200PCM  codec23200pcm;

CPCMYSFDN       pcmysfdn;
CPCMDStar       pcmdstar;
CPCMDMRNXDN     pcmdmrnxdn;

CPCMIMBE        pcmimbe;
CPCMIMBEFEC     pcmimbefec;
CPCMCodec23200  pcmcodec23200;

CYSFDNDMRNXDN   ysfdndmrnxdn;
CDMRNXDNYSFDN   dmrnxdnysfdn;

CIMBEIMBEFEC    imbeimbefec;
CIMBEFECIMBE    imbefecimbe;

CALawPCM        alawpcm;
CPCMALaw        pcmalaw;
CMuLawPCM       mulawpcm;
CPCMMuLaw       pcmmulaw;

imbe_vocoder    imbe;
CCodec2         codec23200(true);

CDVSIDriver     dvsi;
CAMBE3003Driver ambe;

#if defined(HAS_LEDS)
CLEDDriver      leds;
#endif

#if defined(HAS_LEDS)
unsigned long start = 0UL;
bool on = true;
#endif

OPMODE opmode = OPMODE::NONE;

extern "C" {
  void setup()
  {
    serial.start();

#if defined(HAS_LEDS)
    leds.startup();
#endif

    dvsi.startup();
    ambe.startup();

#if defined(HAS_LEDS)
    leds.setLED2(on);
    start = millis();
#endif
  }

  void loop()
  {
    serial.process();

    if (opmode == OPMODE::TRANSCODING)
      ambe.process();

#if defined(HAS_LEDS)
    unsigned long end = millis();
    if ((end - start) > 500UL) {
      on = !on;
      leds.setLED2(on);
      start = end;
    }
#endif
  }
}
