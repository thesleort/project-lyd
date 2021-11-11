#ifndef H_CONTROLLER
#define H_CONTROLLER

#include "..\Cyclic\Cyclic.h"
#include "..\Protocol\bytestuffer.h"
#include "..\FrameGenPlaceholder\framegen.h"

#include <vector>
#include <mutex>
#include "semaphore.h"

class Controller {
  public:
  //Constructor/deconstructor
  Controller();
  ~Controller();

  //setup
  void addOutput(vector<int> &);
  void addInput(vector<int> &);

  //Transmission
  void Transmit(vector<bool>);    //transmits message implement framegen and lower.
  void TransmitACK(vector<bool>); //Transmits ACK
  void Receive(vector<int>);      //gets split message from splitter, from inputbuffer

  //buffersplitting
  void SplitBuffer();

  //autosetup
  void autoTransmit();
  void autoReceive();
  void autoSplitInput();

 
 //Testing
  void testTransmit();
  void printReceived();
  private:
  //Essentials
  bool _ACKReceived = 0;

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

  //buffers
  vector<int> *_outputBuffer;              //for outgoing transmissions
  vector<int> *_inputBuffer;               //for incoming transmissions NEEDS TO BE "PUSHED BACK" FROM DTMF CODE(APPEND TO END)
  vector<vector<int>> *_incomingFrames;    //stack with split frames from receivebuffer
  vector<vector<bool>> *_outgoingMessages; //stack with messages for outputbuffer, ACKs are added directly to outbuffer
  vector<vector<vector<bool>>> *_ReceiveMessageBuffer; //receive buffer for msg
  vector<vector<bool>> *_ReceivedACKBuffer;            //receive buffer for ACK, useless

};

//transmit: transmit->timer, if ACK->good, if no ACK, retransmit
//Receive: msg: CRC check, send ACK if CRC yields no issue
//ACK: change bool

//INPUT BUFFER OUTPUTBUFFER -> VECTOR<VECTOR.. TRANSMIT+RECEIVE FROM VECTOR<VECTOR->ERASE,

#endif
