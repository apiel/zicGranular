#ifndef _AUDIO_API_H_
#define _AUDIO_API_H_

#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#include "audioHandler.h"
#include "def.h"

static void pa_set_sink_info(pa_context* c, const pa_sink_info* i,
    int eol, void* userdata)
{
    if (eol) {
        pa_mainloop_api* mlApi = (pa_mainloop_api*)userdata;
        mlApi->quit(mlApi, 0);
        return;
    }

    // printf("name: %s (outputChannels %d preferredSampleRate %d)\n", pa_proplist_gets(i->proplist, "device.description"), i->sample_spec.channels, i->sample_spec.rate);
    APP_PRINT("- %s [AUDIO_OUTPUT=%s]\n", pa_proplist_gets(i->proplist, "device.description"), i->name);
}

static void pa_context_state_callback(pa_context* context, void* userdata)
{
    pa_mainloop_api* mlApi = (pa_mainloop_api*)userdata;

    switch (pa_context_get_state(context)) {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
        debug("PA_CONTEXT_READY\n");
        pa_context_get_sink_info_list(context, pa_set_sink_info, userdata); // output info ... needs to be before input
        // pa_context_get_source_info_list(context, pa_set_source_info_and_quit, userdata); // input info
        break;

    case PA_CONTEXT_TERMINATED:
        mlApi->quit(mlApi, 0);
        break;

    case PA_CONTEXT_FAILED:
    default:
        debug("PA_CONTEXT_FAILED\n");
        mlApi->quit(mlApi, 0);
    }
}

class AudioPulse : public AudioApi {
protected:
    AudioHandler& audioHandler = AudioHandler::get();
    static AudioPulse* instance;
    AudioPulse() { }

    pa_mainloop* ml = NULL;
    pa_context* context = NULL;
    char* server = NULL;

    void freeListDevice()
    {
        if (context)
            pa_context_unref(context);

        if (ml)
            pa_mainloop_free(ml);

        pa_xfree(server);
    }

    void listDevices()
    {
        if (!(ml = pa_mainloop_new())) {
            debug("pa_mainloop_new() failed.\n");
            return freeListDevice();
        }

        pa_mainloop_api* paMainLoopApi = pa_mainloop_get_api(ml);

        if (!(context = pa_context_new_with_proplist(paMainLoopApi, NULL, NULL))) {
            debug("pa_context_new() failed.\n");
            return freeListDevice();
        }

        pa_context_set_state_callback(context, pa_context_state_callback, paMainLoopApi);

        if (pa_context_connect(context, server, PA_CONTEXT_NOFLAGS, NULL) < 0) {
            debug("pa_context_connect() failed: %s\n", pa_strerror(pa_context_errno(context)));
            return freeListDevice();
        }

        int ret = 1;
        if (pa_mainloop_run(ml, &ret) < 0) {
            debug("pa_mainloop_run() failed.\n");
        }
        freeListDevice();
    }

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
        APP_PRINT("AudioPulse::open\n");

        static const pa_sample_spec streamFormat = {
            .format = PA_SAMPLE_FLOAT32LE,
            .rate = SAMPLE_RATE,
            .channels = APP_CHANNELS,
        };

        pa_simple* device = pa_simple_new(NULL, NULL, PA_STREAM_PLAYBACK, audioOutput, "zicGranular", &streamFormat, NULL, NULL, NULL);

        uint8_t bufferMultiplier = sizeof(float) / sizeof(uint8_t);

        while (1) {
            float outputBuffer[APP_AUDIO_CHUNK];
            audioHandler.samples((float*)outputBuffer, APP_AUDIO_CHUNK);

            pa_simple_write(device, outputBuffer, APP_AUDIO_CHUNK * bufferMultiplier, NULL);
        }

        pa_simple_drain(device, NULL);

        return 0;
    }

    void list()
    {
        listDevices();
    }
};

AudioPulse* AudioPulse::instance = NULL;

#endif