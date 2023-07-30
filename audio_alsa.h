#ifndef _AUDIO_API_H_
#define _AUDIO_API_H_

#include <alsa/asoundlib.h>

#include "audioHandler.h"
#include "def.h"

class AudioAlsa : public AudioApi {
protected:
    AudioHandler& audioHandler = AudioHandler::get();

    const char* device = "default";
    snd_output_t* output = NULL;

    static AudioAlsa* instance;
    AudioAlsa() { }

public:
    static AudioAlsa& get()
    {
        if (!instance) {
            instance = new AudioAlsa();
        }
        return *instance;
    }

    int open()
    {
        APP_PRINT("AudioAlsa::open\n");

        snd_pcm_t* handle;

        int err;
        if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            APP_INFO("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }
        if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                 APP_CHANNELS, SAMPLE_RATE, 1, 500000))
            < 0) {
            APP_INFO("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }

        while (1) {
            float outputBuffer[APP_AUDIO_CHUNK * APP_CHANNELS];
            audioHandler.samples((float*)outputBuffer, APP_AUDIO_CHUNK * APP_CHANNELS);
            snd_pcm_writei(handle, outputBuffer, APP_AUDIO_CHUNK);
        }

        snd_pcm_close(handle);
        return 0;
    }

    void list()
    {
        int cardNum;
        cardNum = -1;
        while (snd_card_next(&cardNum) > -1 && cardNum > -1) {
            char *name;
            snd_card_get_name(cardNum, &name);
            APP_PRINT("- %s\n", name);
        }

        snd_config_update_free_global();
    }
};

AudioAlsa* AudioAlsa::instance = NULL;

#endif