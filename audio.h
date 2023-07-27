#ifndef _AUDIO_H_
#define _AUDIO_H_

class AudioApi {
public:
    virtual int open() = 0;
    virtual void list() = 0;
};

#endif