
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define TEST_SUITE

#include "libdtmf.h"

void socket1(DTMF*dtmfClass, std::atomic<bool> &runFlag);
void socket2(DTMF*dtmfClass, std::atomic<bool> &runFlag);

int main(int argc, char *argv[]) {
  std::atomic<bool> runFlag(true);
  std::vector<DTMFFrame> buffer1;
  std::vector<DTMFFrame> buffer2;
  DTMF *dtmfSocket1 = new DTMF(&buffer1, &buffer2);
  DTMF *dtmfSocket2 = new DTMF(&buffer2, &buffer1);

  std::thread socketThread1(&socket1, dtmfSocket1, std::ref(runFlag));
  std::thread socketThread2(&socket2, dtmfSocket2, std::ref(runFlag));
  do {
   std::cout << '\n' << "Press a key to close...";
  } while (std::cin.get() != '\n');
  runFlag = false;
  socketThread1.join();
  socketThread2.join();
}

void socket1(DTMF *dtmfClass, std::atomic<bool> &runFlag) {
  std::cout << "Creating socket1" << std::endl;
  DTMFFrame frame;
  frame.sizeBytes = 10;
  dtmfClass->transmit(frame);
  frame.sizeBytes = 100;
  dtmfClass->transmit(frame);
  std::cout << "Socket 1: frame sent (" << frame.sizeBytes << ")" << std::endl;
}

void socket2(DTMF *dtmfClass, std::atomic<bool> &runFlag) {
  std::cout << "Creating socket2" << std::endl;
  DTMFFrame *frame = new DTMFFrame;
  while (runFlag) {
    if (dtmfClass->receive(*frame, false) > 0) {
      std::cout << "Socket 2: frame received (" << frame->sizeBytes << ")" << std::endl;
    }
  }
  delete frame;
}
