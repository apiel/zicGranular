// https://handmade.network/forums/articles/t/6927-audio_api_examples
// Build with: gcc sound_pulse.c -o sound -lm -lpulse -lpulse-simple

#include <stdlib.h>
#include <pulse/simple.h>

#define SAMPLE_RATE  (44100)
#define CHANNELS     (2)

int main(int argc, char **argv) {
    static const pa_sample_spec streamFormat = {
        .format = PA_SAMPLE_S16LE,
        .rate = SAMPLE_RATE,
        .channels = CHANNELS,
    };

    pa_simple *device = pa_simple_new(NULL, NULL, PA_STREAM_PLAYBACK, NULL, 
            "playback", &streamFormat, NULL, NULL, NULL);

    while (1) {
        uint16_t buffer[1024];

        for (int i = 0; i < 512; i++) {
            buffer[i * 2 + 0] = rand();
            buffer[i * 2 + 1] = rand();
        }

        pa_simple_write(device, buffer, 1024, NULL);
    }

    pa_simple_drain(device, NULL);

    return 0;
}