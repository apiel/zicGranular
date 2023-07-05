#ifndef _MIDI_CONTROLLER_H_
#define _MIDI_CONTROLLER_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"
#include "fileBrowser.h"
#include "midiControllerDef.h"
#include "midiControllerGrainStart.h"
#include "midiControllerInterface.h"
#include "midiControllerSampleSelector.h"

MidiControllerInterface* midiControllerMode = &MidiControllerGrainStart::get();

void midiControlerRenderSelector()
{
    sendPad(pad::ClipStop, padMode::_On);
    // sendPad(pad::Solo, padMode::_Blink);
}

void midiControllerRender()
{
    midiControlerRenderSelector();
    midiControllerMode->render();
}

void midiControllerRender(MidiControllerInterface& mode)
{
    midiControllerMode = &mode;
    midiControllerRender();
}

uint8_t ccpct = 10;
void midiControllerCcPercentage(uint8_t value)
{
    uint8_t pad = value / 3;
    for (uint8_t i = 0; i < padMatrixLen; i++) {
        uint8_t mode = padMatrixMode::On10pct;
        if (i < pad) {
            mode = padMatrixMode::On100pct;
        } else if (i == pad) {
            uint8_t lastPad = ccpct - (pad * 3);
            switch (lastPad) {
            case 0:
                mode = padMatrixMode::On25pct;
                break;
            case 1:
                mode = padMatrixMode::On65pct;
                break;
            case 2:
                mode = padMatrixMode::On100pct;
                break;
            }
        }
        sendPadMatrix(i, padMatrixColor::Cyan, mode);
    }
}

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        // TODO support all row of matrix
        if (message->at(1) <= 0x27) {
            midiControllerMode->noteOnMatrix(message->at(1));
        } else if (message->at(1) == pad::ClipStop) {
            // here we should select the selector...
            // and then
            midiControllerRender(MidiControllerSampleSelector::get());
        } else {
            printf("midi note on: %d\n", message->at(1));
        }
    } else if (message->at(0) == 0x80) {
        if (message->at(1) == pad::ClipStop) {
            midiControllerRender(MidiControllerGrainStart::get());
        } else {
            printf("midi note off: %d\n", message->at(1));
        }
    } else if (message->at(0) == 0xB0) {
        // printf("midi controller cc: %d %d\n", message->at(1), message->at(2));
        int8_t dir = message->at(2) < 64 ? message->at(2) : -(128 - message->at(2));
        ccpct = range((int)ccpct + dir, 0, 120);
        printf("ccpct: %d dir: %d msg: %d\n", ccpct, dir, message->at(2));
        midiControllerCcPercentage(ccpct);
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