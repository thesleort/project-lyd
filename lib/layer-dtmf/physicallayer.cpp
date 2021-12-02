#include "physicallayer.h"
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>
#include "digitalfilter.h"

using namespace std;

PhysicalLayer::PhysicalLayer(int durationSound, int durationRecord, int sampletime)
    : _decodeObj(sampletime)
{
    _duration = durationSound;
    _sampleTime = sampletime;
    _durationRecorded = durationRecord;

    setProcessingInterval(sf::milliseconds(durationRecord));

    sem_init(&_inBufferMutex, 0, 1);
    sem_init(&_outBufferMutex, 0, 1);
    sem_init(&_soundVectorMutex, 0, 1);

    //Initiate soundbuffer window
    _vectorRecordedBuffers = vector<sf::SoundBuffer>(SLIDEWINDOWSIZE+1);

    //Initiate decoder Obj



    new thread(&PhysicalLayer::encoding, this);
    new thread(&PhysicalLayer::decodingV2, this);



    //new thread(&PhysicalLayer::recordMode, this);
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

     _dtmfEncoder = DtmfEncoder(_duration, OUTPUT_SAMPLERATE, 10000);
     sf::Clock clock;

     while(true){

         if (_outBuffer.size() > 0){
             cout << "_outBuffer size: " << _outBuffer.size() << endl;
             soundPlaying = true;


             //encoder.playDtmfTone(16);

             sem_wait(&_outBufferMutex);

             _dtmfEncoder.playDtmfTone(16);

             _dtmfEncoder.playDtmfTone(_outBuffer.at(0));


             //cout << _outBuffer.at(0) << endl;
             _outBuffer.erase(_outBuffer.begin());

             sem_post(&_outBufferMutex);

         }else{
             if(soundPlaying == true){
             soundPlaying = false;
             timeSinceSoundPlayed.restart();
             }
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

        //cout << "decode samplesize: " <<_vectorRecordedBuffers[slidingDecodeIterator].getSampleCount() << endl;


        int i = _decodeObj.identifyDTMF(_vectorRecordedBuffers.at(slidingDecodeIterator).getSamples(), _vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount(),(_vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount()/RECORD_SAMPLERATE)*1000.0);



        //cout << clock.getElapsedTime().asMilliseconds() << endl;


        // cout << "DTMF TONE: " << i << endl;


        if(i >= 0 && i == _dtmfTone){
            _dtmfComboCounter++;
            if(_dtmfComboCounter == _comboMax){


                sem_wait(&_inBufferMutex);

                _inBuffer.push_back(i);

                sem_post(&_inBufferMutex);


                //cout << "                                              COMBO BREAKER: " << i << endl;
                _decodeObj.UpdateAmpBlock();
                _dtmfComboCounter = 0;
            }
        }else{
            _dtmfTone = i;
            _dtmfComboCounter = 1;
        }

        _vectorRecordedBuffers.at(slidingDecodeIterator) = SoundBuffer();

        //cout << "Decoded: " <<slidingDecodeIterator << endl;

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

        //write identified tone to input buffer

    }
    return;

}

void PhysicalLayer::inputSampleToBuffer(const Int16 *samples, size_t sampleCount)
{

    _vectorSamples.clear();


    //cout <<  "sampletime Mikroseconds: " << clock.getElapsedTime().asMicroseconds() <<  "   : <3"  <<  endl;

    std::copy(samples, samples + sampleCount, std::back_inserter(_vectorSamples));

    int splitNum = _durationRecorded/_sampleTime;
    for(int i = 0; i < splitNum; i++){ // Split sound, and send three record packets with sampletime as their duration

        _soundbuffer = SoundBuffer();
        _soundbuffer.loadFromSamples(&_vectorSamples[(_vectorSamples.size()/splitNum)*i], _vectorSamples.size()/splitNum, getChannelCount(), getSampleRate());


        _vectorRecordedBuffers[slidingWindowIterator] = _soundbuffer;
        //cout << "sliding iterator: " << slidingWindowIterator << endl;



       //cout << "Inserted sample count: " <<  slidingWindowIterator << endl;

        switch(slidingWindowIterator){
        case SLIDEWINDOWSIZE:
            slidingWindowIterator = 0;
            break;
        default:
            slidingWindowIterator++;
        }



    }



}

bool PhysicalLayer::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{
    if(timeSinceSoundPlayed.getElapsedTime().asMilliseconds() > 1000 && soundPlaying == false){
    new thread(&PhysicalLayer::inputSampleToBuffer, this, samples, sampleCount);
    }

    return true;
}

