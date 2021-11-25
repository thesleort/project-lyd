#ifndef H_CONTROLLER
#define H_CONTROLLER

#include "Cyclic.h"
#include "bytestuffer.h"
#include "framegen.h"

#include <vector>
#include <mutex>
#include "semaphore.h"
#include <string>
#include <thread>
//add IFNDEF on COUTs to define debugmode
class Controller {
  public:
  //Constructor/deconstructor
  Controller(double);
  ~Controller();

  //setup for physical layer
  void addOutput(vector<int> &);
  void addInput(vector<int> &);

  //Buffer Read/Write/Check
  void write(vector<bool>);
  vector<bool>read();
  bool checkReceive();

  //autosetup
  void autoTransmit();
  void autoReceive();
  void autoSplitInput();


 
 //Testing
  void testTransmit();
  void printReceived();

private:
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
  //modules
  FrameGenerator *_FG = new FrameGenerator();
  Cyclic *_CRChecker = new Cyclic({1, 0, 1, 1, 1});
  bytestuffer *_Stuffer = new bytestuffer({1, 1, 1, 1}, {0, 0, 0, 0});

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
  
};

//transmit: transmit->timer, if ACK->good, if no ACK, retransmit
//Receive: msg: CRC check, send ACK if CRC yields no issue
//ACK: change bool

//INPUT BUFFER OUTPUTBUFFER -> VECTOR<VECTOR.. TRANSMIT+RECEIVE FROM VECTOR<VECTOR->ERASE,

#endif
