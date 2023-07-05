#ifndef _MIDI_CONTROLLER_SAMPLE_SELECTOR_H_
#define _MIDI_CONTROLLER_SAMPLE_SELECTOR_H_

#include "audioHandler.h"
#include "def.h"
#include "fileBrowser.h"
#include "midiControllerDef.h"
#include "midiControllerInterface.h"

class MidiControllerSampleSelector : public MidiControllerInterface {
protected:
    static MidiControllerSampleSelector* instance;

    MidiControllerSampleSelector()
    {
    }

public:
    static MidiControllerSampleSelector& get()
    {
        if (!instance) {
            instance = new MidiControllerSampleSelector();
        }
        return *instance;
    }

    void render()
    {
        printf("MidiControllerSampleSelector::render\n");
        FileBrowser& fileBrowser = AudioHandler::get().fileBrowser;
        for (uint8_t i = 0; i < padMatrixLen; i++) {
            sendPadMatrix(padMatrixFlat[i], padMatrixColor::Blue,
                fileBrowser.position == i ? padMatrixMode::On100pct : padMatrixMode::On10pct);
        }
    }

    void noteOnMatrix(uint8_t index)
    {
        char* filepath = AudioHandler::get().fileBrowser.getFile(index);
        AudioHandler::get().audioGranular.open(filepath);
        render();
    }
};

MidiControllerSampleSelector* MidiControllerSampleSelector::instance = NULL;

#endif