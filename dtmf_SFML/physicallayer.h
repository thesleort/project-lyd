#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>
#include <semaphore.h>
#include <SFML/Audio.hpp>
#include "dtmfEncoder.h"
#include "dtmfdecoder.h"

#define RECORD_SAMPLERATE 44100.0

#define OUTPUT_SAMPLERATE 44100.0

#define SLIDEWINDOWSIZE 100


using namespace std;

class PhysicalLayer : public sf::SoundRecorder
{
public:
    PhysicalLayer(int duration, int sampletime);
    ~PhysicalLayer();
    bool readInBuffer(int& dtmf);
    bool writeOutBuffer(int dtmf);
    virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount) override; //Prototype


private:
    int _dtmfTone = 0;
    int _dtmfComboCounter = 0; //<- Find bedre navn
    int _comboMax = 1;

    int slidingWindowIterator = 0; // Slide for inputing samples

    int slidingDecodeIterator = 0; //Slide for decoding samples

    sf::Clock clock;


    sf::SoundBuffer _recordBuffer;
    const sf::Int16* _samples;
    vector<sf::Int16> _vectorSamples;

    vector<sf::SoundBuffer> _vectorRecordedBuffers;

    int _sampleCount;

    vector<int> _outBuffer, _inBuffer;
    int _duration;
    int _sampleTime;

    sem_t _inBufferMutex, _outBufferMutex;

    sem_t _soundVectorMutex;//Testing purposes

    void encoding();
    void decoding();

    void decodingV2(); //Test version af Decoding koblet til prototype onProcessSamples
    DtmfDecoder _decodeObj;
    sf::SoundBuffer _soundbuffer;

    void inputSampleToBuffer(const sf::Int16 *samples, std::size_t sampleCount);
};

#endif // PHYSICALLAYER_H
