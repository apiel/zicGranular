#ifndef _AUDIO_P_H_
#define _AUDIO_P_H_

#include "audioHandler.h"
#include "def.h"

AudioHandler& audioHandlerPulse = AudioHandler::get();

// int audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
//     double /*streamTime*/, RtAudioStreamStatus status, void* data)
// {
//     audioHandler.samples((float*)outputBuffer, nBufferFrames * APP_CHANNELS);
//     return 0;
// }
#include <pulse/simple.h>

int openpulse() {
    static const pa_sample_spec streamFormat = {
        .format = PA_SAMPLE_FLOAT32LE,
        // .format = PA_SAMPLE_FLOAT32BE,
        .rate = SAMPLE_RATE,
        .channels = APP_CHANNELS,
    };

    pa_simple *device = pa_simple_new(NULL, NULL, PA_STREAM_PLAYBACK, NULL, 
            "playback", &streamFormat, NULL, NULL, NULL);

    while (1) {
        float outputBuffer[1024];
        audioHandlerPulse.samples((float*)outputBuffer, 256);

        pa_simple_write(device, outputBuffer, 1024, NULL);
    }

    pa_simple_drain(device, NULL);

    return 0;
}

#endif