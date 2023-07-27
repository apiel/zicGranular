#include "audio.h"
#include "config.h"
#include "def.h"

#define AUDIO_API pulse

#if AUDIO_API == pulse
#include "audio_pulse.h"
AudioApi& audioApi = AudioPulse::get();
#else
#include "audio_rt.h"
AudioApi& audioApi = AudioRT::get();
#endif

int main(int argc, char* args[])
{
    if (argc > 1) {
        if (strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0) {
            APP_PRINT("Usage:\n    %s\n    %s --list\n", args[0], args[0]);
            return 0;
        }

        if (strcmp(args[1], "--list") == 0) {
            // showAudioDeviceInfo();
            audioApi.list();
            return 0;
        }
    }

    // TODO make config.cfg a parameter
    // make default config
    if (loadConfig() == false) {
        return 1;
    }

    return audioApi.open();
}