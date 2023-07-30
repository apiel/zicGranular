# for FTP EOF
.ONESHELL:

# default
CC=g++ -o zicGranular

LIDSND=`pkg-config --cflags --libs sndfile`

RTAUDIO=`pkg-config --cflags --libs rtaudio`

RTMIDI=`pkg-config --cflags --libs rtmidi`

# AUDIO_API by default will use RT audio
# else uncomment one of the following to use the other audio API
PULSEAUDIO=`pkg-config --cflags --libs libpulse-simple` -DAUDIO_API=1
# ALSA=`pkg-config --cflags --libs alsa` -DAUDIO_API=2

BUILD=-Wall zicGranular.cpp -fopenmp -Wno-narrowing $(RTAUDIO) $(LIDSND) $(RTMIDI) $(PULSEAUDIO) $(ALSA)

linux: build run

build:
	$(CC) $(BUILD)

run:
	echo start zicGranular
	LD_LIBRARY_PATH=. ./zicGranular
