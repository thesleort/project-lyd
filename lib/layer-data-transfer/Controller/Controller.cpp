#include <vector>
#include <iostream>

#include "data-transfer-config.h"
#include "Cyclic.h"
#include "bytestuffer.h"
#include "framegen.h"
#include "Controller.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//-------------Constructor/Deconstructor-----------
Controller::Controller(double s) {
  _ReceiveMessageBuffer = new vector<vector<vector<bool>>>;
  _ReceivedACKBuffer = new vector<vector<bool>>;
  _incomingFrames = new vector<vector<int>>;
  _outgoingMessages = new vector<vector<bool>>;

  _outputBuffer = new vector<int>; // Output to channel
  _inputBuffer = new vector<int>;  // Incoming channel NEEDS TO BE CHRONOLOGICALLY INDEXED FROM 0

  _FG = new FrameGenerator();
  _Stuffer = new bytestuffer({1, 1, 1, 1}, {0, 0, 0, 0});
  _pLayer = new PhysicalLayer(60,60,15);
  _CRChecker = new Cyclic({1, 0, 1, 1, 1});

  _timeout = (s / 0.00005); // implicit typecast :3
  // sem_init(&_outbufferLock,0,1);
  receiveThread = new thread(&Controller::autoReceive, this);   // auto receive on _incomingframes->_RMSGBuffer
  splitThread = new thread(&Controller::autoSplitInput, this);  // auto split _inputbuffer into frames->_incomingframes
  transmitThread = new thread(&Controller::autoTransmit, this); // auto transmit on MSG from _outgoingMSGbuffer
  DTMFreadT = new thread(&Controller::autoWriteDTMF, this);     // auto write DTMF from beginning of _outputbuffer
  DTMFwriteT = new thread(&Controller::autoReadDTMF, this);     // auto read DTMF to end of _inputbuffer
}

Controller::~Controller() {
  delete _ReceiveMessageBuffer;
  delete _ReceivedACKBuffer;
  delete _CRChecker;
  delete _Stuffer;
  delete _incomingFrames;
  delete _outgoingMessages;
  delete _pLayer;

  // sem_destroy(&_outbufferLock);
  receiveThread->join();
  transmitThread->join();
  splitThread->join();
  DTMFreadT->join();
  DTMFwriteT->join();
}

//-------------Setup---------
void Controller::addOutput(vector<int> &out) {
  _outputBuffer = &out;
}

void Controller::addInput(vector<int> &in) {
  _inputBuffer = &in;
}

//------------Public Access-----------

void Controller::write(vector<bool> msg) {
  _TMstackLock.lock();
  _outgoingMessages->push_back(msg);
  _TMstackLock.unlock();
  m_cv_autoTransmit.notify_all();
}

bool Controller::checkReceive() {
  bool element = _ReceiveMessageBuffer->size() > 0;
  return element;
}

vector<bool> Controller::read() {
  std::unique_lock<std::mutex> lock(m_readMutex);
  m_cv_read.wait(lock);
  if (!checkReceive()) {
    return {};
  } else {
    _RMstackLock.lock();
    vector<bool> msg = _ReceiveMessageBuffer->at(0).at(DATA);
    _ReceiveMessageBuffer->erase(_ReceiveMessageBuffer->begin());
    _RMstackLock.unlock();
    return msg;
  }
}

//------------Transmission------------

bool Controller::compareACK(vector<bool> in) {
  bool ACKcorrect = 0;
  if (_ReceivedACKBuffer->size() < 1) {
  } else {
    if (in == _ReceivedACKBuffer->at(0)) {
      ACKcorrect = 1;
      _ACKstackLock.lock();
      _ReceivedACKBuffer->erase(_ReceivedACKBuffer->begin()); // if seq is same as ACK seq, set bool, erase vect
      _ACKstackLock.unlock();
    } else {
      _ACKstackLock.lock();
      _ReceivedACKBuffer->erase(_ReceivedACKBuffer->begin()); // if not, seq is deleted and ignored so we dont get stuck
      _ACKstackLock.unlock();
#ifdef DEBUG_PRINT
      cout << "ACK deleted thrown out" << endl;
#endif
    }
  }
  return ACKcorrect;
}

void Controller::Transmit(vector<bool> msg) {
  vector<bool> crc = _CRChecker->Encode(msg);
  vector<bool> type = _msgType;
  vector<bool> seq = _currentSeq;
  vector<vector<bool>> parts = {msg, type, seq, crc};
  vector<bool> frame = _FG->generateFrame(parts); // gen frame

  vector<int> intMsg = _Stuffer->stuff(frame); // stuff
#ifdef DEBUG_PRINT
  cout << "sent msg in integers" << endl;
  for (int n : intMsg) {
    cout << n << " ";
  }
  cout << endl;

  cout << "--------------" << endl;
#endif
  _outbufferLock.lock();
  for (int n : intMsg) {
    _outputBuffer->push_back(n); // values inserted in order on end of buffer, buffer is read from index 0 on split
  }
  _outbufferLock.unlock();

  //_outputBuffer->insert(_outputBuffer->begin(),intMsg.begin(),intMsg.end()); //insert on buffer

  // wait for any ack, can never receive old ack since we dont send msg without getting an ack

  int k = _timeout;

  bool ACK = compareACK(seq);
  while (k > 0 && !ACK) { // timeout 10s or ACKreceived->continue
#ifdef _WIN32
    Sleep(0.02); // Windows sleep
#else
    // usleep(2); // Linux sleep
    std::this_thread::sleep_for(std::chrono::microseconds(20));
#endif
    ACK = compareACK(seq);
    k--;
    // cout<<k<<endl;
  }
  if (!ACK) { // if no ACK is received either the frame or ack was lost or an error occurred
    Transmit(msg);
    cout<<"retransmit"<<endl;
  } else { // if an ACK has been received, we flip the _currentSeq, which means the next msg we send has a new seqnr
    _currentSeq.flip();
#ifdef DEBUG_PRINT
    cout << Name << "ack receive registered by transmit, seq flipped now: ";
    for (bool n : _currentSeq) {
      cout << n;
    }
    cout << endl;
#endif
    ACK = 0;
  }
}

void Controller::Receive(vector<int> in) {
// assumes: {data,type,seq,crc};
#ifdef DEBUG_PRINT
  cout << "Received() message:" << endl;
  for (int n : in) {
    cout << n;
  }
  cout << endl;
  cout << "--------------" << endl;
#endif

  vector<vector<bool>> msg = _FG->splitFrame(_Stuffer->unstuff(in));

  if (msg.at(TYPE) == _msgType) { // if msg is a message, we do a CRC check
    
    if (_CRChecker->Decode(msg.at(DATA), msg.at(CRCC))) { // If crc returns no error, we check the sequencenumber
      if (msg.at(SEQ) == _lastReceivedSeq) {
        // if seq of msg is same as last message, and CRC shows no error, the last ACK has been lost and we "resend" the ack
        TransmitACK(_lastReceivedSeq);
#ifdef DEBUG_PRINT
        cout << "ACK Retransmitted" << endl;
#endif
      } else {
        // if seq number is "new" we transmit an ack and place msg into buffer
        TransmitACK(msg.at(SEQ)); // since we check for any ack and handle redundancy on _lRS  check, ack seqnr is useless(might be usefull later)
        _lastReceivedSeq = msg.at(SEQ);
#ifdef DEBUG_PRINT
        cout << "new ACK transmitted" << endl;
#endif
        _RMstackLock.lock();
        _ReceiveMessageBuffer->push_back(msg); // msg->buffer
        _RMstackLock.unlock();
        m_cv_read.notify_all();
      }
    } // if a crc error has occurred we just wait for the msg to be resent
    
  } else if (msg.at(TYPE) == _ackType) {
    _ACKstackLock.lock();
    _ReceivedACKBuffer->push_back(msg.at(SEQ)); // Saves SEQ on received ACK stack
    _ACKstackLock.unlock();
#ifdef DEBUG_PRINT
    cout << "-----------" << endl;
#endif
  }
}

void Controller::TransmitACK(vector<bool> seq) {
  vector<bool> decoydata = {0, 1, 1, 0};
  vector<vector<bool>> parts = {decoydata, _ackType, seq};
  vector<bool> frame = _FG->generateACKFrame(parts);
  vector<int> intAck = _Stuffer->stuff(frame);
  _outbufferLock.lock();
  for (int n : intAck) {
    _outputBuffer->push_back(n);
  }
  _outbufferLock.unlock();

 
}

void Controller::autoTransmit() {
  while (1) {
    std::unique_lock<std::mutex> lock(m_autoTransmitMutex);
    m_cv_autoTransmit.wait(lock);
    while (_outgoingMessages->size() > 0) {
      if (_TMstackLock.try_lock()) {
      vector<bool> msg = _outgoingMessages->at(0);          // first element on outgoing frames is msg
      _outgoingMessages->erase(_outgoingMessages->begin()); // first msg on stack is
      _TMstackLock.unlock();
      Transmit(msg);
      }
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

void Controller::autoReceive() {
  while (1) {
    while (_incomingFrames->size() > 0) {
      _splitFramesLock.lock();
      vector<int> frame = _incomingFrames->at(0);
      _incomingFrames->erase(_incomingFrames->begin());
      _splitFramesLock.unlock();
      Receive(frame);
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

void Controller::autoSplitInput() {
  while (1) {
    std::unique_lock<std::mutex> lock(m_autoSplitMutex);
    m_cv_autoSplit.wait(lock);
    while (_inputBuffer->size() > 2) {
      SplitBuffer();
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

void Controller::SplitBuffer() {
  _inbufferLock.lock();
  int _flagI = _Stuffer->getFlag();
  int _etcI = _Stuffer->getEtc();
  int size = _inputBuffer->size();
  vector<int> frame;
  int frameStart = -1; // starting flag position
  try {
    for (int i = 0; i < size - 1; i++) {   // look at size -1 values since we check i+1 to determine flag
      if (_inputBuffer->at(i) == _flagI) { // if flag is found it might be a starting flag
        if (i == 0) {                      // else/iffed on i=0 if value after 0 wasnt type+seq because of && statement, resulted in checking i-1 for i=0, fixed by taking else if to i=0 statement instead of i=0 && ....
          if (_inputBuffer->at(i + 1) == 4 || _inputBuffer->at(i + 1) == 7 || _inputBuffer->at(i + 1) == 8 || _inputBuffer->at(i + 1) == 11) {
            frameStart = 0; // if it is the buffer beginning, it is a flag if the next value is a combination of type + seq
            break;
          }
        } else if (_inputBuffer->at(i - 1) != _etcI) { // else if previous int is not etc,
          if (_inputBuffer->at(i + 1) == 4 || _inputBuffer->at(i + 1) == 7 || _inputBuffer->at(i + 1) == 8 || _inputBuffer->at(i + 1) == 11) {
            frameStart = i; // we have a starting flag if the next int is a valid type+seq
            break;
          }
        } // if not we keep looking      
      } 
    }
  } catch (const exception &e) {
    cout << "Framestart error" << e.what() << endl;
    cout << "size: " << size << "actual size: " << _inputBuffer->size() << endl;
    for (int i : *_inputBuffer) {
      cout << i;
    }
    exit(0);
  }
  int frameStop = -1;
  try {
    if (frameStart != -1) {                                                      // only look for framestop if we have a defined start
                                                                                 //  cout << "Framestart +1 - size: " << frameStart+1 - size << endl;
      for (int i = frameStart + 1; i < size; i++) {                              // Starts at +1 since we do not want the starting flag
        if (_inputBuffer->at(i) == _flagI && _inputBuffer->at(i - 1) != _etcI) { // the next flag we find without an etc will be the stop         
          frameStop = i;
          break;
        } else if(i>2) {
            if(_inputBuffer->at(i) == _flagI && _inputBuffer->at(i - 1) == _etcI && _inputBuffer->at(i - 2) == _etcI){//deals with natural 0s before flags by accepting stop flag if it has 2 etc before it(natural 0 + stuff), cant break(at(-1)) because it requries i>2
              frameStop = i;
              break;
            }
        }
      }
    }
  } catch (const exception &e) {
    cout << "Framestop error" << e.what();
    cout << "size: " << size << "actual size: " << _inputBuffer->size() << endl;
    for (int i : *_inputBuffer) {
      cout << i;
    }
    cout << endl;
  }

  if (frameStart != -1 && frameStop != -1) {
    for (int i = frameStart; i <= frameStop; i++) {

      frame.push_back(_inputBuffer->at(i)); // frame is created based on start and stop values
    }
    _splitFramesLock.lock();
    _incomingFrames->push_back(frame);
    _splitFramesLock.unlock();
    _inputBuffer->erase(_inputBuffer->begin(), _inputBuffer->begin() + frameStop + 1); // fixes program. in erase range with begin(), the amount of erased elements is equal to It_last-It_first(so if framestop without +1 is used and framestop index is 2, only 2 elements are deleted from input)
  }
  _inbufferLock.unlock();
}

void Controller::autoReadDTMF() {
  int tone;
  while (1) {
    if (_pLayer->readInBuffer(tone)) {
      _inbufferLock.lock();
      _inputBuffer->push_back(tone); // oldest first in buffer
      _inbufferLock.unlock();
      m_cv_autoSplit.notify_all();
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

void Controller::autoWriteDTMF() {
  while (1) {
    while (_outputBuffer->size() > 1) {
      _outbufferLock.lock();
      int size = _outputBuffer->size();
      for (int i = 0; i < size; i++) {
        _pLayer->writeOutBuffer(_outputBuffer->at(0)); // sends frame from beginning of buffer
        _outputBuffer->erase(_outputBuffer->begin());
      }
      _outbufferLock.unlock();
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

void Controller::printReceived() {

  cout << endl;
  cout << "Printing Receipt.... Length: " << _ReceiveMessageBuffer->size();
  cout << endl;
  _RMstackLock.lock();
  for (int i = 0; i < _ReceiveMessageBuffer->size(); i++) {
    cout << "Message " << i + 1 << " :" << endl;
    for (int j = 0; j < _ReceiveMessageBuffer->at(i).size(); j++) {
      for (int k = 0; k < _ReceiveMessageBuffer->at(i).at(j).size(); k++) {
        cout << _ReceiveMessageBuffer->at(i).at(j).at(k);
      }
      cout << " ";
    }
    cout << endl;
  }
  _RMstackLock.unlock();
}
