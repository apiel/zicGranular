#ifndef _MIDI_CONTROLLER_DEF_H_
#define _MIDI_CONTROLLER_DEF_H_

#include "def.h"

#define PAD_MATRIX_COLS 8
#define PAD_MATRIX_ROWS 5

uint8_t padMatrix[PAD_MATRIX_ROWS][PAD_MATRIX_COLS] = {
    { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27 },
    { 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f },
    { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 },
    { 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 },
};

const uint8_t padMatrixLen = PAD_MATRIX_ROWS * PAD_MATRIX_COLS;
// clang-format off
uint8_t padMatrixFlat[padMatrixLen] = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 ,0x27,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d ,0x1e, 0x1f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15 ,0x16, 0x17,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d ,0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05 ,0x06, 0x07,
};
// clang-format on

enum padMatrixMode {
    Off = 0x00,
    On10pct = 0x90,
    On25pct = 0x91,
    On50pct = 0x92,
    On65pct = 0x93,
    On75pct = 0x94,
    On90pct = 0x95,
    On100pct = 0x96,
    Pulsing1_16 = 0x97,
    Pulsing1_8 = 0x98,
    Pulsing1_4 = 0x99,
    Pulsing1_2 = 0x9a,
    Blinking1_24 = 0x9b,
    Blinking1_16 = 0x9c,
    Blinking1_8 = 0x9d,
    Blinking1_4 = 0x9e,
    Blinking1_2 = 0x9f,
};

enum padMatrixColor {
    Blue = 78,
    Pink = 95,
    Yellow = 96,
    Green = 73,
    Orange = 84,
    Red = 5,
    Purple = 50,
    Cyan = 102,
    PastelPink = 4,
    LightBlue = 32,
};


enum pad {
    Up = 0x40,
    Down = 0x41,
    Left = 0x42,
    Right = 0x43,
    Volume = 0x44,
    Pan = 0x45,
    Send = 0x46,
    Device = 0x47,
    ClipStop = 0x52,
    Solo = 0x53,
    Mute = 0x54,
    RecArm = 0x55,
    Select = 0x56,
    StopAllClips = 0x51,
    Play = 0x5b,
    Record = 0x5d,
    Shift = 0x62,
};

enum padMode {
    _Off = 0x00,
    _On = 0x01,
    _Blink = 0x02,
};

enum encoder {
    k1 = 0x30,
    k2 = 0x31,
    k3 = 0x32,
    k4 = 0x33,
    k5 = 0x34,
    k6 = 0x35,
    k7 = 0x36,
    k8 = 0x37,
};

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

#endif