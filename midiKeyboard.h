#ifndef _MIDI_KEYBOARD_H_
#define _MIDI_KEYBOARD_H_

#include "audioHandler.h"
#include "def.h"

enum keyboardCC {
    sustain = 0x40,
};

void midiKeyboardCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        AudioHandler::get().audioGranular.noteOn(message->at(1), message->at(2));
    } else if (message->at(0) == 0x80) {
        AudioHandler::get().audioGranular.noteOff(message->at(1), message->at(2));
    } else {
        printf("Midi keyboard message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");
    }
}

#endif