#ifndef H_CONTROLLER

#include "..\Cyclic\Cyclic.h"
#include "..\Protocol\bytestuffer.h"
#include "..\FrameGenPlaceholder\framegen.h"

#include <vector>

class Controller{
public:
//Constructor/deconstructor
    Controller();
    ~Controller();

//setup
    void addOutput(vector<int>&);
    void addInput(vector<int>&);
    
//Transmission
    void Transmit(vector<bool>); //transmits message implement framegen and lower.
    void TransmitACK(vector<bool>); //Transmits ACK
    void Receive(vector<bool>);  //gets split message from splitter, from inputbuffer

    //  void autoTransmit();
  //  void autoReceive();
    //void autoACK();

private:
    vector<vector<bool>> * _ReceivedACKBuffer; //receive buffer for ACK
    vector<vector<vector<bool>>> * _ReceiveMessageBuffer; //receive buffer for msg
    vector<bool> _lastRecievedSeq={1,1};//sequence number of last received msg/last sent ACK 
    vector<bool> _currentSeq={0,0};
    bool _ACKReceived=0;
//modules
    FrameGenerator * _FG =new FrameGenerator();
    Cyclic * _CRChecker =new Cyclic({1,0,1,1,1});
    bytestuffer * _Stuffer=new bytestuffer({1,1,1,1},{0,0,0,0});
   
//type patterns
    vector<bool> _msgType ={1,0}; 
    vector<bool> _ackType ={0,1};

//buffers
    vector<int> * _outputBuffer;
    vector<int> * _inputBuffer;
    
};

//transmit: transmit->timer, if ACK->good, if no ACK, retransmit
//Receive: msg: CRC check, send ACK if CRC yields no issue
    //ACK: change bool


#endif