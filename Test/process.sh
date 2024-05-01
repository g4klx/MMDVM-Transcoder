#! /bin/sh

echo "D-Star to WAV"
../FileConvert /dev/ttyUSB0 dstar audio.dstar wav dstar_wav.wav
echo "DMR/NXDN to WAV"
../FileConvert /dev/ttyUSB0 nxdn audio.nxdn wav nxdn_wav.wav
echo "M17 to WAV"
../FileConvert /dev/ttyUSB0 m17 audio.m17 wav m17_wav.wav
echo "P25 to WAV"
../FileConvert /dev/ttyUSB0 imbe audio.p25 wav p25_wav.wav

echo "D-Star to DMR/NXDN"
../FileConvert /dev/ttyUSB0 dstar audio.dstar nxdn dstar_nxdn.nxdn
../FileConvert /dev/ttyUSB0 nxdn dstar_nxdn.nxdn wav dstar_nxdn_wav.wav
rm -f dstar_nxdn.nxdn

echo "D-Star to M17"
../FileConvert /dev/ttyUSB0 dstar audio.dstar m17 dstar_m17.m17
../FileConvert /dev/ttyUSB0 m17 dstar_m17.m17 wav dstar_m17_wav.wav
rm -f dstar_m17.m17

echo "D-Star to P25"
../FileConvert /dev/ttyUSB0 dstar audio.dstar imbe dstar_p25.p25
../FileConvert /dev/ttyUSB0 imbe dstar_p25.p25 wav dstar_p25_wav.wav
rm -f dstar_p25.p25

echo "DMR/NXDN to D-Star"
../FileConvert /dev/ttyUSB0 nxdn audio.nxdn dstar nxdn_dstar.dstar
../FileConvert /dev/ttyUSB0 dstar nxdn_dstar.dstar wav nxdn_dstar_wav.wav
rm -f nxdn_dstar.dstar

echo "DMR/NXDN to M17"
../FileConvert /dev/ttyUSB0 nxdn audio.nxdn m17 nxdn_m17.m17
../FileConvert /dev/ttyUSB0 m17 nxdn_m17.m17 wav nxdn_m17_wav.wav
rm -f nxdn_m17.m17

echo "DMR/NXDN to P25"
../FileConvert /dev/ttyUSB0 nxdn audio.nxdn imbe nxdn_p25.p25
../FileConvert /dev/ttyUSB0 imbe nxdn_p25.p25 wav nxdn_p25_wav.wav
rm -f nxdn_p25.p25

echo "M17 to D-Star"
../FileConvert /dev/ttyUSB0 m17 audio.m17 dstar m17_dstar.dstar
../FileConvert /dev/ttyUSB0 dstar m17_dstar.dstar wav m17_dstar_wav.wav
rm -f m17_dstar.dstar

echo "M17 to DMR/NXDN"
../FileConvert /dev/ttyUSB0 m17 audio.m17 nxdn m17_nxdn.nxdn
../FileConvert /dev/ttyUSB0 nxdn m17_nxdn.nxdn wav m17_nxdn_wav.wav
rm -f m17_nxdn.nxdn

echo "M17 to P25"
../FileConvert /dev/ttyUSB0 m17 audio.m17 imbe m17_p25.p25
../FileConvert /dev/ttyUSB0 imbe m17_p25.p25 wav m17_p25_wav.wav
rm -f m17_p25.p25

echo "P25 to D-Star"
../FileConvert /dev/ttyUSB0 imbe audio.p25 dstar p25_dstar.dstar
../FileConvert /dev/ttyUSB0 dstar p25_dstar.dstar wav p25_dstar_wav.wav
rm -f p25_dstar.dstar

echo "P25 to DMR/NXDN"
../FileConvert /dev/ttyUSB0 imbe audio.p25 nxdn p25_nxdn.nxdn
../FileConvert /dev/ttyUSB0 nxdn p25_nxdn.nxdn wav p25_nxdn_wav.wav
rm -f p25_nxdn.nxdn

echo "P25 to M17"
../FileConvert /dev/ttyUSB0 imbe audio.p25 m17 p25_m17.m17
../FileConvert /dev/ttyUSB0 m17 p25_m17.m17 wav p25_m17_wav.wav
rm -f p25_m17.m17
