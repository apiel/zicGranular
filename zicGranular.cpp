#include "def.h"
#include "audioHandler.h"

std::vector<RtMidi::Api> apis;
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

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    unsigned int nBytes = message->size();
    for (unsigned int i = 0; i < nBytes; i++) {
        printf("%02x ", (int)message->at(i));
    }
    printf("\n");
}

bool loadMidiInput()
{
    unsigned int portCount = midi.getPortCount();

    for (unsigned int i = 0; i < portCount; i++) {
        if (midi.getPortName(i).find("FL STUDIO FIRE") != std::string::npos) {
            midi.openPort(i);
            midi.setCallback(&midiCallback);
            midi.ignoreTypes(false, false, false);
            printf("Midi input loaded: %s\n", midi.getPortName(i).c_str());
            return true;
        }
    }

    printf("FL STUDIO FIRE not found\n");
    return false;
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

    if (loadMidiInput() == false) {
        return 1;
    }

    unsigned int deviceId = atoi(args[1]);

    RtAudio::StreamParameters audioParams;

    // TODO should sample rate come from RtAudio::DeviceInfo  ?
    unsigned int bufferFrames = APP_AUDIO_CHUNK;
    audioParams.deviceId = deviceId;
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