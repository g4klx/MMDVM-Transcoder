This is the source code of the MMDVM-Transcoder firmware that supports transcoding to and from the following (digital) audio mode:
- D-Star
- DMR
- YSF (DN and VW mode)
- P25 phase 1
- NXDN
- M17 (3200 and 1600 mode)
- PCM

The available conversions depends on the hardware available. With no AMBE DSP chip then very limited transcoding is possible, but with a DVSI AMBE3000R most conversions are possible, although some of the transcoding to and from D-Star are not available. With a DVSI AMBE3003R then every combination of transcoding is available.

It runs on the the ST-Micro STM32F4xxx and STM32F7xxx processors. Current testing uses an STM32F446 Nucleo board plugged into an NWDR AMBE3000 hat for the Raspberry Pi which is no longer in production. More integrated version will be available in the future which aren't reliant on unavailable hardware.

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.

Portions of the ARM support code include the following copyright:

   Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
