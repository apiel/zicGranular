#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "audioHandler.h"
#include "def.h"

AudioHandler& audioHandler = AudioHandler::get();

int audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    audioHandler.samples((float*)outputBuffer, nBufferFrames * APP_CHANNELS);
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

unsigned int getAudioDeviceId(char * name)
{
    unsigned int deviceCount = audio.getDeviceCount();

    RtAudio::DeviceInfo rtInfo;
    for (unsigned int i = 0; i < deviceCount; i++) {
        rtInfo = audio.getDeviceInfo(i);
        if (rtInfo.probed == true) {
            if (rtInfo.name.find(name) != std::string::npos) {
                return i;
            }
        }
    }
    return 0;
}

#endif