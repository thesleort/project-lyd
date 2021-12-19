
#include "libdtmf.h"

#include <unistd.h>
#include <iostream>
#include <chrono>

/**
 * @brief Construct a new DTMF::DTMF object
 * 
 */
DTMF::DTMF() {
  m_receiveBuffer = new std::vector<DTMFPacket>();
  m_transmitBuffer = new std::vector<DTMFPacket>();

  m_controller = new Controller(2); // 2 is number of seconds to wait

  m_stopFlag = false;

  m_transmitThread = new std::thread(&DTMF::transmitter, this, std::ref(m_stopFlag));
  m_receiveThread = new std::thread(&DTMF::receiver, this, std::ref(m_stopFlag));
}

/**
 * @brief Destroy the DTMF::DTMF object
 * 
 */
DTMF::~DTMF() {
  m_stopFlag = true;

  m_transmitThread->join();
  m_receiveThread->join();

  delete m_controller;
  delete m_transmitThread;
  delete m_receiveThread;
  delete m_receiveBuffer;
  delete m_transmitBuffer;
}

/**
 * @brief Transmits the accompanied packet
 * 
 * @param packet Reference to the packet that has to be sent.
 * @param blocking TODO: Wait until message has been sent.
 */
void DTMF::transmit(DTMFPacket packet, bool blocking) {
  m_transmitBufferMutex.lock();
  m_transmitBuffer->push_back(packet);
  m_transmitBufferMutex.unlock();
  m_cv_transmitted.notify_all();
}

/**
 * @brief Returns the earliest packet still available on the buffer.
 * Acts as a FIFO queue.
 * 
 * @param packet A reference to the location of the packet
 * @param blocking Wait until a packet exists or not
 * 
 * @return const uint16_t The size of the packet data in bytes.
 */
const uint16_t DTMF::receive(DTMFPacket &packet, bool blocking) {
  u_int16_t frameSize = 0;
  do {
    if (m_receiveBufferMutex.try_lock()) {
      if (m_receiveBuffer->size() > 0) {
        packet = m_receiveBuffer->front();
        frameSize = packet.data_size;
        m_receiveBuffer->erase(m_receiveBuffer->begin());
        blocking = false;
      }
      m_receiveBufferMutex.unlock();
    }
  } while (blocking);
  return frameSize;
}

const bool DTMF::isWaitingResponse() {
  return m_waitingResponse;
}

/**
 * @brief Thread that will send everything one-by-one in the m_transmitBuffer.
 * To add to the transmit buffer simply use DTMF::transmit(...).
 * 
 * @param cancellation_token Stops transmit loop if set to true.
 */
void DTMF::transmitter(std::atomic<bool> &cancellation_token) {
  while (!cancellation_token) {
    std::unique_lock<std::mutex> lock(m_transmitMutex);
    m_cv_transmitted.wait(lock);
    while (m_transmitBuffer->size() > 0) {

      if (m_transmitBufferMutex.try_lock()) {
        m_controller->write(generateBooleanFrame(m_transmitBuffer->at(0)));
        // This postpones all other messages until a response message has been received.
        if (m_transmitBuffer->at(0).packet_response_type == DATA_REQUIRE_RESPONSE) {
          std::unique_lock<std::mutex> lock(m_waitingResponseMutex);
          m_waitingResponse = true;
          m_cv_waitingResponse.wait(lock);
        }
        m_transmitBuffer->erase(m_transmitBuffer->begin());
        m_transmitBufferMutex.unlock();

      }
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

/**
 * @brief Thread that will add received messages to the m_receiveBuffer.
 * Too read what has been received use DTMF::receive(...).
 * 
 * @param cancellation_token Stop receive loop if set to true.
 */
void DTMF::receiver(std::atomic<bool> &cancellation_token) {
  while (!cancellation_token) {
    std::vector<bool> readData;
    DTMFPacket packet;
    readData = m_controller->read();
    if (readData.size() > 0) {
      packet = convertBoolVectorToPacket(readData);

      // If it has received the response to a request, it will notify the
      // transmit thread to allow for new messages to be sent.
      if (packet.packet_response_type == DATA_RESPONSE) {
        m_cv_waitingResponse.notify_all();
        m_waitingResponse = false;
      }
      m_receiveBufferMutex.lock();
      m_receiveBuffer->push_back(packet);
      m_receiveBufferMutex.unlock();
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

std::vector<bool> DTMF::generateBooleanFrame(DTMFPacket &packet) {
  std::vector<bool> boolDataVector;
  // Add the data response type in the first 4 bits of the packet to be sent.
  uint8_t bitmask = BIT_3;
  for (unsigned i = 0; i < sizeof(uint8_t) * 4; i++) {
    if ((packet.packet_response_type & bitmask) == bitmask) {
      boolDataVector.push_back(true);
    } else {
      boolDataVector.push_back(false);
    }
    bitmask = bitmask >> 1;
  }

  for (unsigned i = 0; i < packet.data_size; i++) {
    uint8_t bitmask = BIT_7;
    for (unsigned bit = 0; bit < sizeof(uint8_t) * 8; bit++) { // 8 is the size of a byte in bits

      // Checks if the current bit is set or not.
      if ((packet.data[i] & bitmask) == bitmask) {
        boolDataVector.push_back(true);
      } else {
        boolDataVector.push_back(false);
      }

      bitmask = bitmask >> 1;
    }
  }
  return boolDataVector;
}

DTMFPacket DTMF::convertBoolVectorToPacket(std::vector<bool> boolFrame) {
  DTMFPacket packet;
  packet.data_size = unsigned(boolFrame.size() / (sizeof(uint8_t) * 8));
  packet.data = new uint8_t(packet.data_size);
  unsigned super_index = 0;

  uint8_t responseType = DATA_NO_RESPONSE;
  for (int bit = 3; bit >= 0; --bit) {
    responseType |= short(boolFrame.at(super_index)) << bit;
    super_index++;
  }
  packet.packet_response_type = responseType; 

  for (unsigned i = 0; i < packet.data_size; i++) {
    packet.data[i] = 0;
    for (int bit = (sizeof(uint8_t) * 8) - 1; bit >= 0; --bit) {
      packet.data[i] |= short(boolFrame.at(super_index)) << bit;
      super_index++;
    }
  }

  return packet;
}

#ifdef TEST_SUITE

/**
 * @brief Construct a new DTMF::DTMF object. This one is used for testing
 * 
 * @param transmitBuffer 
 * @param receiveBuffer 
 */
DTMF::DTMF(std::vector<DTMFPacket> *transmitBuffer, std::vector<DTMFPacket> *receiveBuffer) {
  m_transmitBuffer = transmitBuffer;
  m_receiveBuffer = receiveBuffer;

  m_stopFlag = false;

  m_transmitThread = new std::thread(&DTMF::transmitter, this, std::ref(m_stopFlag));
  m_receiveThread = new std::thread(&DTMF::receiver, this, std::ref(m_stopFlag));
}

#endif
