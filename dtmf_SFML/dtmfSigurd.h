#ifndef DTMFS_H
#define DTMFS_H
#include <SFML/Audio.hpp>
#include <vector>
using namespace std;
using namespace sf;

class DtmfSigurd
{
private:
    const int _lowFreqs[4] = {697, 770, 852, 941};
    const int _highFreqs[4] = {1209, 1336, 1477, 1633};
    const double _2PI = 6.28318;

    Sound _sound;
    SoundBuffer _buffer;

    int _amplitude;
    int _mSec;
    int _sampleRate;

public:
    DtmfSigurd(int mSec, int sampleRate, int amplitude);

    int getStatus();
    void playDualTone(int freq1, int freq2);
    void playDtmfTone(int tone);

    void setAmplitude(int amplitude);
    void setSampleRate(int sampleRate);
    void setDuration(int mSec);
};

#endif // DTMFS_H
