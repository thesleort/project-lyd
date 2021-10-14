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
    int indentifyDTMF(sf::Int16* data, int count);

private:
    struct signal {
        signal(double amplitude, double frequency) : amplitude(amplitude), frequency(frequency){}
        double amplitude;
        double frequency;
    };
    vector<int> _lowFreqs = {697, 770, 852, 941};
    vector<int> _highFreqs = {1209, 1336, 1477, 1633};
    const int _cutoffLow = 600;
    const int _cutoffHigh = 1800;
    const int sampleTime;

    int frequenceToSequence(double freq){return freq * sampleTime; };
    double sequenceToFrequence(int seq){return seq / sampleTime; };

    vector<complex<double>> realToComplexVector(sf::Int16* reals, int count);

    vector<complex<double>> fft(vector<complex<double>>& data);

    vector<signal> sequenceToSignals(vector<complex<double>>& sequence);

    double error(double val, double ref);

    double intervalMaxAmp(vector<signal>& signals);

    int signalsToDtmf(const vector<signal>& signals);
    int frequencyToDtmf(double lowfreq, double highfreq, int error);

    // dumps a vector<double> to a textfile for debugging
    void dumpDataToFile(vector<double>& data, string path, string fileName);
};

#endif // DTMFDECODER_H
