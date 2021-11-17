#include "libdtmf.h"
#include "packet.h"

int main(int argc, char *argv[]) {
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;
  while (true) {

    if (dtmf->receive(frame) > 0) {
      switch (frame.data[0] ^ DATATYPE_MOVE) {
        case MOVE_STOP:
          break;
        case MOVE_FORWARD:
          break;
        case MOVE_BACKWARDS:
          break;
        case MOVE_RIGHT90:
          break;
        case MOVE_LEFT90:
          break;
      }
    }
  }
}
