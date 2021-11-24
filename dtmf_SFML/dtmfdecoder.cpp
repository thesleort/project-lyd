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

bool DtmfDecoder::detectDTMFTone0(const sf::Int16* data, int count)
{

    vector<complex<double>> complexSoundBuffer = realToComplexVector(data,count);

    complexSoundBuffer = fft(complexSoundBuffer);

    vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

    vector<double> peaks = findSignalPeaks(frequencyData);

    //return isDTMF_N(peaks[0], peaks [1], );

}

complex<double> DtmfDecoder::Goertzel(const sf::Int16 *data, double frequency, int vectorCount)
{

    double w,cw,sw,c;
    double z1 = 0;
    double z2 = 0;

    w = 2*M_PI*frequency/vectorCount;
    cw = cos(w); c = 2*cw;
    sw = sin(w);
    z1=0; z2=0;

    for(int i = 0 ; i < vectorCount ; i++){
       double z0 = data[i] + c*z1 - z2;
       z2 = z1;
       z1 = z0;

    }
    complex<double> result;

    result.real(cw*z1 - z2);
    result.imag(sw*z1);

    return result;
}




vector<complex<double>> DtmfDecoder::realToComplexVector(const Int16* reals, int count)
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


