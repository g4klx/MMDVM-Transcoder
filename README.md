This is the source code of the MMDVM-Transcoder firmware that supports transcoding to and from the following digital and analogue audio modes:
- D-Star
- DMR
- YSF (DN and VW mode)
- P25 phase 1
- NXDN
- M17
- PCM

The available conversions depends on the hardware available. With no AMBE DSP chip then very limited transcoding is possible, but with a DVSI AMBE3000 most conversions are possible.

It runs on the the ST-Micro STM32F722 processor and a custom AMBE board. The design of this board will be made available once it's design has been finalised.

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.
