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
    int doSample(int msDuration);

private:
    const int _lowFreqs[4] = {697, 770, 852, 941};
    const int _highFreqs[4] = {1209, 1336, 1477, 1633};
    const int _cutoffLow = 600;
    const int _cutoffHigh = 3000;

    vector<complex<double>> recordData(int msDuration);
    vector<complex<double>> fft(vector<complex<double>>& data);
    vector<double> modulus(const vector<complex<double>>& data);

    // Find closest dtmf tone
    int splitHighestPeak(const vector<double>& data);
    int sampleAtFreq(const vector<double>& data);

    sf::SoundBufferRecorder _recorder;
};

#endif // DTMFDECODER_H
