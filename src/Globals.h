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

#if !defined(GLOBALS_H)
#define  GLOBALS_H

#include <Arduino.h>

#include "IMBE/imbe_vocoder.h"

#include "Codec2/codec2.h"

#include "AMBE3000Driver.h"
#include "Codec23200PCM.h"
#include "PCMCodec23200.h"
#include "DStarDMRNXDN.h"
#include "DMRNXDNDStar.h"
#include "YSFDNDMRNXDN.h"
#include "DMRNXDNYSFDN.h"
#include "IMBEIMBEFEC.h"
#include "IMBEFECIMBE.h"
#include "DStarYSFDN.h"
#include "YSFDNDStar.h"
#include "DMRNXDNFEC.h"
#include "DMRNXDNPCM.h"
#include "PCMDMRNXDN.h"
#include "IMBEFECPCM.h"
#include "PCMIMBEFEC.h"
#include "DVSIDriver.h"
#include "SerialPort.h"
#include "LEDDriver.h"
#include "PCMYSFDN.h"
#include "YSFDNPCM.h"
#include "YSFDNFEC.h"
#include "DStarFEC.h"
#include "DStarPCM.h"
#include "PCMDStar.h"
#include "YSFDNPCM.h"
#include "MuLawPCM.h"
#include "PCMMuLaw.h"
#include "ALawPCM.h"
#include "PCMALaw.h"
#include "IMBEFEC.h"
#include "IMBEPCM.h"
#include "PCMIMBE.h"
#include "Config.h"
#include "Debug.h"

extern CSerialPort     serial;

extern CDStarFEC       dstarfec;
extern CDMRNXDNFEC     dmrnxdnfec;
extern CYSFDNFEC       ysfdnfec;
extern CIMBEFEC        imbefec;

extern CDStarDMRNXDN   dstardmrnxdn;
extern CDMRNXDNDStar   dmrnxdndstar;
extern CDStarYSFDN     dstarysfdn;
extern CYSFDNDStar     ysfdndstar;

extern CYSFDNPCM       ysfdnpcm;
extern CDStarPCM       dstarpcm;
extern CDMRNXDNPCM     dmrnxdnpcm;
extern CIMBEPCM        imbepcm;
extern CIMBEFECPCM     imbefecpcm;
extern CCodec23200PCM  codec23200pcm;

extern CPCMYSFDN       pcmysfdn;
extern CPCMDStar       pcmdstar;
extern CPCMDMRNXDN     pcmdmrnxdn;
extern CPCMIMBE        pcmimbe;
extern CPCMIMBEFEC     pcmimbefec;
extern CPCMCodec23200  pcmcodec23200;

extern CYSFDNDMRNXDN   ysfdndmrnxdn;
extern CDMRNXDNYSFDN   dmrnxdnysfdn;

extern CIMBEIMBEFEC    imbeimbefec;
extern CIMBEFECIMBE    imbefecimbe;

extern CALawPCM        alawpcm;
extern CPCMALaw        pcmalaw;
extern CMuLawPCM       mulawpcm;
extern CPCMMuLaw       pcmmulaw;

extern imbe_vocoder    imbe;
extern CCodec2         codec23200;

#if AMBE_TYPE > 0
extern CDVSIDriver     dvsi;
extern CAMBE3000Driver ambe3000;
#endif

#if defined(HAS_LEDS)
extern CLEDDriver      leds;
#endif

enum OPMODE {
  OPMODE_NONE,
  OPMODE_TRANSCODING,
  OPMODE_PASSTHROUGH
};

extern OPMODE opmode;

#endif
