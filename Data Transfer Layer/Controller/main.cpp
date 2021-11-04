//TESTFILE
#include <iostream>
#include <vector>
#include "Controller.h"
#include "windows.h"
#include <thread>

int main(int argc, char const *argv[])
{
Controller controller1;
Controller controller2;
vector<int> outbuf={0,15,12,14,15,15,15,15,15,15};

vector<int> inbuf={15};


controller1.addInput(outbuf);
controller1.addOutput(inbuf);
controller2.addInput(inbuf);
controller2.addOutput(outbuf);

vector<bool> msg={1,1,1,0,1,1,1,1};

thread tthread(&Controller::Transmit,&controller2,msg);
Sleep(1000);
thread tt2hread(&Controller::Transmit,&controller2,msg); //simulates lost ACK, works
thread Receive1thread(&Controller::autoReceive,&controller1);
thread Receive2thread(&Controller::autoReceive,&controller2);
thread Split1thread(&Controller::autoSplitInput,&controller1);
thread Split2thread(&Controller::autoSplitInput,&controller2);
Sleep(5000);
vector<bool> msg2={1,1,0,1,1,1,1,0,1,1,1,1};
thread t2thread(&Controller::Transmit,&controller1,msg2);
Sleep(5000);
thread t3thread(&Controller::Transmit,&controller1,msg2);
Receive1thread.join();
Receive2thread.join();
Split1thread.join();
Split2thread.join();

tt2hread.join();
tthread.join();

t2thread.join();

t3thread.join();
    /* code */
    return 0;
}
