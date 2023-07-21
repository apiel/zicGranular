#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"
#include "midiController.h"
#include "midiKeyboard.h"
#include "fs.h"

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


#define MIDI_CONFIG_FILE "./midi.cfg"
#define MIDI_CONFIG_LEN 4096
char midiConfig[MIDI_CONFIG_LEN];

bool loadMidi()
{
    if (readFileContent(MIDI_CONFIG_FILE, midiConfig, MIDI_CONFIG_LEN) == false) {
        printf("Midi config file not found: %s\n", MIDI_CONFIG_FILE);
        return false;
    }
    printf("Midi config file loaded: %s\n%s\n", MIDI_CONFIG_FILE, midiConfig);

    bool success = loadMidiInput(midiController, "APC Key 25 mk2 C", &midiControllerCallback)
        && loadMidiOutput(midiControllerOut, "APC Key 25 mk2 C")
        && loadMidiInput(midiKeyboard, "APC Key 25 mk2 K", &midiKeyboardCallback);

    if (success) {
        midiControllerRender();
    }

    return success;
}

#endif