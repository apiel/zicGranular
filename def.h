#ifndef _DEF_H_
#define _DEF_H_

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <RtAudio.h>
#include <RtMidi.h>

#ifndef APP_LOG
#define APP_LOG printf
#endif

#ifndef APP_SAMPLES_FOLDER
#define APP_SAMPLES_FOLDER "./samples"
#endif

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

#ifndef APP_CHANNELS
#define APP_CHANNELS 2
// #define APP_CHANNELS 1
#endif

#ifndef APP_AUDIO_FORMAT
#define APP_AUDIO_FORMAT RTAUDIO_FLOAT32
#endif

#ifndef APP_AUDIO_CHUNK
// Seem like what rg350 expects
#define APP_AUDIO_CHUNK 128
#endif

#ifndef APP_MAX_VOLUME
#define APP_MAX_VOLUME 2.0
#endif

#ifndef APP_TRACKS
#define APP_TRACKS 16
// #define APP_TRACKS 1
#endif

#ifndef APP_TRACK_STEPS
#define APP_TRACK_STEPS 32
#endif

#ifndef APP_TRACK_NAME
#define APP_TRACK_NAME 12
#endif

#ifndef APP_TRACK_LIST_MAX
#define APP_TRACK_LIST_MAX 1000
#endif

#ifndef APP_SAMPLE_NAME
#define APP_SAMPLE_NAME 255
#endif

#ifndef APP_DEFAULT_FONT_SIZE
#define APP_DEFAULT_FONT_SIZE 16
#endif

#ifndef isBetween
#define isBetween(x, a, b) (((a) <= (x)) && ((x) <= (b)))
#endif

#ifndef range
#define range(x, _min, _max) ((x) < (_max) ? ((x) > (_min) ? (x) : (_min)) : (_max))
#endif

#ifndef CLEAR
#define CLEAR true
#endif

#endif