This is the source code of the MMDVM-Transcoder firmware that supports transcoding to and from the following digital and analogue audio modes:
- D-Star
- DMR
- YSF (DN and VW mode)
- P25 phase 1 (with or without FEC)
- NXDN
- M17 (3200 mode only)
- PCM

The available conversions depends on the hardware available. With no AMBE DSP chip then very limited transcoding is possible, with one DVSI AMBE3000 many more are possible, most of the restrictions involve going from to and from D-Star. With two DVSI AMBE3000s all conversions are possible.

It runs on the the ST-Micro or STM32F722 or STM32H723 processor and a custom AMBE board. The design of this board will be made available once it's design has been finalised.

In addition there are two supplied programs that exercise the transcoder. Tester is used to test the basic funationality of the transcoder and to hopefully enable the finding of serious bugs that may be added during development. FileConvert is used to convert the contents of DV and WAV files to and from different modes, it is similar in functionality to the older AMBETools in this GitHub repository.

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.
