#ifndef _FILTER_H_
#define _FILTER_H_

#include "def.h"

#include <math.h>

// https://www.martin-finke.de/articles/audio-plugins-013-filter/
// https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html

class Filter {
protected:
    enum FilterMode {
        FILTER_MODE_OFF,
        FILTER_MODE_LOWPASS_12,
        FILTER_MODE_HIGHPASS_12,
        FILTER_MODE_COUNT,
    };

    // cutoff cannot be 1.0 else div by zero range(_cutoff, 0.01, 0.99);
    float cutoff = 0.99;
    float feedback;
    float buf0 = 0;
    float buf1 = 0;

    void calculateVar()
    {
        calculateVar(cutoff, resonance);
    }

    void calculateVar(float _cutoff, float _resonance)
    {
        if (_resonance == 0.0f) {
            feedback = 0.0f;
            return;
        }

        // cutoff cannot be 1.0 (should we ensure this?)
        feedback = _resonance + _resonance / (1.0 - _cutoff); // would it make sense to make a lookup table for `1.0 / (1.0 - _cutoff);` ?
    }

    float sample(float inputValue, float _cutoff)
    {
        if (mode == FILTER_MODE_OFF || inputValue == 0) {
            return inputValue;
        }

        buf0 += _cutoff * (inputValue - buf0 + feedback * (buf0 - buf1));
        buf1 += _cutoff * (buf0 - buf1);

        if (mode == FILTER_MODE_LOWPASS_12) {
            return buf1;
        }
        return inputValue - buf0;
    }

public:
    int16_t value = 0;
    float resonance = 0.0;
    uint8_t mode = FILTER_MODE_OFF;

    Filter()
    {
        set(value);
    };

    float sample(float inputValue)
    {
        return sample(inputValue, cutoff);
    }

    Filter& set(int16_t val)
    {
        // Frequency should be under 7350Hz else cutoff is = 1 and then no sound
        // We dont want to have LPF/HPF under 50Hz

        value = range(val, -7250, 7250);
        int16_t frequency = value;
        if (value == 0) {
            mode = FILTER_MODE_OFF;
        } else if (value > 0) {
            mode = FILTER_MODE_HIGHPASS_12;
            frequency = frequency + 50;
        } else {
            mode = FILTER_MODE_LOWPASS_12;
            frequency = 7350 - (frequency * -1 + 40);
        }

        cutoff = 2.0 * sin(M_PI * frequency / SAMPLE_RATE); // lookup table?? js: Array.from(Array(7350).keys()).map(frequency => 2.0 * Math.sin(3.141592653589793238 * frequency / 44100));

        // printf("Filter: %d -> %d cutoff %f\n", value, frequency, cutoff);

        calculateVar();

        debug("Filter: value=%d cutoff=%f\n", value, cutoff);

        return *this;
    }

    /**
     * @brief set filter cutoff
     * 
     * @param value where 0.0 is 7250Hz LPF, 0.5 is no filter, 1.0 is 7250Hz HPF
     * @return Filter& 
     */
    Filter& set(float value) {
        int16_t val = range(value, 0.0, 1.0) * 7250.0 * 2.0 - 7250.0;
        return set(val);
    }

    Filter& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 0.99);
        calculateVar();

        debug("Filter: resonance=%f\n", resonance);

        return *this;
    };
};

#endif
