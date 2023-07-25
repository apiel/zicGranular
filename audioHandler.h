#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioGranular.h"
#include "def.h"
#include "fileBrowser.h"
#include "master.h"
#include "tempo.h"

class AudioHandler {
protected:
    Master& master = Master::get();

    static AudioHandler* instance;

    AudioHandler()
    {     
        audioGranular.open(fileBrowser.getFile(0));
    }

public:
    Tempo& tempo = Tempo::get();
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
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif