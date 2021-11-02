#include "dtmfdecoder.h"
#include "dtmfEncoder.h"
#include <complex>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <math.h>
#include <fstream>

#include <unistd.h>

using namespace std;

DtmfDecoder::DtmfDecoder(double sampleTime) : _sampleTime(sampleTime/1000.0){

}

int DtmfDecoder::identifyDTMF(const Int16 *data, int count)
{

//    cout << "amount of samples: " << count << endl;

   vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count,20); //Sample is looped 30 times for precision

   vector<double> testPrioriFFT;

   for (int i = 0; i < count; i++){
       testPrioriFFT.push_back(data[i]);
   }

   dumpDataToFile(testPrioriFFT, "/home/peter/Desktop", "PrioriFFT");

   complexSoundBuffer = fft(complexSoundBuffer);

   vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

   vector<double> testPosterior;

   for(int i= 0; i < frequencyData.size(); i++){
       testPosterior.push_back(frequencyData.at(i).amplitude);
   }

   dumpDataToFile(testPosterior, "/home/peter/Desktop", "PosteriorFFT");

   vector<double> peaks = findSignalPeaks(frequencyData);

   return frequencyToDtmf(peaks[0], peaks[1]);

}

bool DtmfDecoder::detectDTMFTone0(const sf::Int16* data, int count)
{

    vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count,20); //Sample is looped n times for precision

    complexSoundBuffer = fft(complexSoundBuffer);

    vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

    vector<double> peaks = findSignalPeaks(frequencyData);

    return isDTMF0(peaks[0], peaks [1]);




}

vector<complex<double>> DtmfDecoder::realToComplexVector(const Int16* reals, int count, int resMultiplier)
{
    vector<complex<double>> complexes;

   for(int p = 0; p < resMultiplier; p++){
       for (int i = 0; i < count; i++){
           complex<double> number;
           number.real((double)reals[i]);
           complexes.push_back(number);
       }
   }

   _sampleTime = _sampleTime*resMultiplier;

    return complexes;
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

vector<DtmfDecoder::signal> DtmfDecoder::sequenceToSignals(const vector<complex<double>> &sequence)
{
    vector<signal> sigs;


    for (int i = _cutoffLow * _sampleTime; i < _cutoffHigh * _sampleTime; i++){
        double real = sequence.at(i).real();
        double imag = sequence.at(i).imag();
        double amp = sqrt(real*real + imag*imag); // possible optimization by not taking square root
        double freq = i / _sampleTime;
        signal sig(amp, freq);
        sigs.push_back(sig);
    }



    return sigs;
}


vector<double> DtmfDecoder::findSignalPeaks(const vector<signal> & signaldata, double error){

    double middleFreq = (_lowFreqs.at(_lowFreqs.size()-1)+_highFreqs.at(0))/2;

    double highestAmp = 0;
    double highestFreq;
    vector<double> peakFreqs;
    vector<int> dtmf_Tone;

    bool searchLowFreq = true;

    for(signal i : signaldata){
        if(i.frequency-middleFreq > 0 && searchLowFreq){
            searchLowFreq = false;

            peakFreqs.push_back(highestFreq);

            highestAmp = 0;
        }

        if(i.amplitude > highestAmp){
            highestAmp = i.amplitude;
            highestFreq = i.frequency;
        }
    }
    peakFreqs.push_back(highestFreq);

    return peakFreqs; // Error decimal

}

int DtmfDecoder::frequencyToDtmf(double lowfreq, double highfreq){

    double lowshortestDistance = abs(_lowFreqs[0]-lowfreq);
    int lowIndex = 0;

    double highshortestDistance = abs(_highFreqs[0]-highfreq);
    int highIndex = 0;

    for(int i = 0; i < 4; i++){
        if(lowshortestDistance > abs(_lowFreqs[i]-lowfreq)){
            lowshortestDistance = abs(_lowFreqs[i]-lowfreq);
            lowIndex = i;
        }

        if(highshortestDistance > abs(_highFreqs[i]-highfreq)){
            highshortestDistance = abs(_highFreqs[i]-highfreq);
            highIndex = i;
        }
    }



    return lowIndex*4+highIndex;
}

bool DtmfDecoder::isDTMF0(double lowfreq, double highfreq)
{
    double errorPercentage = 3.6;

    if((abs(lowfreq-_lowFreqs[0])/_lowFreqs[0])*100.0 > errorPercentage){ //If error is greater than 2.5%
        return false;
    }

    if((abs(highfreq-_highFreqs[0])/_highFreqs[0])*100.0 > errorPercentage){ //If error is greater than 2.5%
        return false;
    }

    return true;


}

void DtmfDecoder::dumpDataToFile(vector<double>& data, string path, string fileName)
{
    ofstream myfile;
    myfile.open (path + "/" + fileName + ".txt", std::ofstream::trunc);
    for (unsigned long i = 0; i < data.size(); i++){
        myfile << to_string(data.at(i)) << " ";
    }
    myfile.close();
}
