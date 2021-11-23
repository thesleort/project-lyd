#include <iostream>

#include "roscom.h"

RosCommunicator::RosCommunicator(std::string address) {
  m_address = address;
  const std::string TOPIC { TOPIC_CMD_VEL };
  m_client = new mqtt::async_client(m_address, "");

  m_topic = new mqtt::topic(*m_client, TOPIC, QOS_1);
}

RosCommunicator::~RosCommunicator() { 
  delete m_client;
  delete m_topic;
}

bool RosCommunicator::connect() {
  try {
    std::cout << std::endl << "Connecting..." << std::endl;
    m_client->connect()->wait();
    std::cout << "...OK" << std::endl;
  } catch (const mqtt::exception &e) {
    std::cerr << e << std::endl;
    return false;
  }
  return true;
}

void RosCommunicator::publish_message(json j) {
  try {
    m_token = m_topic->publish(j.dump());
    m_token->wait();
  } catch (const mqtt::exception &e) {
    std::cerr << e << std::endl;
    return;
  }
}
