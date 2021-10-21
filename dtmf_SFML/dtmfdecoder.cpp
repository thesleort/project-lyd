#include "dtmfdecoder.h"
#include "dtmfSigurd.h"
#include <complex>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <math.h>
#include <fstream>

#include <unistd.h>

using namespace std;

DtmfDecoder::DtmfDecoder(){

}

int DtmfDecoder::doSample(int msDuration)
{

    cout << "Speed sampling" << endl;
    _msDuration = msDuration;

    //    _cutoffLow = 600*msDuration/1000;

    //    _cutoffHigh = 1800*msDuration/1000;

    //    _FrequencyShift = 1100*msDuration/1000;

    _cutoffLow = 0;

    _cutoffHigh = 15;

    _FrequencyShift = 8;



    vector<complex<double>> data = recordData(msDuration);
    vector<complex<double>> dataFreq = fft(data);
    vector<double> amps = modulus(dataFreq);
    //dumpDataToFile(amps, "/media/sf_VirBox_Shared", "amps");


    if(detectDTMF(amps) == true){
        cout << "entering full sound sampling" << endl;
            _msDuration = 500;
            msDuration = 500;

            _cutoffLow = 600*msDuration/1000;

            _cutoffHigh = 1800*msDuration/1000;

            _FrequencyShift = 1100*msDuration/1000;


        vector<complex<double>> data = recordData(msDuration);
        vector<complex<double>> dataFreq = fft(data);
        vector<double> amps = modulus(dataFreq);
        return splitHighestPeak(amps);
    }else{
        return -1;
    }


}

vector<complex<double>> DtmfDecoder::recordData(int msDuration)
{
    sf::Clock clock;
    _recorder.start();

    while(clock.getElapsedTime().asMilliseconds() < msDuration){}
    //usleep(1000000);
    _recorder.stop();

    const sf::SoundBuffer& buffer = _recorder.getBuffer();

    const sf::Int16* samples = buffer.getSamples();
    const long count = buffer.getSampleCount();
    vector<complex<double>> data;

    // samples bliver 2 opløftet til et tal (fft funktion virker ved at spiltte data i 2 rekursivt)
    //int pow2Count = pow(2, round(log2(count)));

    for (int i = 0; i < count; i++){
        complex<double> number;
        number.real((double)samples[i]);
        data.push_back(number);
    }

    return data;
}

vector<complex<double>> DtmfDecoder::fft(vector<complex<double> > &a)
{
    // https://www.geeksforgeeks.org/fast-fourier-transformation-poynomial-multiplication/

    using cd = complex<double>;
    int n = a.size();

    // if input contains just one element
    if (n == 1)
        return vector<cd>(1, a[0]);

    // For storing n complex nth roots of unity
    vector<cd> w(n);
    for (int i = 0; i < n; i++) {
        double alpha = -2 * M_PI * i / n;
        w[i] = cd(cos(alpha), sin(alpha));
    }

    vector<cd> A0(n / 2), A1(n / 2);
    for (int i = 0; i < n / 2; i++) {

        // even indexed coefficients
        A0[i] = a[i * 2];

        // odd indexed coefficients
        A1[i] = a[i * 2 + 1];
    }

    // Recursive call for even indexed coefficients
    vector<cd> y0 = fft(A0);

    // Recursive call for odd indexed coefficients
    vector<cd> y1 = fft(A1);

    // for storing values of y0, y1, y2, ..., yn-1.
    vector<cd> y(n);

    for (int k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w[k] * y1[k];
        y[k + n / 2] = y0[k] - w[k] * y1[k];
    }
    return y;
}

vector<double> DtmfDecoder::modulus(const vector<complex<double>>& data)
{
    vector<double> ampVector;

    for (int i = _cutoffLow; i < _cutoffHigh; i++){
        double real = data.at(i).real();
        double imag = data.at(i).imag();
        double amp = sqrt(real*real + imag*imag);
        ampVector.push_back(amp);
    }

        ofstream myfile2;
        myfile2.open ("/home/peter/Desktop/ProjektLyd.txt", std::ofstream::trunc);
        for (int i = 0; i < ampVector.size(); i++){
            myfile2 << to_string(ampVector.at(i)) << " ";
        }
        myfile2.close();

    return ampVector;
}

int DtmfDecoder::splitHighestPeak(const vector<double> &data)
{

    double DistanceLow;
    double DistanceHigh;

    //Cut off 600 Hz, CutOff high 1800 Derfor er 450

    int frequencyShift = _FrequencyShift-_cutoffLow;

    //Decoding Low frequency
    auto it = find(data.begin(), data.begin()+frequencyShift, *max_element(data.begin(), data.begin()+frequencyShift));

    int index = (it - data.begin()+_cutoffLow)*1000/_msDuration;

    vector<double> sampletesting;

    for(int i = 0; i < 4; i++){
        sampletesting.push_back(abs(_lowFreqs[i]-(index)));
    }

    cout << "low frequency: " << index << endl;

    auto lowF = find(sampletesting.begin(), sampletesting.end(), *min_element(sampletesting.begin(), sampletesting.end()));

    int lowIndex = lowF - sampletesting.begin();

     DistanceLow = *min_element(sampletesting.begin(), sampletesting.end());


    //Decoding high frequency
    it = find(data.begin()+frequencyShift, data.end(), *max_element(data.begin()+frequencyShift, data.end()));

    index = (it - data.begin()+_cutoffLow)*1000/_msDuration;

    cout << "high frequency: " << index << endl;
    cout << "___________________________________________" << endl;

    sampletesting.clear();

    for(int i = 0; i < 4; i++){
        sampletesting.push_back(abs(_highFreqs[i]-(index)));
    }

    auto HighF = find(sampletesting.begin(), sampletesting.end(), *min_element(sampletesting.begin(), sampletesting.end()));

    int HighIndex = HighF - sampletesting.begin();

    DistanceHigh = *min_element(sampletesting.begin(), sampletesting.end());

    vector<double> Distances;

    return lowIndex*4+HighIndex;
}

bool DtmfDecoder::detectDTMF(const vector<double> &data){
    double DistanceLow;
    double DistanceHigh;



    //Decoding Low frequency
    auto it = find(data.begin(), data.end(), *max_element(data.begin(), data.end()));

    int index = it - data.begin();

    if(data.at(index) > 10000){
        if((index == 6 || index == 7 || index == 8)){
         dtmf_Detection++;
         if(dtmf_Detection >= 3){
             dtmf_Detection = 0;
             return true;
         }
        }else{
            dtmf_Detection = 0;
            return false;
        }
    }



}

void DtmfDecoder::dumpDataToFile(vector<double>& data, string path, string fileName)
{
    ofstream myfile;
    myfile.open (path + "/" + fileName + ".txt", std::ofstream::trunc);
    for (int i = 0; i < data.size(); i++){
        myfile << to_string(data.at(i)) << " ";
    }
    myfile.close();
}
