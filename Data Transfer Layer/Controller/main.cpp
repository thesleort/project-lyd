//TESTFILE
#include <iostream>
#include <vector>
#include "Controller.h"
#include <thread>

int main(int argc, char const *argv[])
{
Controller controller;

vector<int> outbuf;

controller.addOutput(outbuf);
vector<bool> msg={1,1,1,0,1,1,1,1};

controller.Transmit(msg);

    /* code */
    return 0;
}
