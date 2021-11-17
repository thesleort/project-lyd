#include "dtmfEncoder.h"
#include <cmath>
#include <cassert>
#include <chrono>
#include <iostream>

DtmfEncoder::DtmfEncoder(int mSec, int sampleRate, int amplitude)
{
    _sound.setBuffer(_buffer);
    _mSec = mSec;
    _sampleRate = sampleRate;
    _amplitude = amplitude;
}

int DtmfEncoder::getStatus()
{
    // Stopped, paused, playing
    return _sound.getStatus();
}

void DtmfEncoder::playDualTone(int freq1, int freq2)
{
    double incrementX = ((double)freq1/_sampleRate) * _2PI;
    double incrementY = ((double)freq2/_sampleRate) * _2PI;

    int nSamples = (_sampleRate/1000) * _mSec;
    Int16 samples[nSamples];

    double x = 0, y = 0;


    //Create trapez curve for sinus wave, with rise starting at 5%, and decrease starting at 95%
    //It is both decreasing and increasing at 5% to 90% effectiely staying at a value of 1
    double trapezPercent = 10.0;

    double trapezAmpCurve = 0;
    double trapezAmpIncrement = 1.0/((double(nSamples)/100.0)*trapezPercent);


    for (int i = 0; i < nSamples; i++) {

        if(i < (nSamples/100.0)*(100.0-trapezPercent)){
        trapezAmpCurve += trapezAmpIncrement;
        }

        if(i > (nSamples/100.0)*trapezPercent){
        trapezAmpCurve -= trapezAmpIncrement;
        }

        samples[i] = _amplitude * (sin(x) + sin(y));
        x += incrementX;
        y += incrementY;

    }

    _buffer.loadFromSamples(samples, nSamples, 1, _sampleRate);
    _sound.play();
    // while(_sound.getStatus() == 2){}
}

void DtmfEncoder::playDtmfTone(int tone)
{
    assert(tone >= 0 && tone < 16);

    int lowTone = tone / 4;
    int highTone = tone % 4;
    playDualTone(_lowFreqs[lowTone], _highFreqs[highTone]);
}
