#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
#include "fs.h"
#include "midi.h"

#define CONFIG_FILE "./config.cfg"

char * trimChar(char* str, char c = '\n')
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

void assignKeyValue(char* key, char* value)
{
    // if key = MIDIIN
    if (strcmp(key, "MIDIIN") == 0) {
        loadMidiInput(midiController, trimChar(value), &midiControllerCallback);
    } else {
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