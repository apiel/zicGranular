#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioGranular.h"
#include "def.h"
#include "distortion.h"
#include "fileBrowser.h"
#include "filter.h"
#include "master.h"

class AudioHandler {
protected:
    Master& master = Master::get();

    static AudioHandler* instance;

    AudioHandler()
    {
        audioGranular.open(fileBrowser.getFile(0));
    }

public:
    Filter filter;
    Distortion distortion;
    AudioGranular audioGranular;
    FileBrowser fileBrowser = FileBrowser("./samples");

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
        audioGranular.samples(buf, len);

        for (int i = 0; i < len; i++) {
            buf[i] = distortion.sample(filter.sample(buf[i])) * masterVolumeWithGain;
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif