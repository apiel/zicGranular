#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "def.h"
#include "master.h"
#include "tempo.h"
#include "audioGranular.h"

class AudioHandler {
protected:
    Master& master = Master::get();

    static AudioHandler* instance;

    AudioHandler() { }

public:
    Tempo& tempo = Tempo::get();
    AudioGranular audioGranular;

    uint8_t stepCounter = 0;

    static AudioHandler& get()
    {
        if (!instance) {
            instance = new AudioHandler();
        }
        return *instance;
    }

    void samples(float* buf, int len)
    {
        // unsigned long now = SDL_GetTicks();
        // if (tempo.next(now))
        // // if (tempo.next(SDL_GetTicks64()))
        // // if (tempo.next())
        // {
        //     stepCounter = (stepCounter + 1) % APP_TRACK_STEPS;
        //     for (uint8_t i = 0; i < APP_TRACKS; i++) {
        //         if (data.tracks[i].next(stepCounter)) {
        //             // needRender = true;
        //         }
        //     }
        // }

        // for (int j = 0; j < len; j++) {
        //     buf[j] = 0.0f;
        // }

        // float* buffer = new float[len];
        // for (uint8_t i = 0; i < APP_TRACKS; i++) {
        //     Track& track = data.tracks[i];
        //     if (track.active) {
        //         track.audioFile.samples(buffer, len);
        //         for (int j = 0; j < len; j++) {
        //             // buf[j] += track.filter.sample(buffer[j]) * mixDivider * track.activeStep->velocity * track.volume;
        //             buf[j] += track.sampleModulation(buffer[j], master.mixDivider);
        //         }
        //     }
        // }
        // delete[] buffer;

        // for (int j = 0; j < len; j++) {
        //     buf[j] = master.filter.sample(buf[j]);
        // }

        audioGranular.samples(buf, len);
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif