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


DtmfDecoder::DtmfDecoder(double sampleTime) : _sampleTime(sampleTime/1000.0)
{

    _digitalFilter = DigitalFilter();

    vector<double> a1 = {0, -0.441023, -0.0717413};
    vector<double> b1 = {-0.881973, 0.503572};
    filterCoefficients filter1(a1, b1);

    vector<double> a2 = {0, 0.728023, 0.600765};
    vector<double> b2 = {0.394373, 0.216454};
    filterCoefficients filter2(a2, b2);

    _digitalFilter = DigitalFilter(vector<filterCoefficients>{filter1, filter2});

    fftin = fftw_alloc_complex(SAMPLERATE/_downSampling);
    fftout = fftw_alloc_complex(SAMPLERATE/_downSampling);



}


//Normal sampling: 50 ms, 10 Repeat padding, downsample factor 10, amp threshold 5000, Errormargin = 2

//Speed sampling 10 ms, 60 repeat padding, downsample factor 10, amp threshold 5000, ErrorMargin = 4

int DtmfDecoder::identifyDTMF(const Int16 *data, int count, double sampletime)
{
    _sampleTime = sampletime/1000;
//    cout << "amount of samples: " << count << endl;



   //Vil give fejl indtil filteret er lavet.
   vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count); //Sample is looped 30 times for precision

   complexSoundBuffer = _digitalFilter.simParallel(complexSoundBuffer);


//   vector<double> testPrioriFFT;

//   for (int i = 0; i < complexSoundBuffer.size(); i++){
//       testPrioriFFT.push_back(complexSoundBuffer[i].real());
//   }

//   dumpDataToFile(testPrioriFFT, "/media/sf_Ubuntu_Shared_Folder", "PrioriFFT");

   //complexSoundBuffer = fft(complexSoundBuffer);


   complexSoundBuffer = fftw3(complexSoundBuffer);

   vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);


//   vector<double> testPosterior;

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



vector<complex<double>> DtmfDecoder::realToComplexVector(vector<double> reals, int count)
{
    vector<complex<double>> complexes;

    //wave amping:



    for(int p = 0; p < _repeatPadding; p++){
        //Testing different window types - Linear window sucks
        double sinusIncrement = 0;


        for (int i = 0; i < count; i += _downSampling){
            complex<double> number;
            number.real((double)reals[i]*sin(sinusIncrement)); //-> Sinusincrement

            complexes.push_back(number);


            sinusIncrement += (M_PI/(count))*_downSampling;
        }
    }


    return complexes;
}


vector<complex<double> > DtmfDecoder::fftw3(vector<complex<double> > &data)
{

     int N = data.size();


     // Zero out input array for fft
     for (int i = 0; i<N; i++) {
         fftin[i][0] = 0;
         fftin[i][1] = 0;
     }

     // Setup forward fft
     my_plan = fftw_plan_dft_1d(N, fftin, fftout, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < data.size(); i++) {
      fftin[i][0] = data[i].real();
      fftin[i][1] = 0; // complex part of the array
    }

    fftw_execute(my_plan);

    vector<complex<double>> result;

    for (int i = 0; i < N; i++){
        complex<double> complexObj;
        complexObj.real(fftout[i][0]);
        complexObj.imag(fftout[i][1]);
        result.push_back(complexObj);
    }

    return result;
}

vector<DtmfDecoder::signal> DtmfDecoder::sequenceToSignals(const vector<complex<double>> &sequence)
{
    vector<signal> sigs;


    for (int i = _cutoffLow * (_sampleTime*_repeatPadding); i < _cutoffHigh * (_sampleTime*_repeatPadding); i++){
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

    if(peakAmp.at(0) < _ampthreshhold || peakAmp.at(1) < _ampthreshhold){
        //cout << "Low Amp block: " << peakAmp.at(0) << "    :    HIGH    :    " << peakAmp.at(1) <<  endl;

        peakFreqs[0] = -1;
    }

    return peakFreqs; // Error decimal

}

int DtmfDecoder::frequencyToDtmf(double lowfreq, double highfreq){

    double lowshortestDistance = abs(_lowFreqs[0]-lowfreq);
    int lowIndex = 0;

    double highshortestDistance = abs(_highFreqs[0]-highfreq);
    int highIndex = 0;

    for(int i = 1; i < 4; i++){
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
        return -2;
    }
}

bool DtmfDecoder::isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex)
{
    double errorPercentage = _errorMargin;

    //cout << "ERROR Percentage: " << (lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex]*100.0 << endl;

    if((abs(lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex])*100.0 > errorPercentage){
        //cout << "ERROR LOW Percentage: " << (lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex]*100.0 << endl;
        return false;
    }

    if((abs(highfreq-_highFreqs[highFreqIndex])/_highFreqs[highFreqIndex])*100.0 > errorPercentage){
        //cout << "ERROR HIGH Percentage: " << (abs(highfreq-_highFreqs[highFreqIndex])/_highFreqs[highFreqIndex])*100.0 << endl;
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


