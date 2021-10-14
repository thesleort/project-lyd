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

DtmfDecoder::DtmfDecoder(double sampleTime) : sampleTime(sampleTime){

}

vector<complex<double> > DtmfDecoder::realToComplexVector(Int16* reals, int count)
{
    vector<complex<double>> complexes;

    for (int i = 0; i < count; i++){
        complex<double> number;
        number.real((double)reals[i]);
        complexes.push_back(number);
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

vector<DtmfDecoder::signal> DtmfDecoder::sequenceToSignals(vector<complex<double> > &sequence)
{
    vector<signal> sigs;
    for (int i = _cutoffLow * sampleTime; i < _cutoffHigh * sampleTime; i++){
        double real = sequence.at(i).real();
        double imag = sequence.at(i).imag();
        double amp = sqrt(real*real + imag*imag); // possible optimization by not taking square root
        double freq = i / sampleTime;
        signal sig(amp, freq);
        sigs.push_back(sig);
    }
}

double DtmfDecoder::error(double val, double ref){
    return (val - ref) / ref;
}

int signalsToDtmf(const vector<double>& signals){

}

int DtmfDecoder::splitHighestPeak(const vector<double> &data)
{
    // En anden måde at splitte en vektor i 2, syntes syntax her er mere clean
    // std::vector<int> v = { 1, 2, 3, 4, 5 };
    //
    // std::vector<int> left(v.begin(), v.begin() + v.size() / 2);
    // std::vector<int> right(v.begin() + v.size() / 2, v.end());

    auto it = find(data.begin(), data.begin()+450, *max_element(data.begin(), data.begin()+450));

    int index = it - data.begin();

    vector<double> sampletesting;

    for(int i = 0; i < 4; i++){
        sampletesting.push_back(abs(_lowFreqs[i]-(index+_cutoffLow)));
    }

    auto lowF = find(sampletesting.begin(), sampletesting.end(), *min_element(sampletesting.begin(), sampletesting.end()));

    int lowIndex = lowF - sampletesting.begin();

    //Decoding high frequency
    int frequencyShift = 500;

    it = find(data.begin()+frequencyShift, data.end(), *max_element(data.begin()+frequencyShift, data.end()));

    index = it - data.begin();

    sampletesting.clear();

    for(int i = 0; i < 4; i++){
        sampletesting.push_back(abs(_highFreqs[i]-(index+_cutoffLow)));
    }

    auto HighF = find(sampletesting.begin(), sampletesting.end(), *min_element(sampletesting.begin(), sampletesting.end()));

    int HighIndex = HighF - sampletesting.begin();

    return lowIndex*4+HighIndex;
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
