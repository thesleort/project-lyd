#include "physicallayer.h"
#include "dtmfEncoder.h"
#include "dtmfdecoder.h"
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>

using namespace std;

PhysicalLayer::PhysicalLayer(int duration, int sampleTime)
{
    _duration = duration;
    _sampleTime = sampleTime;
    setProcessingInterval(sf::milliseconds(sampleTime));
    sem_init(&_inBufferMutex, 0, 1);
    sem_init(&_outBufferMutex, 0, 1);
    new thread(&PhysicalLayer::encoding, this);
    new thread(&PhysicalLayer::decoding, this);
    start();
}

PhysicalLayer::~PhysicalLayer()
{
    stop();
}

bool PhysicalLayer::readInBuffer(int& dtmf)
{
    if(_inBuffer.size() > 0){
        sem_wait(&_inBufferMutex);

        dtmf = _inBuffer.at(0);
        _inBuffer.erase(_inBuffer.begin());

        sem_post(&_inBufferMutex);
        return true;
    }else{
        return false;
    }
}

bool PhysicalLayer::writeOutBuffer(int dtmf)
{
    sem_wait(&_outBufferMutex);

    _outBuffer.push_back(dtmf);

    sem_post(&_outBufferMutex);
    return true;
}

void PhysicalLayer::encoding()
{
    DtmfEncoder encoder(_duration, 44100, 3000);
    while(true){
        if (_outBuffer.size() > 0){
            sem_wait(&_outBufferMutex);

            encoder.playDtmfTone(_outBuffer.at(0));
            //cout << _outBuffer.at(0) << endl;
            _outBuffer.erase(_outBuffer.begin());

            sem_post(&_outBufferMutex);
            while(encoder.getStatus() == 2){};
        }
    }
    return;
}

void PhysicalLayer::decoding()
{
    int i = 0;
    while(true){

        sem_wait(&_inBufferMutex);

        _inBuffer.push_back(i);

        sem_post(&_inBufferMutex);

        i++;
        i = i % 16;
        sleep(1);
    }
    return;
}

bool PhysicalLayer::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{
    cout << "SAMPLE COUNT: " << sampleCount << endl;
    //for(int i = 0; i < sampleCount; i++){
    //    cout << *(samples+i) << " ";
    //}
    //cout << endl;
    return true;
}
