#include <nlohmann/json.hpp>
#ifndef TEST_MODE
#include <iostream>
#endif

#include "libdtmf.h"
#include "packet.h"
#include "roscom.h"

using json = nlohmann::json;

#ifndef TEST_MODE
#define ROSCOM_URI "tcp://localhost:1883"
#endif

int main(int argc, char *argv[]) {
  DTMF *dtmf = new DTMF();
  DTMFFrame frame;

  #ifndef TEST_MODE
  RosCommunicator roscom(ROSCOM_URI);
  roscom.connect();
  #endif 
  while (true) {

    if (dtmf->receive(frame) > 0) {
      json jsonMessage;
      float linearVelocity = 0;
      float angularVelocity = 0;
      std::cout << "Frame received" << std::endl;
      if (frame.data[0] & DATATYPE_MOVE == DATATYPE_MOVE) {

        switch (frame.data[0] | DATATYPE_MOVE) {
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
        #ifndef TEST_MODE
        jsonMessage = {
          { "linear", {{"x", linearVelocity}, {"y", 0.0}, {"z", 0}}},
          { "angular", {{"x", 0.0}, {"y", 0.0}, {"z", angularVelocity}}}
        };
        roscom.publish_message(jsonMessage);
        #else
        std::cout << "Linear velocity : " << linearVelocity << std::endl;
        std::cout << "Angular velocity: " << angularVelocity << std::endl;
        #endif
      }
    }
  }
}
