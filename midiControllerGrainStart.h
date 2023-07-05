#ifndef _MIDI_CONTROLLER_GRAIN_START_H_
#define _MIDI_CONTROLLER_GRAIN_START_H_

#include "audioGranular.h"
#include "audioHandler.h"
#include "def.h"
#include "midiControllerDef.h"
#include "midiControllerInterface.h"

class MidiControllerGrainStart : public MidiControllerInterface {
protected:
    static MidiControllerGrainStart* instance;

    MidiControllerGrainStart()
    {
    }

public:
    static MidiControllerGrainStart& get()
    {
        if (!instance) {
            instance = new MidiControllerGrainStart();
        }
        return *instance;
    }

    void render()
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

    void noteOnMatrix(uint8_t index)
    {
        AudioHandler::get().audioGranular.toggleStart(index);
        render();
    }
};

MidiControllerGrainStart* MidiControllerGrainStart::instance = NULL;

#endif