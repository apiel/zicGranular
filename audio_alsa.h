#ifndef _AUDIO_API_H_
#define _AUDIO_API_H_

#include <alsa/asoundlib.h>

#include "audioHandler.h"
#include "def.h"

class AudioAlsa : public AudioApi {
protected:
    AudioHandler& audioHandler = AudioHandler::get();

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
        if ((err = snd_pcm_open(&handle, audioOutput, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            APP_INFO("Playback open audio card \"%s\" error: %s.\nOpen default sound card\n", audioOutput, snd_strerror(err));
            if ((err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
                APP_INFO("Default playback audio card error: %s\n", snd_strerror(err));
                return 1;
            }
        }
        // if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED,
        //          APP_CHANNELS, SAMPLE_RATE, 1, 500000))
        //     < 0) {
        //     APP_INFO("Audio card params error: %s\n", snd_strerror(err));
        //     return 1;
        // }
        if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S32, SND_PCM_ACCESS_RW_INTERLEAVED,
                 APP_CHANNELS, SAMPLE_RATE, 1, 500000))
            < 0) {
            APP_INFO("Audio card params error: %s\n", snd_strerror(err));
            return 1;
        }

        // while (1) {
        //     float outputBuffer[APP_AUDIO_CHUNK * APP_CHANNELS];
        //     audioHandler.samples((float*)outputBuffer, APP_AUDIO_CHUNK * APP_CHANNELS);
        //     snd_pcm_writei(handle, outputBuffer, APP_AUDIO_CHUNK);
        // }
        while (1) {
            float outputBuffer[APP_AUDIO_CHUNK * APP_CHANNELS];
            audioHandler.samples((float*)outputBuffer, APP_AUDIO_CHUNK * APP_CHANNELS);
            int32_t outputBuffer32[APP_AUDIO_CHUNK * APP_CHANNELS];
            for (int i = 0; i < APP_AUDIO_CHUNK * APP_CHANNELS; i++) {
                outputBuffer32[i] = (int32_t)(outputBuffer[i] * 2147483647.0f);
            }
            snd_pcm_writei(handle, outputBuffer32, APP_AUDIO_CHUNK);
        }

        snd_pcm_close(handle);
        return 0;
    }

    void list()
    {
        int cardNum = -1;
        while (snd_card_next(&cardNum) > -1 && cardNum > -1) {
            char* name;
            snd_card_get_name(cardNum, &name);
            APP_PRINT("- %s [AUDIO_OUTPUT=hw:%d,0]\n", name, cardNum);
        }

        snd_config_update_free_global();
    }
};

AudioAlsa* AudioAlsa::instance = NULL;

#endif