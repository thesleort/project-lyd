#ifndef DTMFDECODER_H
#define DTMFDECODER_H
#include <vector>
#include <complex>
#include <SFML/Audio.hpp>

using namespace std;

class DtmfDecoder
{
public:
    DtmfDecoder();
    vector<double> doSample(int msDuration);


private:
    vector<double> _lowFreqs = {697, 770, 852, 941};
    vector<double> _highFreqs = {1209, 1336, 1477, 1633};
    int _cutoffLow = 600;
    int _cutoffHigh = 1800;
    int _FrequencyShift = 1100; //hop fra lav frekvenser til høje frekvenser, kun til testning.
    int _msDuration;

    // records amplitudes of sound for given duration, returns amplitudes as real part of complex number (complex are just zero)
    vector<complex<double>> recordData(int msDuration);


    // witchcraft
    vector<complex<double>> fft(vector<complex<double>>& data);

    // Return a list of amplitudes given a list of complex numbers
    vector<double> modulus(const vector<complex<double>>& data);

    // Finds dtmf tone given list of amplitudes
    vector<double>  splitHighestPeak(const vector<double>& data);

    // dumps a vector<double> to a textfile for debugging
    void dumpDataToFile(vector<double>& data, string path, string fileName);

    sf::SoundBufferRecorder _recorder;
};

#endif // DTMFDECODER_H
