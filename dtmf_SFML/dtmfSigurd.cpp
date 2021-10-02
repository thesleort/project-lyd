#include "dtmf.h"
#include <cmath>

Dtmf::Dtmf(int mSec, int sampleRate, int amplitude)
{
    _mSec = mSec;
    _sampleRate = sampleRate;
    _amplitude = amplitude;
}

int Dtmf::getStatus()
{
    return _sound.getStatus();
}

void Dtmf::playDualTone(int freq1, int freq2)
{
    double increment = ((double)freq1/_sampleRate) * _TWO_PI;
    double increment2 = ((double)freq2/_sampleRate) * _TWO_PI;

    int nSamples = (_sampleRate/1000) * _mSec;
    Int16 samples[nSamples];

    double x = 0, y = 0;
    for (int i = 0; i < nSamples; i++) {
        samples[i] = _amplitude * (sin(x) + sin(y));
        x += increment;
        y += increment2;
    }
    _buffer.loadFromSamples(samples, nSamples, 1, _sampleRate);
    _sound.setBuffer(_buffer);
    _sound.play();
    while(_sound.getStatus() == 2){}
}

void Dtmf::playDtmfTone(int tone1, int tone2)
{
    playDualTone(_lowFreqs[tone1], _highFreqs[tone2]);
}
