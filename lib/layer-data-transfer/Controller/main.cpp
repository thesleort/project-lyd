//TESTFILE
#include <iostream>
#include <vector>
#include <thread>

#include "Controller.h"

int main(int argc, char const *argv[]) {
  Controller controller1;
  Controller controller2;
  vector<int> outbuf;

  vector<int> inbuf;

  controller1.addInput(outbuf);
  controller1.addOutput(inbuf);
  controller2.addInput(inbuf);
  controller2.addOutput(outbuf);

  vector<bool> msg = {1, 1, 1, 0, 1, 1, 1, 1};

  thread tthread(&Controller::Transmit, &controller2, msg);
  thread Receive1thread(&Controller::autoReceive, &controller1);
  thread Receive2thread(&Controller::autoReceive, &controller2);
  thread Split1thread(&Controller::autoSplitInput, &controller1);
  thread Split2thread(&Controller::autoSplitInput, &controller2);
  // Sleep(10000);
  vector<bool> msg2 = {1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1};
  thread t2thread(&Controller::Transmit, &controller1, msg2);
  Receive1thread.join();
  Receive2thread.join();
  Split1thread.join();
  Split2thread.join();

  tthread.join();

  t2thread.join();
  /* code */
  return 0;
}
