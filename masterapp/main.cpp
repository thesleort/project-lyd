#include "packet.h"
#include "libdtmf.h"

#include <iostream>
#include <curses.h>
#include <unistd.h>

int main(void)
{
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;

  frame.data_size = 1;
  frame.data = new uint8_t[frame.data_size];

    initscr();

    cbreak();
    noecho();
    // nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    // string Svalue;
    // std::cin>>Svalue;
    // std::cout << Svalue;

    // if (Svalue.compare("1"))
    // {
      while (true) {
                switch (getch()) {
                  case 119: 
                    frame.data[0] = DATATYPE_MOVE | MOVE_FORWARD;
                    std::cout <<"w"<<std::endl;
                    break;
                  case 115:
                    frame.data[0] = DATATYPE_MOVE | MOVE_BACKWARDS;
                    std::cout <<"s"<<std::endl;
                    break;
                  case 97:
                    frame.data[0] = DATATYPE_MOVE | MOVE_LEFT90;
                    std::cout <<"a"<<std::endl;
                    break;
                  case 100:
                    frame.data[0] = DATATYPE_MOVE | MOVE_RIGHT90;
                    std::cout <<"d"<<std::endl;
                    break;
                  case 32:
                    frame.data[0] = DATATYPE_MOVE | MOVE_STOP;
                    std::cout <<"spacebar"<<std::endl;
                    break;
                  default:
                    break;
                };
                dtmf->transmit(frame);
                
        }
    // }else if (Svalue.compare("2"))
    // {
    //   std::cout<<"ending program"<< std::endl;

    // }else if(false)
    // {
    //   std::cout<<"did not compute"<< std::endl;

    // }
    
    
    
      return 0;
}

// int main(int argc, char *argv[]) {

  
  // if (/*Forward*/) {
  //   frame.data[0] = DATATYPE_MOVE | MOVE_FORWARD;

  // }else if(/*Backwards*/){
  //   frame.data[0] = DATATYPE_MOVE | MOVE_BACKWARDS;

  // }else if(/*Right 90 degrees*/){
  //   frame.data[0] = DATATYPE_MOVE | MOVE_RIGHT90;

  // }else if(/*Left 90 degrees*/){
  //   frame.data[0] = DATATYPE_MOVE | MOVE_LEFT90;

  // }else if(/*Stop*/){
  //   frame.data[0] = DATATYPE_MOVE | MOVE_STOP;

  // }
  // dtmf->transmit(frame);

//}
