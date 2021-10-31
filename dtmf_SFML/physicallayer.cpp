#include "physicallayer.h"
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>

using namespace std;

PhysicalLayer::PhysicalLayer(double duration)
{
    sem_init(&_inBufferMutex, 0, 1);
    sem_init(&_outBufferMutex, 0, 1);
    new thread(&PhysicalLayer::encoding, this);
    new thread(&PhysicalLayer::decoding, this);
}

void PhysicalLayer::encoding()
{
    cout << "encoding called" << endl;

    while(true){
        sleep(1);
        if (_outBuffer.size() > 0){
            sem_wait(&_outBufferMutex);
            cout << _outBuffer.at(0) << endl;
            _outBuffer.erase(_outBuffer.begin());
            sem_post(&_outBufferMutex);
        }

    }

    return;
}

void PhysicalLayer::decoding()
{
    cout << "decoding called" << endl;

    int i = 0;
    while(true){
        sleep(1);
        sem_wait(&_outBufferMutex);
        _outBuffer.push_back(i);
        sem_post(&_outBufferMutex);

        i++;
    }

    return;
}
