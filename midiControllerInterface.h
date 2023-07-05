#ifndef _MIDI_CONTROLLER_INTERFACE_H_
#define _MIDI_CONTROLLER_INTERFACE_H_

#include "def.h"

class MidiControllerInterface {
public:
    virtual void render() = 0;
    virtual void noteOnMatrix(uint8_t index) = 0;
};

#endif