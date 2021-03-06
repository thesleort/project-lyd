#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>
#include <semaphore.h>
#include <SFML/Audio.hpp>
#include <condition_variable>
#include "dtmfEncoder.h"
#include "dtmfdecoder.h"

#define RECORD_SAMPLERATE 44100.0

#define OUTPUT_SAMPLERATE 48000.0

#define SLIDEWINDOWSIZE 100

using namespace std;

class PhysicalLayer : public sf::SoundRecorder {
  public:
  PhysicalLayer(int durationSound, int durationRecord, int sampletime);
  ~PhysicalLayer();
  bool readInBuffer(int &dtmf);
  bool writeOutBuffer(int dtmf);
  virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount) override; //Prototype

  private:
  bool soundPlaying = false;

  int testIterator = 0;   //KIS
  int testInterator2 = 0; //KIS

  int _dtmfTone = -1;
  int _dtmfComboCounter = 0;
  int _comboMax = 3; //Ikke nul indekseret

  int slidingWindowIterator = 0; // Slide for inputing samples

  int slidingDecodeIterator = 0; //Slide for decoding samples

  sf::Clock _clock;               //DEbug object
  sf::Clock timeSinceSoundPlayed; //The onprocess sample can first start 1 second after the last sound has played.

  sf::SoundBuffer _recordBuffer;
  const sf::Int16 *_samples;
  vector<sf::Int16> _vectorSamples;

  vector<sf::SoundBuffer> _vectorRecordedBuffers;

  int _sampleCount;

  vector<int> _outBuffer, _inBuffer;
  double _durationRecorded;
  int _duration;
  int _sampleTime;

  sem_t _inBufferMutex, _outBufferMutex;

  sem_t _soundVectorMutex; //Testing purposes

  void encoding();
  //Initiation of encoding object

  void decoding();

  void decodingV2(); //Test version af Decoding koblet til prototype onProcessSamples
  DtmfDecoder _decodeObj;
  DtmfEncoder _dtmfEncoder;
  sf::SoundBuffer _soundbuffer;

  void inputSampleToBuffer(const sf::Int16 *samples, std::size_t sampleCount);

  std::condition_variable m_cv_read;
  std::mutex m_readMutex;

  std::condition_variable m_cv_write;
  std::mutex m_writeMutex;
};

#endif // PHYSICALLAYER_H
