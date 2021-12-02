
#include "libdtmf.h"

#include <unistd.h>
#include <iostream>
#include <chrono>

/**
 * @brief Construct a new DTMF::DTMF object
 * 
 */
DTMF::DTMF() {
  m_receiveBuffer = new std::vector<DTMFFrame>();
  m_transmitBuffer = new std::vector<DTMFFrame>();

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

  delete m_receiveBuffer;
  delete m_transmitBuffer;
}

/**
 * @brief Transmits the accompanied frame
 * 
 * @param frame Reference to the frame that has to be sent.
 * @param blocking TODO: Wait until message has been sent.
 */
void DTMF::transmit(DTMFFrame frame, bool blocking) {
  m_transmitBufferMutex.lock();
  m_transmitBuffer->push_back(frame);
  m_transmitBufferMutex.unlock();
}

/**
 * @brief Returns the earliest frame still available on the buffer.
 * Acts as a FIFO queue.
 * 
 * @param frame A reference to the location of the frame
 * @param blocking Wait until a frame exists or not
 * 
 * @return const DTMFFrame 
 */
const uint16_t DTMF::receive(DTMFFrame &frame, bool blocking) {
  u_int16_t frameSize = 0;
  do {
    if (m_receiveBufferMutex.try_lock()) {
      if (m_receiveBuffer->size() > 0) {
        frame = m_receiveBuffer->front();
        frameSize = frame.data_size;
        m_receiveBuffer->erase(m_receiveBuffer->begin());
        blocking = false;
      }
      m_receiveBufferMutex.unlock();
    }
  } while (blocking);
  return frameSize;
}

/**
 * @brief Thread that will send everything one-by-one in the m_transmitBuffer.
 * To add to the transmit buffer simply use DTMF::transmit(...).
 * 
 * @param cancellation_token Stops transmit loop if set to true.
 */
void DTMF::transmitter(std::atomic<bool> &cancellation_token) {
  while (!cancellation_token) {
    while (m_transmitBuffer->size() > 0) {
      if (m_transmitBufferMutex.try_lock()) {
        m_controller->write(generateBooleanFrame(m_transmitBuffer->at(0)));
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
    DTMFFrame frame;
    readData = m_controller->read();
    if (readData.size() > 0) {
      frame = convertBoolVectorToFrame(readData);
      m_receiveBufferMutex.lock();
      m_receiveBuffer->push_back(frame);
      m_receiveBufferMutex.unlock();
    }
    #ifdef WITH_SLEEP
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_UTIME));
    #endif
  }
}

std::vector<bool> DTMF::generateBooleanFrame(DTMFFrame &frame) {
  std::vector<bool> boolDataVector;

  // Add the data response type in the first 4 bits of the frame to be sent.
  for (unsigned i = 4; i < sizeof(uint8_t) * 8; i++) {
    uint8_t bitmask = BIT_3;
    if ((frame.frame_response_type & bitmask) == bitmask) {
      boolDataVector.push_back(true);
    } else {
      boolDataVector.push_back(false);
    }

    bitmask = bitmask >> 1;
  }

  for (unsigned i = 0; i < frame.data_size; i++) {
    uint8_t bitmask = BIT_7;
    for (unsigned bit = 0; bit < sizeof(uint8_t) * 8; bit++) { // 8 is the size of a byte in bits

      // Checks if the current bit is set or not.
      if ((frame.data[i] & bitmask) == bitmask) {
        boolDataVector.push_back(true);
      } else {
        boolDataVector.push_back(false);
      }

      bitmask = bitmask >> 1;
    }
  }
  return boolDataVector;
}

DTMFFrame DTMF::convertBoolVectorToFrame(std::vector<bool> boolFrame) {
  DTMFFrame frame;
  frame.data_size = unsigned(boolFrame.size() / (sizeof(uint8_t) * 8));
  frame.data = new uint8_t(frame.data_size);
  unsigned super_index = 0;

  uint8_t responseType = DATA_NO_RES;
  for (unsigned bit = 3; bit > 0; --bit) {
    responseType |= short(boolFrame.at(super_index)) << bit;
    super_index++;
  } 

  for (unsigned i = 0; i < frame.data_size; i++) {
    frame.data[i] = 0;
    for (unsigned bit = (sizeof(uint8_t) * 8); bit > 0; --bit) {
      frame.data[i] |= short(boolFrame.at(super_index)) << bit;
      super_index++;
    }
  }
  return frame;
}

#ifdef TEST_SUITE

/**
 * @brief Construct a new DTMF::DTMF object. This one is used for testing
 * 
 * @param transmitBuffer 
 * @param receiveBuffer 
 */
DTMF::DTMF(std::vector<DTMFFrame> *transmitBuffer, std::vector<DTMFFrame> *receiveBuffer) {
  m_transmitBuffer = transmitBuffer;
  m_receiveBuffer = receiveBuffer;

  m_stopFlag = false;

  m_transmitThread = new std::thread(&DTMF::transmitter, this, std::ref(m_stopFlag));
  m_receiveThread = new std::thread(&DTMF::receiver, this, std::ref(m_stopFlag));
}

#endif
