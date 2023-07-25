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
#define MIN_GRAIN_SIZE_MS 20

const uint16_t minGrainSampleCount = MIN_GRAIN_SIZE_MS * SAMPLE_RATE * 0.001f;

class AudioGranular {
protected:
    uint64_t voicePosition = 0;
    float buffer[AUDIO_BUFFER_SIZE];
    float attackStep = 0.0f;
    float releaseStep = 0.0f;

    struct Grain {
        float pos;
        int64_t start;
        int64_t sampleCount;
        int64_t delay;
        float sampleStep;
    };

    struct Voice {
        int8_t note = -1;
        Grain grains[MAX_GRAINS_PER_VOICE];
        float (AudioGranular::*envelop)(Voice& voice) = &AudioGranular::envelopSustain;
        float env = 0.0f;
        uint64_t position = 0;
    } voices[MAX_GRAIN_VOICES];

    uint8_t baseNote = 60;
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

    int getRand()
    {
        // could create a lookup table ?
        srand(time(0));
        return rand();
    }

    float getRandPct()
    {
        return getRand() / (float)RAND_MAX;
    }

    void initGrain(Grain& grain, float sampleStep)
    {
        grain.sampleStep = sampleStep;
        grain.pos = 0.0f;

        // sprayToAdd is a random value between 0 and spray from starting point till end of file
        float sprayToAdd = spray ? (getRandPct() * spray * (1 - start)) : 0.0;
        grain.start = (start + sprayToAdd) * sfinfo.frames;

        // we deduct minGrainSampleCount to avoid grainSize to be too small
        grain.sampleCount = (sfinfo.frames - (grain.start + minGrainSampleCount)) * grainSize + minGrainSampleCount;

        grain.delay = delay ? (getRand() % delay) : 0;
    }

    float envelopAttack(Voice& voice)
    {
        voice.env += attackStep;
        if (voice.env >= 1.0f) {
            voice.env = 1.0f;
            voice.envelop = &AudioGranular::envelopSustain;
            // printf("envelopAttack finished, set env to %f\n", voice.env);
        }
        return voice.env;
    }

    float envelopSustain(Voice& voice)
    {
        return voice.env;
    }

    float envelopRelease(Voice& voice)
    {
        voice.env -= releaseStep;
        if (voice.env <= 0.0f) {
            voice.env = 0.0f;
            voice.note = -1;
            // printf("envelopRelease finished, set env to %f\n", voice.env);
        }
        return voice.env;
    }

    float sample(Voice& voice)
    {
        float sample = 0.0f;
        float env = (this->*voice.envelop)(voice);
        if (env <= 0.0f) {
            return sample;
        }

        for (uint8_t d = 0; d < density; d++) {
            Grain& grain = voice.grains[d];
            if (grain.delay > 0) {
                grain.delay--;
            } else {
                int64_t samplePos = (uint64_t)grain.pos + grain.start;
                // if (samplePos < sfinfo.frames && (int64_t)grain.pos < grainSampleCount) { // is samplePos < sfinfo.frames even necessary if start calculated properly
                if ((int64_t)grain.pos < grain.sampleCount) {
                    grain.pos += grain.sampleStep;
                    sample += buffer[samplePos] * env;
                } else {
                    initGrain(grain);
                }
            }
        }
        return sample;
    }

    Voice& getNextVoice(uint8_t note)
    {
        // First, we should look if the voice is not already running, due to the envelopRelease
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == note) {
                printf("getNextVoice: voice already running %d\n", note);
                return voice;
            }
        }

        // Else, we should look for a free voice
        uint8_t voiceToSteal = 0;
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == -1) {
                return voice;
            }
            if (voice.position < voices[voiceToSteal].position) {
                voiceToSteal = v;
            }
        }

        printf("getNextVoice: no voice available. Steal voice %d.\n", voiceToSteal);
        return voices[voiceToSteal];
    }

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    uint8_t density = 4;
    float start = 0.0f;
    float grainSize = 0.2;
    float spray = 0.1;
    uint64_t delay = 0;
    uint16_t attack = 300;
    uint16_t release = 1000;

    AudioGranular()
    {
        memset(&sfinfo, 0, sizeof(sfinfo));

        open("samples/0_0_axxe13.wav");

        // allOff();
        setGrainSize(grainSize);
        setAttack(attack);
        setRelease(release);
    }

    ~AudioGranular()
    {
        close();
    }

    /**
     * @brief Set the Grain Size meaning the length duration of the grain.
     *
     * @param grainSize
     * @return AudioGranular&
     */
    AudioGranular& setGrainSize(float value)
    {
        grainSize = range(value, 0.0, 1.0);
        printf("grainSize %f\n", grainSize);
        return *this;
    }

    /**
     * @brief Set the Spray of the grain start position, giving random position to
     * the grain start position.
     *
     * @param spray
     * @return AudioGranular&
     */
    AudioGranular& setSpray(float value)
    {
        spray = range(value, 0.0, 1.0);
        printf("spray %f\n", spray);
        return *this;
    }

    /**
     * @brief Set the Density meaning the number of grains that are played at the same time.
     *
     * @param density
     * @return AudioGranular&
     */
    AudioGranular& setDensity(float value)
    {
        density = value * (MAX_GRAINS_PER_VOICE - 1) + 1; // 1 to MAX_GRAINS_PER_VOICE
        printf("density %d\n", density);
        return *this;
    }

    // delay 0 to 40 (or 80, or 120) for ??????

    /**
     * @brief Set the Delay before grain start to play
     *
     * @param delay where 0 is no delay and 1 is 1000ms
     * @return AudioGranular&
     */
    AudioGranular& setDelay(float value)
    {
        // delay = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 0.001f * 1000;
        // can be simplified to:
        delay = range(value, 0.0f, 1.0f) * SAMPLE_RATE;
        printf("delay %ld\n", delay);
        return *this;
    }

    /**
     * @brief Set the Start position of the sample to play
     *
     * @param _start position from 0.0 to 1.0, where 0.0 is the start of the sample
     * and 1.0 the end of the sample
     * @return AudioGranular&
     */
    AudioGranular& setStart(float value)
    {
        start = range(value, 0.0f, 1.0f);
        printf("setStart %f\n", start);
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
        printf("attack %d ms %ld samples %f step\n", attack, attackSamples, attackStep);
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
        printf("release %d ms %ld samples %f step\n", release, releaseSamples, releaseStep);
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
                    buf[i] += sample(voice) * masterVolumeWithGain;
                }
            }
        }

        return i;
    }

    AudioGranular& noteOn(uint8_t note, uint8_t velocity)
    {
        if (velocity == 0) {
            return noteOff(note, velocity);
        }

        Voice& voice = getNextVoice(note);
        voice.position = voicePosition++;
        voice.note = note;
        voice.envelop = &AudioGranular::envelopAttack;
        float sampleStep = getSampleStep(note);
        for (uint8_t g = 0; g < density; g++) {
            initGrain(voice.grains[g], sampleStep);
        }
        printf("noteOn: %d %d %f\n", note, velocity, sampleStep);

        return *this;
    }

    AudioGranular& noteOff(uint8_t note, uint8_t velocity)
    {
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == note) {
                voice.envelop = &AudioGranular::envelopRelease;
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
        voicePosition = 0;

        // or should we instead do?
        // for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
        //     Voice& voice = voices[v];
        //     voice.envelop = &AudioGranular::envelopRelease;
        // }
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