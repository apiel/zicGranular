#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include "def.h"
#include <dirent.h>

#define FILE_BROWSER_FILES_MAX 40
#define FILE_BROWSER_FILEPATH_LEN 258
#define FILE_BROWSER_FILENAME_LEN 255

class FileBrowser {
protected:
    const char* folder;
    char file[FILE_BROWSER_FILEPATH_LEN];
    char files[FILE_BROWSER_FILES_MAX][FILE_BROWSER_FILENAME_LEN];

    void init()
    {
        DIR* dir = opendir(folder);
        if (dir != NULL) {
            struct dirent* directory;
            uint8_t i = 0;
            while ((directory = readdir(dir)) != NULL && i < FILE_BROWSER_FILES_MAX) {
                if (strcmp(directory->d_name, ".") != 0 && strcmp(directory->d_name, "..") != 0) {
                    strncpy(files[i], directory->d_name, FILE_BROWSER_FILENAME_LEN);
                    i++;
                }
            }
            closedir(dir);
        }
    }

    void sort()
    {
        uint8_t i, j;
        char tmp[FILE_BROWSER_FILENAME_LEN];
        for (i = 0; i < FILE_BROWSER_FILES_MAX - 1; i++) {
            for (j = i + 1; j < FILE_BROWSER_FILES_MAX; j++) {
                if (strcmp(files[i], files[j]) > 0) {
                    strncpy(tmp, files[i], FILE_BROWSER_FILENAME_LEN);
                    strncpy(files[i], files[j], FILE_BROWSER_FILENAME_LEN);
                    strncpy(files[j], tmp, FILE_BROWSER_FILENAME_LEN);
                }
            }
        }
    }

    // void dump()
    // {
    //     for (uint8_t i = 0; i < FILE_BROWSER_FILES_MAX; i++) {
    //         printf("%d. %s\n", i, files[i]);
    //     }
    // }

public:
    uint8_t position = 0;

    FileBrowser(const char* _folder)
        : folder(_folder)
    {
        init();
        sort();

        getFile(position);
    }

    char* getFile(uint8_t pos)
    {
        position = range(pos, 0, FILE_BROWSER_FILES_MAX - 1);
        snprintf(file, FILE_BROWSER_FILEPATH_LEN, "%s/%s", folder, files[position]);
        return file;
    }
};

#endif