#include "dtmfEncoder.h"
#include <cmath>
#include <cassert>
#include <chrono>
#include <iostream>

void DtmfEncoder::setupDTMF()
{

    for(int p = 0; p < 4; p++){
        for(int u = 0; u < 4; u++){


            double freq2 = _highFreqs[u];
            double freq1 = _lowFreqs[p];



            double incrementX = ((double)freq1/_sampleRate) * 2*M_PI;
            double incrementY = ((double)freq2/_sampleRate) * 2*M_PI;

            int nSamples = ((_sampleRate/1000.0) * (_mSec-_mSec/15));

            Int16 samples[nSamples];

            double x = 0, y = 0;


            //Create trapez curve for sinus wave, with rise starting at 5%, and decrease starting at 95%
            //It is both decreasing and increasing at 5% to 90% effectiely staying at a value of 1
            double trapezPercent = 2.0;

            double trapezAmpCurve = 0;
            double trapezAmpIncrement = 1.0/((double(nSamples)/100.0)*trapezPercent);


            for (int i = 0; i < nSamples; i++) {

                if(i < (nSamples/100.0)*(100.0-trapezPercent)){
                    trapezAmpCurve += trapezAmpIncrement;
                }


                if(i > (nSamples/100.0)*trapezPercent){
                    trapezAmpCurve -= trapezAmpIncrement;
                }


                samples[i] = trapezAmpCurve*_amplitude * (sin(x) + sin(y));


                x += incrementX;
                y += incrementY;

            }

            _buffer = sf::SoundBuffer();
            _buffer.loadFromSamples(samples, nSamples, 1, _sampleRate);

            _bufferVector.push_back(_buffer);

            //_soundVector.at(u+p*4).setBuffer(_buffer);
        }
    }




}

DtmfEncoder::DtmfEncoder(int mSec, int sampleRate, int amplitude)
{
    _sound.setBuffer(_buffer);
    _mSec = mSec;
    _sampleRate = sampleRate;
    _amplitude = amplitude;

    _soundVector = vector<sf::Sound>(16);


    //Instantiate all dtmf tones
    setupDTMF();

}

int DtmfEncoder::getStatus()
{
    // Stopped, paused, playing
    return _sound.getStatus();
}

void DtmfEncoder::playDualTone(int freq1, int freq2)
{


    //_clock.restart();

    _sound.play();

    while(_clock.getElapsedTime().asMilliseconds() < _mSec){

    }
    _sound.stop();

    //cout << "Sound uptime: " << _clock.getElapsedTime().asMilliseconds() << endl;

}

void DtmfEncoder::playDtmfTone(int tone)               
{
    assert(tone >= 0 && tone < 17);

//    int lowTone = tone / 4;
//    int highTone = tone % 4;
//    playDualTone(_lowFreqs[lowTone], _highFreqs[highTone]);



    while(_sound.getStatus() == 2){}
    _sound.setBuffer(_bufferVector[tone]);

    //_clock.restart();
    _sound.play();




    //cout << "Sound uptime: " << _clock.getElapsedTime().asMilliseconds() << endl;

}
