#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include <sndfile.h>

#include <cstdlib>
using namespace std;

#include "def.h"

#define AUDIO_BUFFER_SECONDS 30
#define LOAD_SAMPLE_IN_MEMORY 1
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS
#define MAX_GRAINS_PER_VOICE 24
#define MAX_GRAIN_VOICES 4

class AudioGranular {
protected:
    int64_t grainSampleCount = 0;
    float buffer[AUDIO_BUFFER_SIZE];
    float attackStep = 0.0f;
    float releaseStep = 0.0f;

    struct Grain {
        float pos;
        int64_t start;
        int64_t delay;
        float sampleStep;
    };

    struct Voice {
        int8_t note;
        Grain grains[MAX_GRAINS_PER_VOICE];
    } voices[MAX_GRAIN_VOICES];

    uint8_t baseNote = 110;
    float getSampleStep(uint8_t note)
    {
        // https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
        // pow(2, ((note - 0) / 12.0)) = 1 for 0 semitone
        // pow(2, ((note - 1) / 12.0)) = 1.059463094 for 1 semitone
        // pow(2, ((note - 2) / 12.0)) = 1.122462048 for 2 semitone
        // ...
        return pow(2, ((note - baseNote) / 12.0));
    }

    void initGrain(Grain& grain)
    {
        initGrain(grain, grain.sampleStep);
    }

    void initGrain(Grain& grain, float sampleStep)
    {
        grain.sampleStep = sampleStep;
        grain.pos = 0.0f;
        uint16_t _spray = spray ? ((spray - (rand() % (spray * 2))) * SAMPLE_RATE * 0.001f) : 0;
        grain.start = range(start + _spray, 0, sfinfo.frames);
        grain.delay = delay ? ((rand() % delay) * SAMPLE_RATE * 0.001f) : 0;
    }

    float sample(Voice& voice)
    {
        float sample = 0.0f;
        for (uint8_t d = 0; d < density; d++) {
            Grain& grain = voice.grains[d];
            if (grain.delay > 0) {
                grain.delay--;
            } else {
                int64_t samplePos = (uint64_t)grain.pos + grain.start;
                // if (samplePos < sfinfo.frames && (int64_t)grain.pos < grainSampleCount) { // is samplePos < sfinfo.frames even necessary if start calculated properly
                if ((int64_t)grain.pos < grainSampleCount) {
                    grain.pos += grain.sampleStep;
                    sample += buffer[samplePos];
                } else {
                    initGrain(grain);
                }
            }
        }
        return sample;
    }

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    uint8_t density = 4;
    uint16_t grainSize = 300;
    uint16_t spray = 1000;
    uint16_t delay = 0;
    uint16_t start = 0;
    float attack = 0.3f;
    float release = 1.0f;

    AudioGranular()
    {
        memset(&sfinfo, 0, sizeof(sfinfo));

        // open("samples/0_squeak.wav");
        // open("samples/0_altaopa.wav");
        open("samples/0_axxe13.wav");
        // open("samples/bass03.wav");
        // open("samples/0_squeaky.wav");
        // open("samples/0_ir0nwave.wav");
        // open("samples/kick.wav");

        allOff();
        setGrainSize(grainSize);
        setAttack(attack);
        setRelease(release);
    }

    ~AudioGranular()
    {
        close();
    }

    /**
     * @brief Set the Grain Size meaning the length duration of the grain in ms.
     *
     * @param grainSize in ms
     * @return AudioGranular&
     */
    AudioGranular& setGrainSize(int32_t _grainSize)
    {
        grainSize = range(_grainSize, 20, 30000); // should it be between 20 and 1000?
        grainSampleCount = _grainSize * SAMPLE_RATE * 0.001f;
        printf("grainSampleCount %ld grainSize %d ms\n", grainSampleCount, grainSize);
        return *this;
    }

    /**
     * @brief Set the Spray of the grain start position, giving +/- ms random position to
     * the grain start position.
     *
     * @param spray in ms
     * @return AudioGranular&
     */
    AudioGranular& setSpray(int32_t _spray)
    {
        spray = range(_spray, 0, 30000); // should it be between 0 and 1000?
        printf("spray %d ms\n", spray);
        return *this;
    }

    /**
     * @brief Set the Density meaning the number of grains that are played at the same time.
     *
     * @param density
     * @return AudioGranular&
     */
    AudioGranular& setDensity(uint8_t _density)
    {
        density = range(_density, 1, MAX_GRAINS_PER_VOICE);
        printf("density %d\n", density);
        return *this;
    }

    /**
     * @brief Set the Delay before grain start to play
     *
     * @param delay in ms
     * @return AudioGranular&
     */
    AudioGranular& setDelay(int32_t _delay)
    {
        delay = range(_delay, 0, 30000); // should it be between 0 and 1000?
        printf("delay %d ms\n", delay);
        return *this;
    }

    /**
     * @brief Set the Start position of the sample to play
     *
     * @param start in ms
     * @return AudioGranular&
     */
    AudioGranular& setStart(int32_t _start)
    {
        start = range(_start, 0, 30000); // should it be between 0 and 1000?
        printf("start %d ms\n", start);
        return *this;
    }

    /**
     * @brief Set the Attack time of the voice
     *
     * @param attack in ms
     * @return AudioGranular&
     */
    AudioGranular& setAttack(int32_t _attack)
    {
        attack = range(_attack, 0, 5000);
        uint64_t attackSamples = attack * SAMPLE_RATE * 0.001f;
        attackStep = 1.0f / attackSamples;
        printf("attack %f ms %ld samples\n", attack, attackSamples);
        return *this;
    }

    /**
     * @brief Set the Release time of the voice
     *
     * @param release in ms
     * @return AudioGranular&
     */
    AudioGranular& setRelease(int32_t _release)
    {
        release = range(_release, 0, 10000);
        uint64_t releaseSamples = release * SAMPLE_RATE * 0.001f;
        releaseStep = 1.0f / releaseSamples;
        printf("release %f ms %ld samples\n", release, releaseSamples);
        return *this;
    }

    AudioGranular& close()
    {
        if (file) {
            sf_close(file);
        }
        return *this;
    }

    AudioGranular& open(const char* filename, int seek = SEEK_END)
    {
        close();

        if (!(file = sf_open(filename, SFM_READ, &sfinfo))) {
            printf("Error: could not open file %s\n", filename);
            return *this;
        }
        printf("Audio file %s sampleCount %ld sampleRate %d\n", filename, (long)sfinfo.frames, sfinfo.samplerate);

        sf_read_float(file, buffer, AUDIO_BUFFER_SIZE);

        return *this;
    }

    int64_t samples(float* buf, int len)
    {
        int i = 0;

        for (; i < len; i++) {
            buf[i] = 0;
        }

        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note != -1) {
                for (i = 0; i < len; i++) {
                    buf[i] += sample(voice);
                }
            }
        }

        return i;
    }

    // TODO envelop on noteon and noteoff

    AudioGranular& noteOn(uint8_t note, uint8_t velocity)
    {
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == -1) {
                voice.note = note;
                float sampleStep = getSampleStep(note);
                for (uint8_t g = 0; g < density; g++) {
                    initGrain(voice.grains[g], sampleStep);
                }
                printf("noteOn: %d %d %f\n", note, velocity, sampleStep);
                return *this;
            }
        }

        // TODO voice stealing
        printf("noteOn: no voice available. Need to implement voice stealing...\n");

        return *this;
    }

    AudioGranular& noteOff(uint8_t note, uint8_t velocity)
    {
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == note) {
                voice.note = -1;
                printf("noteOff set on to false: %d %d\n", note, velocity);
                return *this;
            }
        }

        printf("noteOff: note not found %d %d\n", note, velocity);
        return *this;
    }

    AudioGranular& allOff()
    {
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            voices[v].note = -1;
        }
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