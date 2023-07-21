#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"
#include "fs.h"

#define MIDI_CONFIG_FILE "./midi.cfg"
#define MIDI_CONFIG_LEN 4096
char midiConfig[MIDI_CONFIG_LEN];

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        printf("midi note on: %d\n", message->at(1));
    } else {
        printf("Midi controller message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");
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

bool loadMidi()
{
    if (readFileContent(MIDI_CONFIG_FILE, midiConfig, MIDI_CONFIG_LEN) == false) {
        printf("Midi config file not found: %s\n", MIDI_CONFIG_FILE);
        return false;
    }
    printf("Midi config file loaded: %s\n%s\n", MIDI_CONFIG_FILE, midiConfig);

    // get first line
    char* midiControllerName = strtok(midiConfig, "\n");
    if (midiControllerName == NULL) {
        printf("Midi config file is empty\n");
        return false;
    }

    bool success = loadMidiInput(midiController, midiControllerName, &midiControllerCallback);

    return success;
}

#endif