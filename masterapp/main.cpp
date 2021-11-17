#include "packet.h"
#include "libdtmf.h"

int main(int argc, char *argv[]) {

  DTMF *dtmf = new DTMF();
  DTMFFrame frame;

  frame.data_size = 1;
  frame.data = new uint8_t[frame.data_size];
  
  if (/*forward*/) {
    frame.data[0] = DATATYPE_MOVE | MOVE_FORWARD;

    dtmf->transmit(frame);
  }
}
