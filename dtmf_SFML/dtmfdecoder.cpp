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


//Normal sampling: 50 ms, 10 Repeat padding, downsample factor 10, amp threshold 5000, Errormargin = 2

//Speed sampling 10 ms, 60 repeat padding, downsample factor 10, amp threshold 5000, ErrorMargin = 4

int DtmfDecoder::identifyDTMF(const Int16 *data, int count)
{

//    cout << "amount of samples: " << count << endl;

   vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count); //Sample is looped 30 times for precision

//   vector<double> testPrioriFFT;

//   for (int i = 0; i < count; i++){
//       testPrioriFFT.push_back(data[i]);
//   }

//   dumpDataToFile(testPrioriFFT, "/home/peter/Desktop", "PrioriFFT");
   complexSoundBuffer = fft(complexSoundBuffer);

   vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

   vector<double> testPosterior;

//   for(int i= 0; i < frequencyData.size(); i++){
//       testPosterior.push_back(frequencyData.at(i).amplitude);
//   }

//   dumpDataToFile(testPosterior, "/media/sf_Ubuntu_Shared_Folder", "PosteriorFFT");


   vector<double> peaks = findSignalPeaks(frequencyData);


   int tone;

   if(peaks[0] != -1){
   tone = frequencyToDtmf(peaks[0], peaks[1]);
    }else{
       return -1;
   }

   return tone;

}

bool DtmfDecoder::detectDTMFTone0(const sf::Int16* data, int count)
{

    vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count);

    complexSoundBuffer = fft(complexSoundBuffer);

    vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

    vector<double> peaks = findSignalPeaks(frequencyData);

    //return isDTMF_N(peaks[0], peaks [1], );

}

vector<complex<double>> DtmfDecoder::realToComplexVector(const Int16* reals, int count)
{
    vector<complex<double>> complexes;

   for(int p = 0; p < _repeatPadding; p++){
       for (int i = 0; i < count; i += _downSampling){
           complex<double> number;
           number.real((double)reals[i]);
           complexes.push_back(number);
       }
   }


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


    for (int i = _cutoffLow * _sampleTime*_repeatPadding; i < _cutoffHigh * _sampleTime*_repeatPadding; i++){
        double real = sequence.at(i).real();
        double imag = sequence.at(i).imag();
        double amp = sqrt(real*real + imag*imag); // possible optimization by not taking square root
        double freq = i / (_sampleTime*_repeatPadding);
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
    vector<double> peakAmp;
    vector<int> dtmf_Tone;

    bool searchLowFreq = true;

    for(signal i : signaldata){
        if(i.frequency-middleFreq > 0 && searchLowFreq){
            searchLowFreq = false;

            peakFreqs.push_back(highestFreq);
            peakAmp.push_back(highestAmp);

            highestAmp = 0;
        }

        if(i.amplitude > highestAmp){
            highestAmp = i.amplitude;
            highestFreq = i.frequency;
        }
    }
    peakFreqs.push_back(highestFreq);
    peakAmp.push_back(highestAmp);

    //Hurtig fix til amp begrænsning

    if(peakAmp.at(0) < 5000 || peakAmp.at(1) < 5000){
        //cout << "Low Amp Block" << endl;
        peakFreqs[0] = -1;
    }

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

    //isDTMF_N(lowfreq,highfreq,lowIndex*4+highIndex)
    if(isDTMF_N(lowfreq,highfreq,lowIndex,highIndex)){
        return lowIndex*4+highIndex;
    }else{
        return -1;
    }
}

bool DtmfDecoder::isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex)
{
    double errorPercentage = _errorMargin;

    //cout << "ERROR Percentage: " << (lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex]*100.0 << endl;

    if((abs(lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex])*100.0 > errorPercentage){ //If error is greater than 2.5%
        return false;
    }

    if((abs(highfreq-_highFreqs[highFreqIndex])/_highFreqs[highFreqIndex])*100.0 > errorPercentage){ //If error is greater than 2.5%
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


