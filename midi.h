#ifndef _MIDI_H_
#define _MIDI_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"

void onMidiNoteOn(uint8_t note, uint8_t velocity)
{
    if (note < 20) {
        return;
    }
    printf("Midi note on: %d %d\n", note, velocity);
    AudioHandler::get().audioGranular.noteOn(note, velocity);
}

void onMidiNoteOff(uint8_t note, uint8_t velocity)
{
    if (note < 20) {
        return;
    }
    printf("Midi note off: %d %d\n", note, velocity);
    AudioHandler::get().audioGranular.noteOff(note, velocity);
}

void onMidiControlChange(uint8_t control, int8_t value)
{
    int8_t direction = value > 64 ? value - 128 : value;
    // printf("Midi control change: %d %d %d\n", control, value, direction);

    AudioGranular& granular = AudioHandler::get().audioGranular;
    switch (control) {
    case 16:
        granular.setDensity(granular.density + direction);
        break;

    case 17:
        granular.setGrainSize(granular.grainSize + direction);
        break;

    case 18:
        granular.setSpray(granular.spray + direction);
        break;

    default:
        break;
    }
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