#include "..\Cyclic\Cyclic.h"
#include "..\Protocol\bytestuffer.h"
#include "..\FrameGenPlaceholder\framegen.h"
#include "Controller.h"
#include <vector>
#include <iostream>
#include "windows.h"
#define DATA 0
#define TYPE 1
#define SEQ 2
#define CRC 3
//-------------Constructor/Deconstuctor-----------
Controller::Controller(){
    _ReceiveMessageBuffer = new vector<vector<vector<bool>>>;
    _ReceivedACKBuffer = new vector<vector<bool>>;
    _incomingFrames = new vector<vector<int>>;
    _outgoingMessages = new vector<vector<bool>>; 

 
}

Controller::~Controller(){
    delete _ReceiveMessageBuffer;
    delete _ReceivedACKBuffer;
    delete _CRChecker;
    delete _Stuffer;
    delete _incomingFrames;
    delete _outgoingMessages;
}

//-------------Setup---------
void Controller::addOutput(vector<int>& out){
_outputBuffer=&out;
}


void Controller::addInput(vector<int>& in){
_inputBuffer=&in;
}

//------------Transmission------------

void Controller::Transmit(vector<bool> msg){
vector<bool> crc=_CRChecker->Encode(msg);
vector<bool> type=_msgType;
vector<bool> seq=_currentSeq; 
vector<vector<bool>> parts={msg,type,seq,crc};
vector<bool> frame=_FG->generateFrame(parts);//gen frame

vector<int> intMsg=_Stuffer->stuff(frame); //stuff
cout << "sent msg in integers" << endl;
for(int n : intMsg){
    cout << n << " "; 
}
    cout<< endl;

cout << "--------------" << endl;
for(int n : intMsg){
    _outputBuffer->push_back(n); //values inserted in order on end of buffer, buffer is read from index 0 on split
}
//_outputBuffer->insert(_outputBuffer->begin(),intMsg.begin(),intMsg.end()); //insert on buffer

//wait for any ack, can never recieve old ack since we dont send msg without getting an ack

int k=10000000;

//--printoutbuffer--

//--printoutbuffer--

while(k>0 && !_ACKReceived){//timeout 10s or ACKreceived->continue
Sleep(0.03);
k--;
}
if(!_ACKReceived){//if no ACK is received either the frame or ack was lost or an error occured 
        Transmit(msg); 
} else { //if an ACK has been received, we flip the _currentSeq, which means the next msg we send has a new seqnr
        _currentSeq.flip();
        cout << "ack receive registrered by transmit" << endl;
        _ACKReceived=0; //resets ack receive so we dont break anything
}

}


void Controller::Receive(vector<int> in){
//assumes: {data,type,seq,crc};
cout << "Received() message:" << endl;
for(int n:in){
    cout <<n;
}
cout << endl;
cout << "--------------"<<endl;
 
vector<vector<bool>> msg=_FG->splitFrame(_Stuffer->unstuff(in)); 

if(msg.at(TYPE)==_msgType){//if msg is a message, we do a CRC check
    cout << "msg received" << endl;

    if(_CRChecker->Decode(msg.at(DATA),msg.at(CRC))){//If crc returns no error, we check the sequencenumber
        if(msg.at(SEQ)==_lastRecievedSeq){
        //if seq of msg is same as last message, and CRC shows no error, the last ACK has been lost and we "resend" the ack
            TransmitACK(_lastRecievedSeq);
            cout << "ACK Retransmitted" <<endl;
            } else {
            //if seq number is "new" we transmit an ack and place msg into buffer
            TransmitACK(msg.at(SEQ)); //since we check for any ack and handle redundancy on _lRS  check, ack seqnr is useless(might be usefull later)
            _lastRecievedSeq=msg.at(SEQ);
            cout << "new ACK transmitted" <<endl;
            _ReceiveMessageBuffer->push_back(msg);//msg->buffer
            
            } 
        }  //if a crc error has occured we just wait for the msg to be resent
    } else if(msg.at(TYPE)==_ackType){
    cout << "ACK received" << endl;
    _ReceivedACKBuffer->push_back(msg.at(SEQ));//is actually useless, since we look for any ACK
    _ACKReceived=1;
    cout << "-----------" << endl;
}
}


void Controller::TransmitACK(vector<bool> seq){
vector<bool> decoydata={0,1,1,0};
vector<vector<bool>> parts={decoydata,_ackType,seq};
vector<bool> frame=_FG->generateACKFrame(parts);
vector<int> intAck=_Stuffer->stuff(frame);
cout << "ACK frame stuffed: " << endl;
for(int n:intAck){
    cout << n;
} 
cout << endl;
cout << "---------" << endl;

for(int n:intAck){
    _outputBuffer->push_back(n);
}

//_outputBuffer->insert(_outputBuffer->begin(),intAck.begin(),intAck.end());

}



void Controller::autoTransmit(){
    while(1){
        while(_outgoingMessages->size()>0){
        vector<bool> msg=_outgoingMessages->at(0);//first element on outgoing frames is msg
        _outgoingMessages->erase(_outgoingMessages->begin()); //first msg on stack is
        Transmit(msg);
        }
    }
}

void Controller::autoReceive(){
    while(1){
        while(_incomingFrames->size()>0){
        vector<int> frame=_incomingFrames->at(0);
        _incomingFrames->erase(_incomingFrames->begin()); 
        Receive(frame);
        }
    }
}

void Controller::autoSplitInput(){
    while(1){
        while(_inputBuffer->size()>2){
            SplitBuffer();
        }
    }
}


void Controller::SplitBuffer(){
int _flagI=_Stuffer->getFlag();
int _etcI=_Stuffer->getEtc();
int size= _inputBuffer->size();
vector<int> frame;
int frameStart=-1; //starting flag position
try{
for(int i=0;i<size-1;i++){//look at size -1 values since we check i+1 to determine flag
    if(_inputBuffer->at(i)==_flagI){//if flag is found it might be a starting flag
        if(i==0&&(_inputBuffer->at(i+1)==4||_inputBuffer->at(i+1)==7||_inputBuffer->at(i+1)==8||_inputBuffer->at(i+1)==11)){ 
            frameStart=0;//if it is the buffer beginning, it is a flag if the next value is a combination of type + seq
            break;
        } else if(_inputBuffer->at(i-1)!=_etcI){//else if previous int is not etc,
            if(_inputBuffer->at(i+1)==4||_inputBuffer->at(i+1)==7||_inputBuffer->at(i+1)==8||_inputBuffer->at(i+1)==11){
                frameStart=i;//we have a starting flag if the next int is a valid type+seq
                break;
            }
        } //if not we keep looking (if frame can naturally consist of etc->flag, this will break)
    }
}} 
catch(const exception& e){
    cout << "Framestart error" << e.what()  << endl;
        cout << "size: " << size <<"actual size: " << _inputBuffer->size() << endl;
    for(int i:*_inputBuffer){
        cout << i;
    }
    exit(0);
}
int frameStop=-1;
try{
   // cout << "Framestart +1 - size: " << frameStart+1 - size << endl;
for(int i=frameStart+1;i<size;i++){//Starts at +1 since we do not want the starting flag
    if(_inputBuffer->at(i)==_flagI&&_inputBuffer->at(i-1)!=_etcI){//the next flag we find without an etc will be the stop
            frameStop=i;
            break;
    }
}
} catch(const exception& e){
    cout << "Framestop error" << e.what();
    cout << "size: " << size <<"actual size: " << _inputBuffer->size() << endl;
    for(int i:*_inputBuffer){
        cout << i;
    }
    cout << endl;

}

if(frameStart!=-1&&frameStop!=-1){
    cout << "start: " << frameStart << "stop: "<< frameStop << endl;
for(int i=frameStart;i<=frameStop;i++){
    
    frame.push_back(_inputBuffer->at(i)); //frame is created based on start and stop values
}
_incomingFrames->push_back(frame);
_inputBuffer->erase(_inputBuffer->begin(),_inputBuffer->begin()+frameStop+1);  //fixes program. in erase range with begin(), the amount of erased elements is equal to It_last-It_first(so if framestop without +1 is used and framestop index is 2, only 2 elements are deleted from input)

}
}