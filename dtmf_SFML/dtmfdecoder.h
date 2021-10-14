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
    const int _lowFreqs[4] = {697, 770, 852, 941};
    const int _highFreqs[4] = {1209, 1336, 1477, 1633};
    const int _cutoffLow = 600;
    const int _cutoffHigh = 1800;
    const double sampleTime;

    double sequenceToFreqency(int sequence){return sequence / sampleTime; };
    int frequencyToSequence(double frequency){return frequency * sampleTime; };

    vector<complex<double>> realToComplexVector(sf::Int16* reals, int count);

    vector<complex<double>> fft(vector<complex<double>>& data);

    vector<double> complexToAmplitude(vector<complex<double>>& complexes);

    double error(double val, double ref);

    int intervalMax(const vector<double>& amps, int begin, int end);

    int splitHighestPeaks(const vector<double>& amps);

    // dumps a vector<double> to a textfile for debugging
    void dumpDataToFile(vector<double>& data, string path, string fileName);

    //OLD
    int splitHighestPeak(const vector<double> &data);
};

#endif // DTMFDECODER_H
