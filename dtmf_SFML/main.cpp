#include <iostream>
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

void dumpDataToFile(vector<int>& data, string path, string fileName)
{
    ofstream myfile;
    myfile.open (path + "/" + fileName + ".txt", std::ofstream::trunc);
    for (unsigned long i = 0; i < data.size(); i++){
        myfile << to_string(data.at(i)) << " ";
    }
    myfile.close();
}
int main()
{
    SoundBufferRecorder rec;
    cout << "record start" << endl;
    sf::Clock clock;
    rec.start();
    sleep(1);
    rec.stop();
    cout << "recording: " << clock.getElapsedTime().asMilliseconds() << endl;
    cout << "record stop" << endl;
    const sf::SoundBuffer& buffer = rec.getBuffer();
    const sf::Int16* samplePtr = buffer.getSamples();
    int count = buffer.getSampleCount();

    vector<int> samples;
    for (int i = 0; i < count; i += 10){
        samples.push_back(*(samplePtr + i));
    }

    dumpDataToFile(samples, "/media/sf_VirBox_Shared", "prefilter");

    vector<double> a1 = {0, -0.797774, 0.0696111};
    vector<double> b1 = {-1.86715, 0.881112};
    DigitalFilter::filterCoefficients filter1(a1, b1);

    vector<double> a2 = {0, 0.110067, -0.026648};
    vector<double> b2 = {-1.65785, 0.772433};
    DigitalFilter::filterCoefficients filter2(a2, b2);
    DigitalFilter dFilter(vector<DigitalFilter::filterCoefficients>{filter1, filter2});

    clock.restart();
    vector<int> filteredSamples = dFilter.simParallel(samples);
    cout << "elapsed: " << clock.getElapsedTime().asMilliseconds() << endl;

    dumpDataToFile(filteredSamples, "/media/sf_VirBox_Shared", "postfilter");

    return 0;
}
