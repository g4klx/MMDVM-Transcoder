/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021,2023 by Jonathan Naylor G4KLX
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

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#else
#error "Unknown processor type"
#endif

#if defined(STM32F7XX)
#define  ARM_MATH_CM7
#elif defined(STM32F4XX)
#define  ARM_MATH_CM4
#else
#error "Unknown processor type"
#endif

#include <arm_math.h>

#include "IMBE/imbe_vocoder.h"
#include "Codec2/codec2.h"
#include "Codec21600PCM.h"
#include "Codec23200PCM.h"
#include "PCMCodec21600.h"
#include "PCMCodec23200.h"
#include "YSFDNDMRNXDN.h"
#include "DMRNXDNYSFDN.h"
#include "YSFVWP25FEC.h"
#include "YSFVWP25PCM.h"
#include "PCMYSFVWP25.h"
#include "DMRNXDNFEC.h"
#include "DMRNXDNPCM.h"
#include "PCMDMRNXDN.h"
#include "SerialPort.h"
#include "YSFDNFEC.h"
#include "DStarFEC.h"
#include "DStarPCM.h"
#include "PCMDStar.h"
#include "Debug.h"

extern CSerialPort serial;

extern CDStarFEC      dstarfec;
extern CDMRNXDNFEC    dmrnxdnfec;
extern CYSFDNFEC      ysfdnfec;
extern CYSFVWP25FEC   ysfvwp25fec;

extern CDStarPCM      dstarpcm;
extern CDMRNXDNPCM    dmrnxdnpcm;
extern CYSFVWP25PCM   ysfvwp25pcm;
extern CCodec23200PCM codec23200pcm;
extern CCodec21600PCM codec21600pcm;

extern CPCMDStar      pcmdstar;
extern CPCMDMRNXDN    pcmdmrnxdn;
extern CPCMYSFVWP25   pcmysfvwp25;
extern CPCMCodec23200 pcmcodec23200;
extern CPCMCodec21600 pcmcodec21600;

extern CYSFDNDMRNXDN  ysfdndmrnxdn;
extern CDMRNXDNYSFDN  dmrnxdnysfdn;

extern imbe_vocoder   imbe;
extern CCodec2        codec23200;
extern CCodec2        codec21600;

#endif

