#include "audio.h"
#include "config.h"
#include "def.h"

int main(int argc, char* args[])
{
    if (argc < 2) {
        APP_PRINT("Usage:\n    %s\n    %s --list\n", args[0], args[0]);
        return 1;
    }

    if (strcmp(args[1], "--list") == 0) {
        showAudioDeviceInfo();
        return 0;
    }

    if (loadConfig() == false) {
        return 1;
    }

    RtAudio::StreamParameters audioParams;

    // TODO should sample rate come from RtAudio::DeviceInfo  ?
    unsigned int bufferFrames = APP_AUDIO_CHUNK;
    audioParams.deviceId = getAudioDeviceId(audioOutput);
    audioParams.nChannels = APP_CHANNELS;
    try {
        audio.openStream(&audioParams, NULL, APP_AUDIO_FORMAT, SAMPLE_RATE, &bufferFrames, &audioCallback);
        audio.startStream();
        while (audio.isStreamRunning()) {
            usleep(100000); // 100ms
        }
    } catch (RtAudioError& e) {
        e.printMessage();
        return 1;
    }

    return 0;
}