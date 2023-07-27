# for FTP EOF
.ONESHELL:

# default
CC=g++ -o zicGranular

LIDSND=`pkg-config --cflags --libs sndfile`

RTAUDIO=`pkg-config --cflags --libs rtaudio`

RTMIDI=`pkg-config --cflags --libs rtmidi`

PULSEAUDIO=`pkg-config --cflags --libs libpulse-simple` -DAUDIO_API=1

BUILD=-Wall zicGranular.cpp -fopenmp -Wno-narrowing $(RTAUDIO) $(LIDSND) $(RTMIDI) $(PULSEAUDIO)

linux: build run

build:
	$(CC) $(BUILD)

run:
	echo start zicGranular
	LD_LIBRARY_PATH=. ./zicGranular
