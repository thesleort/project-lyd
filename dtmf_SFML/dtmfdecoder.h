#ifndef DTMFDECODER_H
#define DTMFDECODER_H
#include <vector>
#include <complex>
#include <SFML/Audio.hpp>

using namespace std;

class DtmfDecoder
{
public:
    DtmfDecoder(double sampleTime);
    int identifyDTMF(const sf::Int16* data, int count);
    bool detectDTMFTone0(const sf::Int16* data, int count);

private:
    struct signal {
        signal(double amplitude, double frequency) : amplitude(amplitude), frequency(frequency){}
        double amplitude;
        double frequency;
    };
    vector<double> _lowFreqs = {697.0, 770.0, 852.0, 941.0};
    vector<double> _highFreqs = {1209.0, 1336.0, 1477.0, 1633.0};
    const int _cutoffLow = 600;
    const int _cutoffHigh = 1800;
    const int _middlefreq = 1100;
    double _sampleTime;

    double sequenceToFrequence(int seq){return seq / _sampleTime; };

    vector<complex<double>> realToComplexVector(const sf::Int16* reals, int count, int resMultiplier);

    vector<complex<double>> fft(vector<complex<double>>& data);

    vector<signal> sequenceToSignals(const vector<complex<double>>& sequence);

    vector<double> findSignalPeaks(const vector<signal> & signals, double error = 1);

    int frequencyToDtmf(double lowfreq, double highfreq);

    // dumps a vector<double> to a textfile for debugging
    void dumpDataToFile(vector<double>& data, string path, string fileName);

    //Synchronisation protocol function
    bool isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex);
};

#endif // DTMFDECODER_H
