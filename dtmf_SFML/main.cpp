#include <iostream>
#include <SFML/Audio.hpp>
#include <cmath>
#include "dtmfEncoder.h"
#include <vector>
#include <complex>
#include <string>
#include <fstream>
#include "dtmfdecoder.h"

#include <unistd.h>

int main()
{
    //DtmfSigurd generator(1000, 44100, 3000);
    //generator.playDtmfTone(0);
    //DtmfDecoder decoder;
    //cout << decoder.doSample(1000) << endl;
    //while(generator.getStatus() == 2);

    SoundBufferRecorder recorder;
    sf::Clock clock;
    recorder.start();
    sleep(1);

    DtmfEncoder generator(1000, 44100, 3000);
    generator.playDtmfTone(9);
    while(generator.getStatus() == 2){};

    sleep(2);
    recorder.stop();

    const sf::SoundBuffer& buffer = recorder.getBuffer();

    const sf::Int16* samples = buffer.getSamples();
    const long count = buffer.getSampleCount();

    ofstream myfile;
    myfile.open ("/media/sf_VirBox_Shared/raw.txt", std::ofstream::trunc);
    for (long i = 0; i < count; i++){
        myfile << to_string(samples[i]) << " ";
    }
    myfile.close();

    double mean = 0;
    for(int i = 0; i < count; i++){
        mean += abs((double)samples[i]) / (double)count;
    }
    cout << "mean: " << mean << endl;

    double average = 0;

    double lastAverage;

    double koef = 0.00005;

    // remove short loud sounds by removing data that is some % above mean.

    vector<double> avgVec;
    for (int i = 0; i < count; i++){
        lastAverage = average;
        average = abs(samples[i]) * koef + (1.0f-koef) * abs(lastAverage);
        if (abs(samples[i]) > mean*5){
            average = avgVec.at(i-1);
        }
        avgVec.push_back(average);
    }

    ofstream myfileOther;
    myfile.open ("/media/sf_VirBox_Shared/avg.txt", std::ofstream::trunc);
    for (unsigned long i = 0; i < avgVec.size(); i++){
        myfile << to_string(avgVec.at(i)) << " ";
    }
    myfile.close();

    return 0;
}
