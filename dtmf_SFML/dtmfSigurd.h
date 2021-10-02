#ifndef DTMF_H
#define DTMF_H
#include <SFML/Audio.hpp>
#include <vector>
using namespace std;
using namespace sf;

class Dtmf
{
private:
    const int _lowFreqs[4] = {697, 770, 852, 941};
    const int _highFreqs[4] = {1209, 1336, 1477, 1633};
    const double _TWO_PI = 6.28318;

    Sound _sound;
    SoundBuffer _buffer;

    int _amplitude;
    int _mSec;
    int _sampleRate;

public:
    Dtmf(int mSec, int sampleRate, int amplitude);

    int getStatus();
    void playDualTone(int freq1, int freq2);
    void playDtmfTone(int tone1, int tone2);

    void setAmplitude(int amplitude);
    void setSampleRate(int sampleRate);
    void setDuration(int mSec);
};

#endif // DTMF_H
