#ifndef _MIDI_MAPPING_H_
#define _MIDI_MAPPING_H_

#include "def.h"

class MidiMapping {
protected:
    bool isValid(std::vector<unsigned char>* message)
    {
        if (message->size() != size) {
            return false;
        }
        if (msg[0] != message->at(0)) {
            return false;
        }
        if (valuePosition == 3 && msg[1] != message->at(1)) {
            return false;
        }
        return true;
    }

public:
    const char* key;
    uint8_t size = 0;
    uint8_t valuePosition = 0;
    uint8_t msg[2] = { 0x00, 0x00 };

    MidiMapping(const char* _key)
        : key(_key)
    {
    }

    bool handle(std::vector<unsigned char>* message)
    {
        if (isValid(message)) {
            uint16_t value = 0;
            float pct = 0.0f;
            if (valuePosition == size) {
                value = message->at(valuePosition - 1);
                pct = (float)value / 127.0f;
            } else {
                value = (message->at(2) << 7) + message->at(1);
                pct = (float)value / 16383.0f;
            }

            printf("Midi message matched: ");
            unsigned int nBytes = message->size();
            for (unsigned int i = 0; i < nBytes; i++) {
                printf("%02x ", (int)message->at(i));
            }
            printf(" = %d = %f\n", value, pct);

            return true;
        }
        return false;
    }
};

#endif