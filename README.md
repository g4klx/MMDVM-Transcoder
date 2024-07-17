This is the source code of the MMDVM-Transcoder firmware that supports transcoding to and from the following digital and analogue audio modes:
- D-Star
- DMR
- YSF (DN and VW mode)
- P25 phase 1 (with or without FEC)
- NXDN
- M17 (3200 mode only)
- PCM

The supported hardware uses an STM32H723 MCU coupled with an AMBE3003 vocoder chip from DVSI.

The design of a suitable AMBE vocoder board to be used with a NUCLEO-H723ZG will be made available once it's design has been finalised.

In addition there are two supplied programs that exercise the transcoder. Tester is used to test the basic funationality of the transcoder and to hopefully enable the finding of serious bugs that may be added during development. FileConvert is used to convert the contents of DV and WAV files to and from different modes, it is similar in functionality to the older AMBETools in this GitHub repository.

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.
