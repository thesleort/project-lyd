#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>
#include <semaphore.h>
#include <SFML/Audio.hpp>
#include "dtmfEncoder.h"
#include "dtmfdecoder.h"



using namespace std;

class PhysicalLayer : public sf::SoundRecorder
{
public:
    PhysicalLayer(int duration, int sampletime);
    ~PhysicalLayer();
    bool readInBuffer(int& dtmf);
    bool writeOutBuffer(int dtmf);
    virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount) override; //Prototype
    bool onStart() override;
    void onStop() override;

    int decodeTesting(); //Bruger jeg ikke men var til testning


private:
    int _dtmfTone = 0;
    int _dtmfComboCounter = 0; //<- Find bedre navn
    int _comboMax = 3;

    sf::SoundBuffer _recordBuffer;
    const sf::Int16* _samples;
    vector<sf::Int16> _vectorSamples;
    int _sampleCount;

    vector<int> _outBuffer, _inBuffer;
    int _duration;
    int _sampleTime;

    sem_t _inBufferMutex, _outBufferMutex;

    void encoding();
    void decoding();

    void decodingV2(); //Test version af Decoding koblet til prototype onProcessSamples
    DtmfDecoder _decodeObj;
    sf::Clock clock; // for testing purposes;
    sf::SoundBuffer _soundbuffer;
};

#endif // PHYSICALLAYER_H
