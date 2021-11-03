#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>
#include <semaphore.h>

using namespace std;

class PhysicalLayer
{
public:
    PhysicalLayer(int duration);
    bool readInBuffer(int& dtmf);
    bool writeOutBuffer(int dtmf);

private:
    vector<int> _outBuffer, _inBuffer;
    int _duration;

    sem_t _inBufferMutex, _outBufferMutex;

    void encoding();
    void decoding();
};

#endif // PHYSICALLAYER_H
