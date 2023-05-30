#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include <sndfile.h>

#include "def.h"

#define AUDIO_BUFFER_SECONDS 30
#define LOAD_SAMPLE_IN_MEMORY 1
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

class AudioGranular {
protected:
    float samplePos = 0;
    float sampleStep = 1.0f;

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
        // open("samples/0_axxe13.wav");
        // open("samples/bass02.wav");
        // open("samples/bass03.wav");
        open("samples/kick.wav");
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
                buf[i] = buffer[(int)samplePos];
                samplePos += sampleStep;
            } else {
                buf[i] = 0;
            }
        }
        return i;
    }

    AudioGranular& noteOn(uint8_t note, uint8_t velocity)
    {
        samplePos = 0;
        // https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
        // pow(2, ((note - 0) / 12.0)) = 1 for 0 semitone
        // pow(2, ((note - 1) / 12.0)) = 1.059463094 for 1 semitone
        // pow(2, ((note - 2) / 12.0)) = 1.122462048 for 2 semitone
        // ...
        sampleStep = pow(2, ((note - 110) / 12.0));
        // printf("noteOn: %d %d %f\n", note, velocity, sampleStep);
        return *this;
    }
};

#endif