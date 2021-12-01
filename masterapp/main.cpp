#include "packet.h"
#include "libdtmf.h"

#include <iostream>
#include <ncurses.h>
#include <unistd.h>

#include <unistd.h>

int main(void) {
  std::cout << "Starting" << std::endl;
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;
  std::cout << "DTMF started" << std::endl;
  frame.data_size = 1;
  frame.data = new uint8_t(frame.data_size);

  frame.data[0] = DATATYPE_MOVE | MOVE_FORWARD;
  std::cout << "data: " << unsigned(frame.data[0]) << std::endl;
  dtmf->transmit(frame);
  while(true) {

  }
  // initscr();

  // cbreak();
  // noecho();
  //   nodelay(stdscr, TRUE);
  // scrollok(stdscr, TRUE);

  // int flag;

  // std::cout << "Press any key to start" << std::endl;
  // flag = std::cin.get();

  // bool inputLoop = true;
  // while (inputLoop) {

    
  //   int c = getch();
  //   clear();
    
  //   switch (c) {
  //   case 119:
  //     frame.data[0] = DATATYPE_MOVE | MOVE_FORWARD;
  //     printw("w");
  //     break;
  //   case 115:
  //     frame.data[0] = DATATYPE_MOVE | MOVE_BACKWARDS;
  //     printw("s");
  //     break;
  //   case 97:
  //     frame.data[0] = DATATYPE_MOVE | MOVE_LEFT90;
  //     printw("a");
  //     break;
  //   case 100:
  //     frame.data[0] = DATATYPE_MOVE | MOVE_RIGHT90;
  //     printw("d");
  //     break;
  //   case 32:
  //     frame.data[0] = DATATYPE_MOVE | MOVE_STOP;
  //     printw("spacebar");
  //     break;
  //   case 27:
  //     inputLoop = false;
  //     endwin();
  //     std::cout << "Program ended"<< std::endl;
  //     break;
  //   default:
  //     break;
  //   };

  //   dtmf->transmit(frame);
  // }

  return 0;
}
