#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include <sndfile.h>

#include <cstdlib>
using namespace std;

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
        int64_t delay;
    } grains[MAX_GRAINS];

    void initGrain(uint8_t index)
    {
        Grain& grain = grains[index];
        grain.pos = 0.0f; // should pos be set in function of density ?
        grain.start = 0; // start should be set randomly in spray
        grain.delay = (rand() % 200) * SAMPLE_RATE * 0.001f; // 10ms, but this could be random
    }

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    uint8_t density = 1;
    uint16_t grainSize = 50; // 20ms to 1000ms
    float spray = 1.0f;

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

        if (on) {
            for (; i < len; i++) {
                buf[i] = 0;

                Grain& grain = grains[0];
                if (grain.delay > 0) {
                    grain.delay--;
                } else {
                    int64_t sample = (uint64_t)grain.pos + grain.start;
                    if (sample < sfinfo.frames && (int64_t)grain.pos < grainSampleCount) { // is sample < sfinfo.frames even necessary if start calculated properly
                        grain.pos += sampleStep;
                        buf[i] += buffer[sample];
                    } else {
                        initGrain(0);
                    }
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
            initGrain(d);
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

// Length (or similar word): the length of the grain in ms or Hz
// Position (or similar word): the start point of the grain located in the original audio sample
// Smoothing (or similar word): adjusts the crossfades between grains
// Distance (or similar word): increase or decrease gaps of time between the grains
// Density (or similar word): the number of grains that are played at the same time
// Pitch (or similar word): the pitch of the grain
// Pan (or similar word): the panning of the grain
// Volume (or similar word): the volume of the grain
// “Grain” - measured in Hz on Granulator II, defines the length of the grain (aka “Length”)
// “Grain Random” - measured from 0 to 100, randomizes the length of the grain
// “FilePos” - the start point of the grain, located within the sample (aka “Position”)
// “Spray” - measured in ms, randomizes Position within x ms of FilePos
// Spray (or similar word): the randomization of the start point of the grain
// “Freq” - simply the cutoff freq of a LFP filter on the filter page, I’m automating this frequency

#endif