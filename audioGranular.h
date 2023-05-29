#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include <sndfile.h>

#include "def.h"

#define AUDIO_BUFFER_SECONDS 30
#define LOAD_SAMPLE_IN_MEMORY 1
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

class AudioGranular {
protected:
    int samplePos = 0;
    float buffer[AUDIO_BUFFER_SIZE];
    bool isOpen = false;

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    AudioGranular()
    {
        memset(&sfinfo, 0, sizeof(sfinfo));

        // open("samples/0_squeaky.wav");
        // open("samples/0_fx_01.wav");
        open("samples/0_axxe13.wav");
    }

    ~AudioGranular()
    {
        close();
    }

    AudioGranular& close()
    {
        isOpen = false;
        if (file) {
            sf_close(file);
        }
        return *this;
    }

    // By default, move to the end of the sample to avoid unwanted playback.
    AudioGranular& open(const char* filename, int seek = SEEK_END)
    {
        close();

        if (!(file = sf_open(filename, SFM_READ, &sfinfo))) {
            printf("Error: could not open file %s\n", filename);
            return *this;
        }
        printf("Audio file %s sampleCount %ld sampleRate %d\n", filename, (long)sfinfo.frames, sfinfo.samplerate);
        isOpen = true;

        sf_read_float(file, buffer, AUDIO_BUFFER_SIZE);
        samplePos = sfinfo.frames;

        return *this;
    }

    int64_t samples(float* buf, int len)
    {
        int i = 0;
        for (; i < len; i++) {
            if (samplePos < sfinfo.frames) {
                buf[i] = buffer[samplePos];
                samplePos++;
            } else {
                buf[i] = 0;
            }
        }
        return i;
    }

    AudioGranular& noteOn()
    {
        samplePos = 0;
        return *this;
    }
};

#endif