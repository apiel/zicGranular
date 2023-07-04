#ifndef _MIDI_CONTROLLER_H_
#define _MIDI_CONTROLLER_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"
#include "fileBrowser.h"
#include "midiControllerDef.h"

enum MidiControllerMode {
    GRAIN_START_SELECTOR,
    SAMPLE_SELCETOR,
};

uint8_t midiControllerMode = MidiControllerMode::GRAIN_START_SELECTOR;

void sendPadMatrix(int pad, int color, int mode)
{
    std::vector<unsigned char> message;
    message.push_back(mode);
    message.push_back(pad);
    message.push_back(color);
    midiControllerOut.sendMessage(&message);
}

void sendPad(int pad, int mode)
{
    std::vector<unsigned char> message;
    message.push_back(0x90);
    message.push_back(pad);
    message.push_back(mode);
    midiControllerOut.sendMessage(&message);
}

void midiControlerRenderSelector()
{
    sendPad(pad::ClipStop, padMode::_On);
    // sendPad(pad::Solo, padMode::_Blink);
}

void midiControllerRenderGrainStart()
{
    AudioGranular& audioGranular = AudioHandler::get().audioGranular;
    for (int i = 0; i < START_POINTS; i++) {
        sendPadMatrix(padMatrix[0][0] + i, padMatrixColor::Blue, audioGranular.starts[i].active ? padMatrixMode::On100pct : padMatrixMode::On10pct);
    }

    // Temporary
    for (int i = 0; i < START_POINTS; i++) {
        sendPadMatrix(padMatrix[1][0] + i, padMatrixColor::Pink, padMatrixMode::On10pct);
        sendPadMatrix(padMatrix[2][0] + i, padMatrixColor::Yellow, padMatrixMode::On10pct);
        sendPadMatrix(padMatrix[3][0] + i, padMatrixColor::Green, padMatrixMode::On10pct);
        sendPadMatrix(padMatrix[4][0] + i, padMatrixColor::Orange, padMatrixMode::On10pct);
    }
}

void midiControllerSampleSelector()
{
    FileBrowser& fileBrowser = AudioHandler::get().fileBrowser;
    for (uint8_t i = 0; i < padMatrixLen; i++) {
        sendPadMatrix(padMatrixFlat[i], padMatrixColor::Blue,
            fileBrowser.position == i ? padMatrixMode::On100pct : padMatrixMode::On10pct);
    }
}

void midiControllerRender()
{
    midiControlerRenderSelector();
    switch (midiControllerMode) {
    case MidiControllerMode::GRAIN_START_SELECTOR:
        midiControllerRenderGrainStart();
        break;
    case MidiControllerMode::SAMPLE_SELCETOR:
        midiControllerSampleSelector();
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
            switch (midiControllerMode) {
            case MidiControllerMode::GRAIN_START_SELECTOR:
                AudioHandler::get().audioGranular.toggleStart(message->at(1) - 0x20);
                midiControllerRenderGrainStart();
                break;
            case MidiControllerMode::SAMPLE_SELCETOR: {
                char * filepath = AudioHandler::get().fileBrowser.getFile(message->at(1) - 0x20);
                AudioHandler::get().audioGranular.open(filepath);
                midiControllerSampleSelector();
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