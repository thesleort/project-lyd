#ifndef H_CONTROLLER
#define H_CONTROLLER

#include "Cyclic.h"
#include "bytestuffer.h"
#include "framegen.h"
#include "physicallayer.h"
#include "semaphore.h"

#include <condition_variable>
#include <vector>
#include <mutex>
#include <string>
#include <thread>
//add IFNDEF on COUTs to define debugmode
class Controller {
  public:
  //Constructor/deconstructor
  Controller(double);
  ~Controller();

  //Buffer Read/Write/Check
  void write(vector<bool>);
  vector<bool>read();
  bool checkReceive();

private:
  //setup for physical layer
  void addOutput(vector<int> &);
  void addInput(vector<int> &);

  //autosetup
  void autoTransmit();
  void autoReceive();
  void autoSplitInput();
  void autoReadDTMF();
  void autoWriteDTMF();

 
 //Testing
  void testTransmit();
  void printReceived();
  //Transmission
  void TransmitACK(vector<bool>); //Transmits ACK
  void Receive(vector<int>);      //gets split message from splitter, from inputbuffer
  void Transmit(vector<bool>);    //transmits message implement framegen and lower.
  int _timeout;
  bool compareACK(vector<bool>);

  //buffersplitting
  void SplitBuffer();

  //Variables
  mutex _outbufferLock;
  mutex _RMstackLock;
  mutex _TMstackLock;
  mutex _ACKstackLock;
  mutex _splitFramesLock;
  mutex _inbufferLock;

  std::condition_variable m_cv_read;
  std::mutex m_readMutex;
  std::condition_variable m_cv_autoTransmit;
  std::mutex m_autoTransmitMutex;
  std::condition_variable m_cv_autoSplit;
  std::mutex m_autoSplitMutex;

  //modules
  FrameGenerator *_FG;
  Cyclic *_CRChecker;
  bytestuffer *_Stuffer;
  PhysicalLayer *_pLayer;

  //type patterns
  vector<bool> _msgType = {1, 0};
  vector<bool> _ackType = {0, 1};

  //SEQ controllers
  vector<bool> _lastReceivedSeq = {1, 1};              //sequence number of last received msg/last sent ACK
  vector<bool> _currentSeq = {0, 0};

  //IO channels
  vector<int> *_outputBuffer;              //Output to channel
  vector<int> *_inputBuffer;               //Incoming channel NEEDS TO BE "PUSHED BACK" FROM DTMF CODE(APPEND TO END)
  
  //Stacks for internal use
  vector<vector<int>> *_incomingFrames;    //stack with split frames from receivebuffer
  vector<vector<bool>> *_ReceivedACKBuffer;      //stack with seqnr from receivebuffer ACKs
  
  //Processed message buffers
  vector<vector<bool>> *_outgoingMessages; //stack with messages for outputbuffer, ACKs are added directly to outbuffer from receive
  vector<vector<vector<bool>>> *_ReceiveMessageBuffer; //receive buffer for processed msg
  
  //threads
  thread *receiveThread;
  thread *splitThread;
  thread *transmitThread;
  thread *DTMFwriteT;
  thread *DTMFreadT;
  
};

//transmit: transmit->timer, if ACK->good, if no ACK, retransmit
//Receive: msg: CRC check, send ACK if CRC yields no issue
//ACK: change bool

//INPUT BUFFER OUTPUTBUFFER -> VECTOR<VECTOR.. TRANSMIT+RECEIVE FROM VECTOR<VECTOR->ERASE,

#endif
