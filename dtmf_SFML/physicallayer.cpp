#include "physicallayer.h"
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>



using namespace std;

PhysicalLayer::PhysicalLayer(int duration, int sampleTime)
    :_decodeObj(sampleTime)
{
    _duration = duration;
    _sampleTime = sampleTime;


    setProcessingInterval(sf::milliseconds(sampleTime));


    sem_init(&_inBufferMutex, 0, 1);
    sem_init(&_outBufferMutex, 0, 1);
    sem_init(&_soundVectorMutex, 0, 1);

    //Initiate soundbuffer window
    _vectorRecordedBuffers = vector<sf::SoundBuffer>(SLIDEWINDOWSIZE+1);

    //clock = sf::Clock();

    new thread(&PhysicalLayer::encoding, this);
    new thread(&PhysicalLayer::decodingV2, this);


    //new thread(&PhysicalLayer::recordMode, this);
    start(RECORD_SAMPLERATE);
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
    DtmfEncoder encoder(_duration, OUTPUT_SAMPLERATE, 3000);
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

void PhysicalLayer::decoding() //Testing implementing Actual Decoding:
{
    int i = 0;


    while(true){
        sem_wait(&_inBufferMutex);

        _inBuffer.push_back(i);

        sem_post(&_inBufferMutex);

        i++;
        i = i % 16;
    }
    return;
}

void PhysicalLayer::decodingV2()
{
    while(true){
        while(_vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount() == 0){} //0 -> No samples.

        //cout << _vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount() << endl;

        //Hele indsættelsen af _soundbuffer skal være i en linje, for at undgå threading fejl.

        int i = _decodeObj.identifyDTMF(_vectorRecordedBuffers[slidingDecodeIterator].getSamples(), _vectorRecordedBuffers[slidingDecodeIterator].getSampleCount(),(_vectorRecordedBuffers[slidingDecodeIterator].getSampleCount()/RECORD_SAMPLERATE)*1000.0);
        //cout << clock.getElapsedTime().asMilliseconds() << endl;


        if(i >= 0){
            //cout << i << endl;
        }
//        if(i >= 0 && i == _dtmfTone){
//            _dtmfComboCounter++;
//            if(_dtmfComboCounter == _comboMax){
//                cout << i << endl;
//                _dtmfComboCounter = 0;
//            }
//        }else{
//            _dtmfTone = i;
//            _dtmfComboCounter = 0;
//        }


        _vectorRecordedBuffers[slidingDecodeIterator] = SoundBuffer();
        switch(slidingDecodeIterator){
        case SLIDEWINDOWSIZE:
            slidingDecodeIterator = 0;
            break;
        default:
            slidingDecodeIterator++;
        }




//        _vectorSamples.clear();
//        _soundbuffer = SoundBuffer();

        //cout << "clocktime: " << clock.getElapsedTime().asMilliseconds() << endl;


    }
    return;

}

void PhysicalLayer::inputSampleToBuffer(const Int16 *samples, size_t sampleCount)
{
    //sf::Clock clock;
    _vectorSamples.clear();
    _soundbuffer = SoundBuffer();
    //cout <<  "sampletime Mikroseconds: " << clock.getElapsedTime().asMicroseconds() <<  "   : <3"  <<  endl;

    std::copy(samples, samples + sampleCount, std::back_inserter(_vectorSamples));
    _soundbuffer.loadFromSamples(&_vectorSamples[0], _vectorSamples.size(), getChannelCount(), getSampleRate());

    _vectorRecordedBuffers[slidingWindowIterator] = _soundbuffer;

    switch(slidingWindowIterator){
    case SLIDEWINDOWSIZE:
        slidingWindowIterator = 0;
        break;
    default:
        slidingWindowIterator++;
    }


}

bool PhysicalLayer::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{

   new thread(&PhysicalLayer::inputSampleToBuffer, this, samples, sampleCount);
//   cout << clock.getElapsedTime().asMilliseconds() << endl;
//   clock.restart();
   return true;
}

