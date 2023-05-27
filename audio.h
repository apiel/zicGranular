#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "def.h"
#include "audioHandler.h"

AudioHandler& audioHandler = AudioHandler::get();

// float lenDivider = 1.0f / sizeof(float);
// void audioCallBack(void* userdata, Uint8* stream, int len)
// {
//     float* buf = (float*)stream;
//     audio.samples(buf, len * lenDivider);
// }

int audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    audioHandler.samples((float*)outputBuffer, nBufferFrames);
    return 0;
}

void showAudioDeviceInfo()
{
    unsigned int deviceCount = audio.getDeviceCount();

    printf("Found %d audio devices:\n\n", deviceCount);
    RtAudio::DeviceInfo rtInfo;
    for (unsigned int i = 0; i < deviceCount; i++) {
        rtInfo = audio.getDeviceInfo(i);
        if (rtInfo.probed == true) {
            // printf(" (%d) %s", i, rtInfo.name.c_str());
            // printf("  (chan: %d, sampleRate: %d)\n", rtInfo.outputChannels, rtInfo.preferredSampleRate);
            printf(" (%d) %s\n", i, rtInfo.name.c_str());
        }
    }
    printf("\n");
}


#endif