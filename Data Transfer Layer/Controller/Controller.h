#include "Cyclic\Cyclic.h"
#include "Protocol\bytestuffer.h"
//include "framegenerator", layer below,
#include <vector>

class Controller{
public:
//stop and wait with sequencing
    Controller();
    ~Controller();

    void addMessage(vector<bool>); //data

    void Transmit(vector<bool>); //transmits message implement framegen and lower.
    void TransmitACK(); //Transmits ACK
    void Receive(vector<bool>);  //gets split message from splitter, from inputbuffer

    void autoTransmit();
    void autoReceive();
    //void autoACK();

private:
    vector<bool> * _TransmitMessageBuffer; //transmit buffer for msg
    vector<vector<bool>> * _ReceivedACKBuffer; //receive buffer for ACK
    vector<vector<vector<bool>>> * _ReceiveMessageBuffer; //receive buffer for msg
    //vector<int> _lastMessage={-1}; //last transmitted msg, disabled for simple S&W
    //vector<bool> _lastSeq={1,1};sequence number of -||-, disabled for simple S&W
    vector<bool> _currentSeq={0,0};
    //FrameGenerator _FG();
    bool _ACKReceived=0;
    Cyclic * _CRChecker =new Cyclic({1,0,1,1,1});
    bytestuffer * _Stuffer=new bytestuffer({1,1,1,1},{0,0,0,0});
    vector<bool> _msgType ={1,0}; //type pattens
    vector<bool> _ackType ={0,1};

};

//transmit: transmit->timer, if ACK->good, if no ACK, retransmit
//Receive: msg: CRC check, send ACK if CRC yields no issue
    //ACK: change bool