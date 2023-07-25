#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"
#include "fs.h"
#include "midiMapping.h"

#define MIDI_MAPS 3
MidiMapping midiMappings[MIDI_MAPS] = {
    MidiMapping("GRAIN_START_POSITION", [](uint16_t value, float pct) {
        printf("GRAIN_START_POSITION value %d and percentage %f\n", value, pct);
    }),
    MidiMapping("GRAIN_DENSIY", [](uint16_t value, float pct) {
        printf("GRAIN_DENSIY value %d and percentage %f\n", value, pct);
    }),
    MidiMapping("GRAIN_SIZE", [](uint16_t value, float pct) {
        printf("GRAIN_SIZE value %d and percentage %f\n", value, pct);
    }),
};

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0xf8) {
        // ignore midi clock
    } else if (message->at(0) == 0xfe) {
        // ignore active sensing
    } else {
        printf("Midi controller message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");

        for (int i = 0; i < MIDI_MAPS; i++) {
            if (midiMappings[i].handle(message)) {
                break;
            }
        }
    }
}

bool loadMidiInput(RtMidiIn& midi, const char* portName, RtMidiIn::RtMidiCallback callback)
{
    unsigned int portCount = midi.getPortCount();

    for (unsigned int i = 0; i < portCount; i++) {
        if (midi.getPortName(i).find(portName) != std::string::npos) {
            midi.openPort(i);
            midi.setCallback(callback);
            midi.ignoreTypes(false, false, false);
            printf("Midi input loaded: %s\n", midi.getPortName(i).c_str());
            return true;
        }
    }

    printf("Midi input %s not found\n", portName);
    return false;
}

#endif