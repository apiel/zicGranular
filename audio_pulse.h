#ifndef _AUDIO_API_H_
#define _AUDIO_API_H_

#include <pulse/simple.h>

#include "audioHandler.h"
#include "def.h"

AudioHandler& audioHandlerPulse = AudioHandler::get();


int openpulse()
{

    static const pa_sample_spec streamFormat = {
        .format = PA_SAMPLE_FLOAT32LE,
        // .format = PA_SAMPLE_FLOAT32BE,
        .rate = SAMPLE_RATE,
        .channels = APP_CHANNELS,
    };

    pa_simple* device = pa_simple_new(NULL, NULL, PA_STREAM_PLAYBACK, NULL,
        "playback", &streamFormat, NULL, NULL, NULL);

    uint8_t bufferMultiplier = sizeof(float) / sizeof(uint8_t);

    while (1) {
        float outputBuffer[APP_AUDIO_CHUNK];
        audioHandlerPulse.samples((float*)outputBuffer, APP_AUDIO_CHUNK);

        pa_simple_write(device, outputBuffer, APP_AUDIO_CHUNK * bufferMultiplier, NULL);
    }

    pa_simple_drain(device, NULL);

    return 0;
}


class AudioPulse : public AudioApi {
protected:
    static AudioPulse* instance;
    AudioPulse() { }

public:
    static AudioPulse& get()
    {
        if (!instance) {
            instance = new AudioPulse();
        }
        return *instance;
    }

    int open()
    {
        return openpulse();
    }

    void list()
    {
        // showAudioDeviceInfo();
    }
};

AudioPulse* AudioPulse::instance = NULL;

#endif