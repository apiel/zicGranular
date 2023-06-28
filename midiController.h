#ifndef _MIDI_CONTROLLER_H_
#define _MIDI_CONTROLLER_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"
#include "midiControllerDef.h"

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
    sendPad(pad::Solo, padMode::_Blink);
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

void midiControllerRender()
{
    midiControlerRenderSelector();
    midiControllerRenderGrainStart();
}

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0x90) {
        if (message->at(1) >= 0x20 && message->at(1) <= 0x27) {
            AudioHandler::get().audioGranular.toggleStart(message->at(1) - 0x20);
            midiControllerRenderGrainStart();
        }

        // } else if (message->at(0) == 0x80) {
        //     onMidiNoteOff(message->at(1), message->at(2));
        // } else if (message->at(0) == 0xB0) {
        //     onMidiControlChange(message->at(1), message->at(2));
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