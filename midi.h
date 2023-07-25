#ifndef _MIDI_H_
#define _MIDI_H_

#include "audioHandler.h"
#include "def.h"
#include "fs.h"
#include "midiMapping.h"

MidiMapping midiMappings[] = {
    MidiMapping("GRAIN_START_POSITION", [](float value) {
        AudioHandler::get().audioGranular.setStart(value);
    }),
    MidiMapping("GRAIN_DENSIY", [](float value) {
        AudioHandler::get().audioGranular.setDensity(value);
    }),
    MidiMapping("GRAIN_SIZE", [](float value) {
        AudioHandler::get().audioGranular.setGrainSize(value);
    }),
    MidiMapping("GRAIN_SPRAY", [](float value) {
        AudioHandler::get().audioGranular.setSpray(value);
    }),
    MidiMapping("GRAIN_DELAY", [](float value) {
        AudioHandler::get().audioGranular.setDelay(value);
    }),
    MidiMapping("MASTER_VOLUME", [](float value) {
        setMasterVolume(value);
    }),
    MidiMapping("ENV_ATTACK", [](float value) {
        AudioHandler::get().audioGranular.setAttack(value);
    }),
    MidiMapping("ENV_RELEASE", [](float value) {
        AudioHandler::get().audioGranular.setRelease(value);
    }),
    MidiMapping("SAMPLE_SELECTOR", [](float value) {
        FileBrowser& fileBrowser = AudioHandler::get().fileBrowser;
        uint8_t position = fileBrowser.count * value;
        if (position != fileBrowser.position) {
            char *file = fileBrowser.getFile(position);
            debug("SAMPLE_SELECTOR: %f %s\n", value, file);
            AudioHandler::get().audioGranular.open(file);
        }
    }),
};

const uint8_t MIDI_MAPS = sizeof(midiMappings) / sizeof(midiMappings[0]);

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0xf8) {
        // ignore midi clock
    } else if (message->at(0) == 0xfe) {
        // ignore active sensing
    } else if (message->at(0) >= 0x90 && message->at(0) < 0xa0) {
        uint8_t channel = message->at(0) - 0x90;
        if (channel == midiSequencerChannel) {
            AudioHandler::get().audioGranular.noteOn(message->at(1), message->at(2));
        }
    } else if (message->at(0) >= 0x80 && message->at(0) < 0x90) {
        uint8_t channel = message->at(0) - 0x80;
        if (channel == midiSequencerChannel) {
            AudioHandler::get().audioGranular.noteOff(message->at(1), message->at(2));
        }
    } else {
        for (int i = 0; i < MIDI_MAPS; i++) {
            if (midiMappings[i].handle(message)) {
                return;
            }
        }
        if (debugMode) {
            debug("Midi controller message: ");
            unsigned int nBytes = message->size();
            for (unsigned int i = 0; i < nBytes; i++) {
                debug("%02x ", (int)message->at(i));
            }
            debug("\n");
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
            APP_INFO("Midi input loaded: %s\n", midi.getPortName(i).c_str());
            return true;
        }
    }

    APP_INFO("Midi input %s not found\n", portName);
    return false;
}

#endif