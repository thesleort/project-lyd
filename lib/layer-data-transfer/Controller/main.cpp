//TESTFILE
#include <iostream>
#include <vector>
#include <thread>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "Controller.h"
int main(int argc, char const *argv[]) {
  Controller controller1(1);
  Controller controller2(1);
  vector<int> outbuf= {15,15,2,4,5,6,15,15};

  vector<int> inbuf= {15,7,2,15,15,15,15,7,8,2,1};

  string c1="Controller 1 ";
  string c2="Controller 2 ";


  controller1.addInput(outbuf);
  controller1.addOutput(inbuf);
  controller2.addInput(inbuf);
  controller2.addOutput(outbuf);

  controller1.printReceived();
  
  controller2.printReceived();
  /* code */

  return 0;
}
