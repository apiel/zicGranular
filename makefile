# for FTP EOF
.ONESHELL:

# default
CC=g++ -o zicGranular

LIDSND=`pkg-config --cflags --libs sndfile`

RTAUDIO=`pkg-config --cflags --libs rtaudio`

BUILD=-Wall zicGranular.cpp -fopenmp -Wno-narrowing $(RTAUDIO) $(LIDSND)

linux: build run

build:
	$(CC) $(BUILD)

run:
	echo start zicGranular
	LD_LIBRARY_PATH=. ./zicGranular
