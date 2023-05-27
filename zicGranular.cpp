#include "audioHandler.h"
#include "def.h"

AudioHandler& audio = AudioHandler::get();

// float lenDivider = 1.0f / sizeof(float);
// void audioCallBack(void* userdata, Uint8* stream, int len)
// {
//     float* buf = (float*)stream;
//     audio.samples(buf, len * lenDivider);
// }

int audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    audio.samples((float*)outputBuffer, nBufferFrames);
    return 0;
}

void showAudioDeviceInfo()
{
    RtAudio audio;
    unsigned int deviceCount = audio.getDeviceCount();

    RtAudio::DeviceInfo rtInfo;
    for (unsigned int i = 0; i < deviceCount; i++) {
        rtInfo = audio.getDeviceInfo(i);
        if (rtInfo.probed == true) {
            printf("Device %d: %s", i, rtInfo.name.c_str());
            printf("  (chan: %d, sampleRate: %d)\n", rtInfo.outputChannels, rtInfo.preferredSampleRate);
        }
    }
}

int main(int argc, char* args[])
{
    if (argc < 2) {
        printf("Usage:\n    %s <audio device id>\n    %s --list\n", args[0], args[0]);
        return 1;
    }

    if (strcmp(args[1], "--list") == 0) {
        showAudioDeviceInfo();
        return 0;
    }

    unsigned int deviceId = atoi(args[1]);

    RtAudio audio;
    RtAudio::StreamParameters audioParams;

    // TODO should sample rate come from RtAudio::DeviceInfo  ?
    unsigned int bufferFrames = APP_AUDIO_CHUNK;
    audioParams.deviceId = deviceId;
    audioParams.nChannels = APP_CHANNELS;
    try {
        audio.openStream(&audioParams, NULL, APP_AUDIO_FORMAT, SAMPLE_RATE, &bufferFrames, &audioCallback);
        audio.startStream();
        while(audio.isStreamRunning()) {
            usleep(100000); // 100ms
        }
    } catch (RtAudioError& e) {
        e.printMessage();
        return 1;
    }

    return 0;
}