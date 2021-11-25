#ifndef DTMFDECODER_H
#define DTMFDECODER_H
#define SAMPLERATE 44100
#include <vector>
#include <complex>
#include <SFML/Audio.hpp>
#include <fftw3.h>

#include "digitalfilter.h"


using namespace std;

class DtmfDecoder
{
public:
    DtmfDecoder(double sampleTime);
    int identifyDTMF(const sf::Int16* data, int count, double sampleTime);


private:
    struct signal {
        signal(double amplitude, double frequency) : amplitude(amplitude), frequency(frequency){}
        double amplitude;
        double frequency;
    };
    //Debugging Report:mp block, sound i
    //res: -1 sound to quiet
    //res: -2 Error margin is too great;
    vector<double> _lowFreqs = {697.0, 770.0, 852.0, 941.0};
    vector<double> _highFreqs = {1209.0, 1336.0, 1477.0, 1633.0};
    const int _cutoffLow = 600;
    const int _cutoffHigh = 1800;
    const int _middlefreq = 1100;
    double _sampleTime;


    //Normal sampling: 50 ms, 10 Repeat padding, downsample factor 10, amp threshold 5000, Errormargin = 2

    //Speed sampling 10 ms, 60 repeat padding, downsample factor 10, amp threshold 5000, ErrorMargin = 4

    double _errorMargin = 8.0;
    int _repeatPadding = 5;
    int _downSampling  = 2;
    int _ampthreshhold = 5000;


    //jonas fft:
    fftw_complex *fftin, *fftout;
    fftw_plan my_plan;

    //Bandpass filter
    DigitalFilter _digitalFilter;


    vector<complex<double>> realToComplexVector(vector<double>, int count);

    vector<complex<double>> fftw3(vector<complex<double>>& data);

    vector<signal> sequenceToSignals(const vector<complex<double>>& sequence);

    vector<double> findSignalPeaks(const vector<signal> & signals, double error = 1);

    int frequencyToDtmf(double lowfreq, double highfreq);

    // dumps a vector<double> to a textfile for debugging
    void dumpDataToFile(vector<double>& data, string path, string fileName);

    //Synchronisation protocol function
    bool isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex);
};

#endif // DTMFDECODER_H
