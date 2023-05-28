#ifndef _AUDIO_GRANULAR_H_
#define _AUDIO_GRANULAR_H_

#include "audioFile.h"
#include "def.h"

class AudioGranular {
public:
    AudioFile audioFile;

    AudioGranular()
    {
        // audioFile.open("samples/0_squeaky.wav");
        // audioFile.open("samples/0_fx_01.wav");
        audioFile.open("samples/0_axxe13.wav");
    }

    void noteOn()
    {
        audioFile.restart();
    }

    int64_t samples(float* buf, int len)
    {
        return audioFile.samples(buf, len);
    }
};

#endif