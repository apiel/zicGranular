#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
#include "fs.h"
#include "midi.h"

#define CONFIG_FILE "./config.cfg"

char* trimChar(char* str, char c = '\n')
{
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == c) {
            str[i] = '\0';
            break;
        }
    }
    return str;
}

void assignMidiMapping(MidiMapping& map, char* value)
{
    // split value by space
    char* msg1 = strtok(value, " ");
    char* msg2 = strtok(NULL, " ");
    char* msg3 = strtok(NULL, " ");

    if (msg1 == NULL || msg2 == NULL) {
        printf("Invalid midi mapping\n");
        return;
    }

    map.size = msg3 == NULL ? 2 : 3;
    map.valuePosition = msg2[0] == 'x' && msg2[1] == 'x' ? 2 : 3;
    map.msg[0] = (uint8_t)strtol(msg1, NULL, 16);
    map.msg[1] = (uint8_t)strtol(msg2, NULL, 16);

    printf("Midi mapping: %02x %02x, size: %d valuePosition: %d\n", map.msg[0], map.msg[1], map.size, map.valuePosition);
}

void assignKeyValue(char* key, char* value)
{
    if (strcmp(key, "MIDIIN") == 0) {
        loadMidiInput(midiController, trimChar(value), &midiControllerCallback);
    } else if (strcmp(key, "AUDIO_OUTPUT") == 0) {
        strcpy(audioOutput, trimChar(value));
        printf("Audio output set: %s\n", audioOutput);
    } else if (strcmp(key, "GAIN_OUTPUT") == 0) {
        setMasterVolume(masterVolume, atof(value));
        printf("Gain output set: %f\n", gainOutput);
    } else if (strcmp(key, "SEQUENCER_CHANNEL") == 0) {
        midiSequencerChannel = atoi(value);
        printf("Midi sequencer channell set: %d\n", midiSequencerChannel);
    } else {
        for (int i = 0; i < MIDI_MAPS; i++) {
            if (strcmp(key, midiMappings[i].key) == 0) {
                assignMidiMapping(midiMappings[i], value);
                return;
            }
        }
        printf("unknown config key: %s\n", key);
    }
}

void parseConfigLine(char* line)
{
    // ignore comments and empty lines
    if (line[0] == '#' || line[0] == '\n') {
        return;
    }
    // split by '='
    char* key = strtok(line, "=");
    char* value = strtok(NULL, "=");
    if (key == NULL || value == NULL) {
        printf("Invalid config line: %s\n", line);
        return;
    }
    assignKeyValue(key, value);
}

bool loadConfig()
{
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        printf("Failed to load config file: %s\n", CONFIG_FILE);
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        parseConfigLine(line);
    }
    fclose(file);

    return true;
}

#endif