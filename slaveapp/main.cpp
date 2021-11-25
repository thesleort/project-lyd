#include <nlohmann/json.hpp>

#include "libdtmf.h"
#include "packet.h"
#include "roscom.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;

  RosCommunicator roscom("tcp://localhost:1883");
  roscom.connect();
  while (true) {

    if (dtmf->receive(frame) > 0) {
      json jsonMessage;
      float linearVelocity = 0;
      float angularVelocity = 0;
      switch (frame.data[0] ^ DATATYPE_MOVE) {
        case MOVE_STOP:
          break;
        case MOVE_FORWARD:
          angularVelocity = 0.5;
          break;
        case MOVE_BACKWARDS:
          break;
        case MOVE_RIGHT90:
          break;
        case MOVE_LEFT90:
          break;
      }
      jsonMessage = {
        { "linear", {{"x", 0.0}, {"y", 0.0}, {"z", 0}}},
        { "angular", {{"x", 0.0}, {"y", 0.0}, {"z", 0}}}
      };
      roscom.publish_message(jsonMessage);
    }
  }
}
