#include "Cyclic\Cyclic.h"
#include "Protocol\bytestuffer.h"
#include "Controller.h"
#include <vector>
#include <iostream>
#include "windows.h"

Controller::Controller(){
    _TransmitMessageBuffer = new vector<bool>;
    _ReceiveMessageBuffer = new vector<vector<vector<bool>>>;
    _ReceivedACKBuffer = new vector<vector<bool>>;
 

    //autoTransmit();
    //autoACK();
}

Controller::~Controller(){
    delete _TransmitMessageBuffer;
    delete _ReceiveMessageBuffer;
    delete _ReceivedACKBuffer;
    delete _CRChecker;
    delete _Stuffer;
}

void Controller::Transmit(vector<bool> msg){
vector<bool> crc=_CRChecker->Encode(msg);
vector<bool> type={1,1};//Data
vector<bool> seq=_currentSeq; //doesnt do anything on simple mode

vector<bool> frame;//=generateFrame(datac,type,seq,crc)
vector<int> intMsg=_Stuffer->stuff(frame);
//intMsg -> outbuffer

bool ACKreceived=0; //flips if ACK is received
int k=10000;
while(k>0 && !ACKreceived){//timeout 10s or ACKreceived->continue
Sleep(0.001);
k++;
}
if(!ACKreceived){//if no ACK is received, retransmit
         Transmit(msg); //if not, either the frame or ack was lost or an error occured    }
} //probably not recursive :)


}


void Controller::Receive(vector<bool> in){
//assumes: {data,type,seq,crc};
vector<vector<bool>> msg; // = framesplitter(in)
if(msg.at(2)==_msgType){
    _ReceiveMessageBuffer->push_back(msg);//msg->buffer
    if(_CRChecker->Decode(msg.at(1),msg.at(4))){
    TransmitACK();
    }
} else if(msg.at(2)==_ackType){
    _ReceivedACKBuffer->push_back(msg.at(3));//
    _ACKReceived=1;
}
}

void Controller::autoReceive(){
//Receive(_Stuffer->unstuff(_Stuffer->split(DTMF_in_buf)));

}