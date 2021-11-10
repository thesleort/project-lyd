
#include <thread>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define TEST_SUITE

#include "libdtmf.h"

void socket1(DTMF*dtmfClass);
void socket2(DTMF*dtmfClass);

int main(int argc, char *argv[]) {
  std::vector<DTMFFrame> buffer1;
  std::vector<DTMFFrame> buffer2;
  DTMF *dtmfSocket1 = new DTMF(&buffer1, &buffer2);
  DTMF *dtmfSocket2 = new DTMF(&buffer2, &buffer1);

  std::thread socketThread1(&socket1, dtmfSocket1);
  std::thread socketThread2(&socket2, dtmfSocket2);
  do {
   std::cout << '\n' << "Press a key to close...";
  } while (std::cin.get() != '\n');
}

void socket1(DTMF *dtmfClass) {
  std::cout << "Creating socket1" << std::endl;
}

void socket2(DTMF *dtmfClass) {
  std::cout << "Creating socket2" << std::endl;
}
