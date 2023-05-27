#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"

void onMidiNoteOn(uint8_t note, uint8_t velocity)
{
    printf("Midi note on: %d %d\n", note, velocity);
}

void onMidiNoteOff(uint8_t note, uint8_t velocity)
{
    printf("Midi note off: %d %d\n", note, velocity);
}

void onMidiControlChange(uint8_t control, uint8_t value)
{
    printf("Midi control change: %d %d\n", control, value);
}

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        onMidiNoteOn(message->at(1), message->at(2));
    } else if (message->at(0) == 0x80) {
        onMidiNoteOff(message->at(1), message->at(2));
    } else if (message->at(0) == 0xB0) {
        onMidiControlChange(message->at(1), message->at(2));
    } else {
        printf("Midi message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");
    }
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

#endif