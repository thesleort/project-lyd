#include "packet.h"
#include "libdtmf.h"

#include <iostream>
#include <ncurses.h>
#include <unistd.h>

#include <unistd.h>
#include <condition_variable>
#include <mutex>

int main(void) {
  std::cout << "Starting" << std::endl;
  DTMF *dtmf = new DTMF();
  DTMFPacket packet;
  std::cout << "DTMF started" << std::endl;
  packet.data_size = 1;
  packet.data = new uint8_t(packet.data_size);

  // frame.data[0] = DATATYPE_MOVE | MOVE_STOP;
  // dtmf->transmit(frame);
  // // dtmf->transmit(frame);

  // std::mutex mutex;
  // std::unique_lock<std::mutex> lock(mutex);
  // std::condition_variable cv;
  // cv.wait(lock);
  // while(true) {

  // }

  initscr();

  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);

  int flag;

  std::cout << "Press any key to start" << std::endl;
  flag = std::cin.get();

  bool inputLoop = true;
  while (inputLoop) {
    // Assume a key has been pressed.
    bool keypress = true;
    int c = getch();
    
    switch (c) {
    case 114: // R
      packet.data[0] = DATATYPE_INFO | INFO_VELOCITY;
      packet.packet_response_type = DATA_REQUIRE_RESPONSE;
      // clear();
      printw("Requesting velocity information");
      break;
    case 119: // W
      packet.data[0] = DATATYPE_MOVE | MOVE_FORWARD;
      clear();
      printw("w");
      break;
    case 115: // S
      packet.data[0] = DATATYPE_MOVE | MOVE_BACKWARDS;
      clear();
      printw("s");
      break;
    case 97:  // A
      packet.data[0] = DATATYPE_MOVE | MOVE_LEFT90;
      clear();
      printw("a");
      break;
    case 100: // D
      packet.data[0] = DATATYPE_MOVE | MOVE_RIGHT90;
      clear();
      printw("d");
      break;
    case 32:  // Spacebar
      packet.data[0] = DATATYPE_MOVE | MOVE_STOP;
      clear();
      printw("spacebar");
      break;
    case 27: // Escape
      inputLoop = false;
      endwin();
      std::cout << "Program ended"<< std::endl;    
    default:
      // clear();
      // If none of the keys below (except escape: 27), change keypress state.
      keypress = false;
      break;
    };

    // Send message if key was pressed and there are no pending responses.
    if (keypress) {
      dtmf->transmit(packet);
      packet.packet_response_type = DATA_NO_RESPONSE;
    }
    if (dtmf->receive(packet, false) > 0) {
      printw("Linear velocity: %f - Angular velocity: %f", (float) ((int8_t) packet.data[0]) / 10.0, (float) ((int8_t) packet.data[1]) / 2.0);
      delete packet.data;
      packet.data_size = 1;
      packet.data = new uint8_t(packet.data_size);
    }
  }

  return 0;
}
