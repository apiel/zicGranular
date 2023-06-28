#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"
#include "midiController.h"
#include "midiKeyboard.h"

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

bool loadMidiOutput(RtMidiOut& midi, const char* portName)
{
    unsigned int portCount = midi.getPortCount();

    for (unsigned int i = 0; i < portCount; i++) {
        if (midi.getPortName(i).find(portName) != std::string::npos) {
            midi.openPort(i);
            printf("Midi output loaded: %s\n", midi.getPortName(i).c_str());
            return true;
        }
    }

    printf("Midi output %s not found\n", portName);
    return false;
}

bool loadMidi()
{
    bool success = loadMidiInput(midiController, "APC Key 25 mk2 C", &midiControllerCallback)
        && loadMidiOutput(midiControllerOut, "APC Key 25 mk2 C")
        && loadMidiInput(midiKeyboard, "APC Key 25 mk2 K", &midiKeyboardCallback);

    if (success) {
        midiControllerRenderGrainStart();
    }

    return success;
}

#endif