﻿#include <iostream>
#include <SFML/Audio.hpp>
#include <cmath>
#include "dtmfEncoder.h"
#include <vector>
#include <complex>
#include <string>
#include <fstream>
#include "dtmfdecoder.h"

#include "physicallayer.h"

#include "digitalfilter.h"

#include <unistd.h>


int main()
{


//    for(int i = 0; i< 1000; i++){
//    SoundBufferRecorder recorder;

//    sf::Clock clock;
//    double sampletime = 100.0;
//    usleep(100);
//    recorder.start();

//    while(clock.getElapsedTime().asMilliseconds() <=  sampletime){}
//    recorder.stop();

//    const sf::SoundBuffer& buffer = recorder.getBuffer();

//    const sf::Int16* samples = buffer.getSamples();

//    const long count = buffer.getSampleCount();

//    double fractionSampleTime = 10.0;

//    double timefraction = sampletime/fractionSampleTime;



//       DtmfDecoder testSampling((count/(44100.0*timefraction))*1000.0);
//       int tone = testSampling.identifyDTMF(samples, count/timefraction);

//           cout << tone << endl;

//    }


    PhysicalLayer soundObj(70,80,20);
    vector<int> mega{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        for (int i = 0; i < mega.size(); i++){

            soundObj.writeOutBuffer(mega.at(i));
        }

        while(true){

        }

    return 0;
}