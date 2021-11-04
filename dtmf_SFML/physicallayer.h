#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>
#include <semaphore.h>
#include <SFML/Audio.hpp>

using namespace std;

class PhysicalLayer : public sf::SoundRecorder
{
public:
    PhysicalLayer(int duration, int sampletime);
    ~PhysicalLayer();
    bool readInBuffer(int& dtmf);
    bool writeOutBuffer(int dtmf);
    virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount) override;

private:
    vector<int> _outBuffer, _inBuffer;
    int _duration;
    int _sampleTime;

    sem_t _inBufferMutex, _outBufferMutex;

    void encoding();
    void decoding();
};

#endif // PHYSICALLAYER_H
