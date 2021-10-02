#ifndef DTMFDECODER_H
#define DTMFDECODER_H
#include <vector>
#include <complex>
#include <SFML/Audio.hpp>

using namespace std;

class DtmfDecoder
{
public:
    DtmfDecoder(int dataRate);
    int doSample(int duration);

private:
    const int _lowFreqs[4] = {697, 770, 852, 941};
    const int _highFreqs[4] = {1209, 1336, 1477, 1633};

    vector<complex<double>> recordData();
    void fft(vector<complex<double>>& data);
    vector<double> modulus(const complex<double>& data);
    // identifyPeaks(vector<double>& data)
    // identifyClosestDtmf(peaks&)

    sf::SoundBufferRecorder _recorder;
};

#endif // DTMFDECODER_H
