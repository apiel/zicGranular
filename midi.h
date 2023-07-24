#ifndef _MIDI_H_
#define _MIDI_H_

#include "def.h"
#include "fs.h"
#include "midiConfig.h"

#define MIDI_CONFIG_FILE "./midi.cfg"
#define MIDI_CONFIG_LEN 4096
char midiConfig[MIDI_CONFIG_LEN];

#define MIDI_CONTROLS 2
MidiConfig midiControls[MIDI_CONTROLS] = {
    MidiConfig(3, 2, 0xe0),
    MidiConfig(2, 2, 0xd0),
};

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0xf8) {
        // ignore midi clock
    } else if (message->at(0) == 0xfe) {
        // ignore active sensing
    // } else if (message->at(0) >= 0xe0 && message->at(0) <= 0xef) {
    //     // pitch bend
    //     if (message->size() == 3) {
    //         uint8_t channel = message->at(0) & 0x0f;
    //         uint16_t value = (message->at(2) << 7) + message->at(1);
    //         float pct = (float)value / 16383.0f;
    //         printf("Pitch bend (ch %d): %d / 16383 = %f\n", channel, value, pct);
    //     }
    } else {
        printf("Midi controller message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");

        for (int i = 0; i < MIDI_CONTROLS; i++) {
            if (midiControls[i].handle(message)) {
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