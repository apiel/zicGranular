#ifndef _MIDI_CONTROLLER_H_
#define _MIDI_CONTROLLER_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"
#include "fileBrowser.h"
#include "midiControllerDef.h"
#include "midiControllerGrainStart.h"
#include "midiControllerSampleSelector.h"
#include "midiControllerInterface.h"

void midiControlerRenderSelector()
{
    sendPad(pad::ClipStop, padMode::_On);
    // sendPad(pad::Solo, padMode::_Blink);
}

void midiControllerRender()
{
    midiControlerRenderSelector();
    switch (midiControllerMode) {
    case MidiControllerMode::GRAIN_START_SELECTOR:
        MidiControllerGrainStart::get().render();
        break;
    case MidiControllerMode::SAMPLE_SELCETOR:
        MidiControllerSampleSelector::get().render();
        break;
    }
}

void midiControllerRender(uint8_t mode)
{
    midiControllerMode = mode;
    midiControllerRender();
}

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        // TODO support all row of matrix
        if (message->at(1) >= 0x20 && message->at(1) <= 0x27) {
            uint8_t index = message->at(1) - 0x20;
            switch (midiControllerMode) {
            case MidiControllerMode::GRAIN_START_SELECTOR:
                MidiControllerGrainStart::get().noteOnMatrix(index);
                break;
            case MidiControllerMode::SAMPLE_SELCETOR: {
                MidiControllerSampleSelector::get().noteOnMatrix(index);
                break;
            }
            }

        } else if (message->at(1) == pad::ClipStop) {
            // here we should select the selector...
            // and then
            midiControllerRender(MidiControllerMode::SAMPLE_SELCETOR);
        } else {
            printf("midi note on: %d\n", message->at(1));
        }
    } else if (message->at(0) == 0x80) {
        if (message->at(1) == pad::ClipStop) {
            midiControllerRender(MidiControllerMode::GRAIN_START_SELECTOR);
        } else {
            printf("midi note off: %d\n", message->at(1));
        }
    } else if (message->at(0) == 0xB0) {
        printf("midi controller: %d\n", message->at(1));
    } else {
        printf("Midi controller message: ");
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++) {
            printf("%02x ", (int)message->at(i));
        }
        printf("\n");
    }
}

#endif