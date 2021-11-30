#include <nlohmann/json.hpp>

#include "libdtmf.h"
#include "packet.h"
#include "roscom.h"

using json = nlohmann::json;

#define ROSCOM_URI "tcp://localhost:1883"

int main(int argc, char *argv[]) {
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;

  RosCommunicator roscom(ROSCOM_URI);
  roscom.connect();
  while (true) {

    if (dtmf->receive(frame) > 0) {
      json jsonMessage;
      float linearVelocity = 0;
      float angularVelocity = 0;
      switch (frame.data[0] ^ DATATYPE_MOVE) {
        case MOVE_STOP:
          angularVelocity = 0;
          linearVelocity = 0;
          break;
        case MOVE_FORWARD:
          linearVelocity = 0.5;
          break;
        case MOVE_BACKWARDS:
          linearVelocity = -0.5;
          break;
        case MOVE_RIGHT90:
          angularVelocity = 0.5;
          break;
        case MOVE_LEFT90:
          angularVelocity = -0.5;
          break;
      }
      jsonMessage = {
        { "linear", {{"x", linearVelocity}, {"y", 0.0}, {"z", 0}}},
        { "angular", {{"x", 0.0}, {"y", 0.0}, {"z", angularVelocity}}}
      };
      roscom.publish_message(jsonMessage);
    }
  }
}
