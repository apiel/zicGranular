#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
#include "fs.h"

#define CONFIG_FILE "./config.cfg"

bool loadConfig()
{

    // loop through lines
    // for each line, split by '='
    // if first part matches a config variable, set it


    FILE* file = fopen(CONFIG_FILE, "r"); /* should check the result */
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
        printf("%s", line); 
    }
    fclose(file);
        

    return true;
}

#endif