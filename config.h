#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
#include "fs.h"

#define CONFIG_FILE "./config.cfg"

bool loadConfig()
{
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        printf("Failed to load config file: %s\n", CONFIG_FILE);
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // ignore comments and empty lines
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }
        // split by '='
        char* key = strtok(line, "=");
        char* value = strtok(NULL, "=");
        if (key == NULL || value == NULL) {
            printf("Invalid config line: %s\n", line);
            continue;
        }
        printf("key: %s, value: %s\n", key, value); 
    }
    fclose(file);
        

    return true;
}

#endif