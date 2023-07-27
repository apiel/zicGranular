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

    APP_PRINT("Found %d audio devices:\n\n", deviceCount);
    RtAudio::DeviceInfo rtInfo;
    for (unsigned int i = 0; i < deviceCount; i++) {
        rtInfo = audio.getDeviceInfo(i);
        if (rtInfo.probed == true) {
            // APP_PRINT(" (%d) %s", i, rtInfo.name.c_str());
            // APP_PRINT("  (chan: %d, sampleRate: %d)\n", rtInfo.outputChannels, rtInfo.preferredSampleRate);
            APP_PRINT(" (%d) %s\n", i, rtInfo.name.c_str());
        }
    }
    APP_PRINT("\n");
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
    debug("/!\\ Warning /!\\ Audio device %s not found\n", name);
    rtInfo = audio.getDeviceInfo(0);
    debug("Using default device [%s]\n", rtInfo.name.c_str());
    return 0;
}

#endif