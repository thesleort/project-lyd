#include <nlohmann/json.hpp>
#include <iostream>

#include "libdtmf.h"
#include "packet.h"
#include "roscom.h"

using json = nlohmann::json;

#ifndef TEST_MODE
#define ROSCOM_URI "tcp://localhost:1883"
#endif

int main(int argc, char *argv[]) {
  DTMF *dtmf = new DTMF();
  DTMFPacket packet;

#ifndef TEST_MODE
  RosCommunicator roscom(ROSCOM_URI);
  roscom.connect();
#endif
  float linearVelocity = 0;
  float angularVelocity = 0;
  while (true) {

    if (dtmf->receive(packet) > 0) {
      json jsonMessage;
      std::cout << "Frame received " << unsigned(packet.data[0]) << std::endl;
      switch (packet.packet_response_type) {
        case DATA_NO_RESPONSE: {
          if ((packet.data[0] & DATATYPE_MOVE) == DATATYPE_MOVE) {
            switch (packet.data[0] & (~DATATYPE_MOVE)) {
            case MOVE_STOP:
              angularVelocity = 0;
              linearVelocity = 0;
              break;
            case MOVE_FORWARD:
              linearVelocity += 0.02;
              break;
            case MOVE_BACKWARDS:
              linearVelocity += -0.02;
              break;
            case MOVE_RIGHT90:
              angularVelocity += 0.5;
              break;
            case MOVE_LEFT90:
              angularVelocity += -0.5;
              break;
            }
            #ifndef TEST_MODE
            jsonMessage = {
                {"linear", {{"x", linearVelocity}, {"y", 0.0}, {"z", 0}}},
                {"angular", {{"x", 0.0}, {"y", 0.0}, {"z", angularVelocity}}}};
            roscom.publish_message(jsonMessage);
            #else
            std::cout << "Linear velocity : " << linearVelocity << std::endl;
            std::cout << "Angular velocity: " << angularVelocity << std::endl;
            #endif
          }
        }
        break;
        case DATA_REQUIRE_RESPONSE: {
          std::cout << "require response" << std::endl;
          if ((packet.data[0] & DATATYPE_INFO) == DATATYPE_INFO) {
            std::cout << unsigned(packet.data[0] & DATATYPE_INFO) << ", " << unsigned(DATATYPE_INFO) << ", " << unsigned(packet.data[0] & (~DATATYPE_INFO)) << std::endl;
            switch (packet.data[0] & (~DATATYPE_INFO)) {
              case INFO_VELOCITY: {
                delete packet.data;
                int8_t linVel = linearVelocity * 10;
                int8_t angVel = angularVelocity * 2;
                packet.data = new uint8_t(2);
                packet.data_size = 2;
                packet.data[0] = linVel;
                packet.data[1] = angVel;
                packet.packet_response_type = DATA_RESPONSE;
                dtmf->transmit(packet);
              }
            }
          }
        }
      }
    }
  }
}
