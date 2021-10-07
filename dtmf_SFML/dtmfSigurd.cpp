#include "dtmfSigurd.h"
#include <cmath>
#include <cassert>

DtmfSigurd::DtmfSigurd(int mSec, int sampleRate, int amplitude)
{
    _mSec = mSec;
    _sampleRate = sampleRate;
    _amplitude = amplitude;
}

int DtmfSigurd::getStatus()
{
    // Stopped, paused, playing
    return _sound.getStatus();
}

void DtmfSigurd::playDualTone(int freq1, int freq2)
{
    double increment1 = ((double)freq1/_sampleRate) * _2PI;
    double increment2 = ((double)freq2/_sampleRate) * _2PI;

    int nSamples = (_sampleRate/1000) * _mSec;
    Int16 samples[nSamples];

    double x = 0, y = 0;
    for (int i = 0; i < nSamples; i++) {
        samples[i] = _amplitude * (sin(x) + sin(y));
        x += increment1;
        y += increment2;
    }
    _buffer.loadFromSamples(samples, nSamples, 1, _sampleRate);
    _sound.setBuffer(_buffer);
    _sound.play();
    //while(_sound.getStatus() == 2){} Kommenteret ud for at teste dtmf præcision
}

void DtmfSigurd::playDtmfTone(int tone)
{
    assert(tone >= 0 && tone < 16);

    int lowTone = tone / 4;
    int highTone = tone % 4;
    playDualTone(_lowFreqs[lowTone], _highFreqs[highTone]);
}
