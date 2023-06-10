#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include <sndfile.h>

#include "def.h"

#define AUDIO_BUFFER_SECONDS 30
#define LOAD_SAMPLE_IN_MEMORY 1
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS
#define MAX_GRAINS 24

class AudioGranular {
protected:
    bool on = false;

    float samplePos = 0;
    float sampleStep = 1.0f;
    int64_t grainSampleCount = 0;

    float buffer[AUDIO_BUFFER_SIZE];
    bool isOpen = false;

    struct Grain {
        float pos;
        int64_t start;
    } grains[MAX_GRAINS];

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    uint8_t density = 1;
    uint16_t grainSize = 50; // 20ms to 1000ms
    float window = 1.0f;

    AudioGranular()
    {
        memset(&sfinfo, 0, sizeof(sfinfo));

        // open("samples/0_squeaky.wav");
        // open("samples/0_fx_01.wav");
        // open("samples/0_axxe13.wav");
        // open("samples/bass02.wav");
        // open("samples/bass03.wav");
        open("samples/kick.wav");

        setGrainSize(grainSize);
    }

    ~AudioGranular()
    {
        close();
    }

    AudioGranular& setGrainSize(uint16_t grainSize)
    {
        this->grainSize = grainSize;
        grainSampleCount = grainSize * SAMPLE_RATE * 0.001f;
        printf("grainSize %dms grainSampleCount %ld\n", grainSize, (long)grainSampleCount);
        return *this;
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

        // for (; i < len; i++) {
        //     if (samplePos < sfinfo.frames) {
        //         buf[i] = buffer[(int)samplePos];
        //         samplePos += sampleStep;
        //     } else {
        //         buf[i] = 0;
        //     }
        // }

        if (on) {
            for (; i < len; i++) {
                Grain& grain = grains[0];
                int64_t sample = (uint64_t)grain.pos + grain.start;
                if (sample < sfinfo.frames && (int64_t)grain.pos < grainSampleCount) {
                    grain.pos += sampleStep;
                    buf[i] = buffer[sample];
                } else {
                    grain.pos = 0.0f;
                    // here should set a random start in window
                    buf[i] = 0;
                }
            }
        } else {
            for (; i < len; i++) {
                buf[i] = 0;
            }
        }

        return i;
    }

    uint8_t baseNote = 110;

    AudioGranular& noteOn(uint8_t note, uint8_t velocity)
    {
        samplePos = 0; // to be removed
        on = true;

        for (uint8_t d = 0; d < density; d++) {
            grains[d].pos = 0.0f;
            // grains[d].start = (uint64_t)(samplePos - grainSize * (1.0f - window) * 0.5f);
            grains[d].start = 0;
        }

        // https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
        // pow(2, ((note - 0) / 12.0)) = 1 for 0 semitone
        // pow(2, ((note - 1) / 12.0)) = 1.059463094 for 1 semitone
        // pow(2, ((note - 2) / 12.0)) = 1.122462048 for 2 semitone
        // ...
        sampleStep = pow(2, ((note - baseNote) / 12.0));
        printf("noteOn: %d %d %f\n", note, velocity, sampleStep);
        return *this;
    }

    AudioGranular& noteOff(uint8_t note, uint8_t velocity)
    {
        on = false;
        printf("noteOff set on to false: %d %d\n", note, velocity);
        return *this;
    }
};

#endif