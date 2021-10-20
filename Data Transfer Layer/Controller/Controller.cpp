#include "..\Cyclic\Cyclic.h"
#include "..\Protocol\bytestuffer.h"
#include "Controller.h"
#include <vector>
#include <iostream>
#include "windows.h"
#define DATA 0
#define TYPE 1
#define SEQ 2
#define CRC 3

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
vector<bool> seq=_currentSeq; 

vector<bool> frame;//=generateFrame(datac,type,seq,crc)
vector<int> intMsg=_Stuffer->stuff(frame);
//intMsg -> outbuffer

//wait for any ack, can never recieve old ack since we dont send msg without getting an ack

int k=10000;
while(k>0 && !_ACKReceived){//timeout 10s or ACKreceived->continue
Sleep(0.001);
k++;
}
if(!_ACKReceived){//if no ACK is received either the frame or ack was lost or an error occured 
         Transmit(msg); 
} else { //if an ACK has been received, we flip the _currentSeq, which means the next msg we send has a new seqnr
        _currentSeq.flip();
        _ACKReceived=0; //resets ack receive so we dont break anything
}
    //probably not recursive :)


}


void Controller::Receive(vector<bool> in){
//assumes: {data,type,seq,crc};
vector<vector<bool>> msg; // = framesplitter(in)

if(msg.at(TYPE)==_msgType){//if msg is a message, we do a CRC check
    if(_CRChecker->Decode(msg.at(DATA),msg.at(CRC))){//If crc returns no error, we check the sequencenumber
        if(msg.at(SEQ)==_lastRecievedSeq){
        //if seq of msg is same as last message, and CRC shows no error, the last ACK has been lost and we "resend" the ack
        TransmitACK(_lastRecievedSeq);
        } else {
        //if seq number is "new" we transmit an ack and place msg into buffer
        TransmitACK(msg.at(SEQ)); //since we check for any ack and handle redundancy on _lRS  check, ack seqnr is useless(might be usefull later)
        _lastRecievedSeq=msg.at(SEQ);
        _ReceiveMessageBuffer->push_back(msg);//msg->buffer
        } 
    } //if a crc error has occured we just wait for the msg to be resent
    } else if(msg.at(TYPE)==_ackType){
    _ReceivedACKBuffer->push_back(msg.at(SEQ));//is actually useless, since we look for any ACK
    _ACKReceived=1;
}
}

void Controller::autoReceive(){
//Receive(_Stuffer->unstuff(_Stuffer->split(DTMF_in_buf)));

}

void Controller::TransmitACK(vector<bool> seq){
vector<bool> frame;//=generateACKFrame(seq,_ackType)
vector<int> intAck=_Stuffer->stuff(frame);
//intAck -> outbuffer

}