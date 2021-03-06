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
    : _decodeObj(sampletime) {
  _duration = durationSound;
  _sampleTime = sampletime;
  _durationRecorded = durationRecord;

  setProcessingInterval(sf::milliseconds(durationRecord));

  sem_init(&_inBufferMutex, 0, 1);
  sem_init(&_outBufferMutex, 0, 1);
  sem_init(&_soundVectorMutex, 0, 1);

  //Initiate soundbuffer window
  _vectorRecordedBuffers = vector<sf::SoundBuffer>(SLIDEWINDOWSIZE + 1);

  //Initiate decoder Obj

  new thread(&PhysicalLayer::encoding, this);
  new thread(&PhysicalLayer::decodingV2, this);

  start();
}

PhysicalLayer::~PhysicalLayer() {
  stop();
}

bool PhysicalLayer::readInBuffer(int &dtmf) {
  std::unique_lock<std::mutex> lock(m_readMutex);
  m_cv_read.wait(lock);
  if (_inBuffer.size() > 0) {
    sem_wait(&_inBufferMutex);

    dtmf = _inBuffer.at(0);
    _inBuffer.erase(_inBuffer.begin());

    sem_post(&_inBufferMutex);
    return true;
  } else {
    return false;
  }
}

bool PhysicalLayer::writeOutBuffer(int dtmf) {
  sem_wait(&_outBufferMutex);

  _outBuffer.push_back(dtmf);

  sem_post(&_outBufferMutex);
  m_cv_write.notify_all();
  return true;
}

void PhysicalLayer::encoding() {

  _dtmfEncoder = DtmfEncoder(_duration, OUTPUT_SAMPLERATE, 10000);
  sf::Clock clock;

  while (true) {
    std::unique_lock<std::mutex> lock(m_writeMutex);
    m_cv_write.wait(lock);
    while (_outBuffer.size() > 0) {
      soundPlaying = true;

      sem_wait(&_outBufferMutex);

      _dtmfEncoder.playDtmfTone(16);

      _dtmfEncoder.playDtmfTone(_outBuffer.at(0));

      _outBuffer.erase(_outBuffer.begin());

      sem_post(&_outBufferMutex);
    }
    if (soundPlaying == true) {
      soundPlaying = false;
      timeSinceSoundPlayed.restart();
    }
  }
  return;
}

void PhysicalLayer::decoding() //Testing implementing Actual Decoding:
{
  int i = 0;

  while (true) {
    sem_wait(&_inBufferMutex);

    _inBuffer.push_back(i);

    sem_post(&_inBufferMutex);

    i++;
    i = i % 16;
  }
  return;
}

void PhysicalLayer::decodingV2() {
  while (true) {
    while (_vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount() == 0) {
    } //0 -> No samples.

    //Hele inds??ttelsen af _soundbuffer skal v??re i en linje, for at undg?? threading fejl.

    int i = _decodeObj.identifyDTMF(_vectorRecordedBuffers.at(slidingDecodeIterator).getSamples(), _vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount(), (_vectorRecordedBuffers.at(slidingDecodeIterator).getSampleCount() / RECORD_SAMPLERATE) * 1000.0);

    if (i >= 0 && i == _dtmfTone) {
      _dtmfComboCounter++;
      if (_dtmfComboCounter == _comboMax) {

        sem_wait(&_inBufferMutex);

        _inBuffer.push_back(i);

        sem_post(&_inBufferMutex);

        _decodeObj.UpdateAmpBlock();
        _dtmfComboCounter = 0;
        m_cv_read.notify_all();
      }
    } else {
      _dtmfTone = i;
      _dtmfComboCounter = 1;
    }

    _vectorRecordedBuffers.at(slidingDecodeIterator) = SoundBuffer();

    switch (slidingDecodeIterator) {
    case SLIDEWINDOWSIZE:
      slidingDecodeIterator = 0;
      break;
    default:
      slidingDecodeIterator++;
    }

    //write identified tone to input buffer
  }
  return;
}

void PhysicalLayer::inputSampleToBuffer(const Int16 *samples, size_t sampleCount) {

  _vectorSamples.clear();

  std::copy(samples, samples + sampleCount, std::back_inserter(_vectorSamples));

  int splitNum = _durationRecorded / _sampleTime;
  for (int i = 0; i < splitNum; i++) { // Split sound, and send three record packets with sampletime as their duration

    _soundbuffer = SoundBuffer();
    _soundbuffer.loadFromSamples(&_vectorSamples[(_vectorSamples.size() / splitNum) * i], _vectorSamples.size() / splitNum, getChannelCount(), getSampleRate());

    _vectorRecordedBuffers[slidingWindowIterator] = _soundbuffer;

    switch (slidingWindowIterator) {
    case SLIDEWINDOWSIZE:
      slidingWindowIterator = 0;
      break;
    default:
      slidingWindowIterator++;
    }
  }
}

bool PhysicalLayer::onProcessSamples(const Int16 *samples, std::size_t sampleCount) {
  if (soundPlaying == false) {
    new thread(&PhysicalLayer::inputSampleToBuffer, this, samples, sampleCount);
  }

  return true;
}
