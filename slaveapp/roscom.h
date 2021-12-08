#ifndef ROSCOM_H
#define ROSCOM_H

#ifndef TEST_MODE

#include <string>
#include <mqtt/async_client.h>
#include <nlohmann/json.hpp>

#define TOPIC_CMD_VEL "cmd_vel"
#define QOS_1 1

using json = nlohmann::json;

class RosCommunicator {
  public:
    RosCommunicator(std::string address);
    ~RosCommunicator();

    bool connect();

    void publish_message(json j);

  private:
    std::string m_address;
    mqtt::async_client *m_client;
    mqtt::topic *m_topic;
    mqtt::token_ptr m_token;

};
#endif
#endif
