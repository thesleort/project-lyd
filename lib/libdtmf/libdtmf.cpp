
#include "libdtmf.h"

/**
 * @brief Construct a new DTMF::DTMF object
 * 
 */
DTMF::DTMF() {
  m_receiveBuffer = new std::vector<DTMFFrame>();
  m_transmitBuffer = new std::vector<DTMFFrame>();

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
void DTMF::transmit(DTMFFrame &frame, bool blocking = true) {
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
const uint16_t DTMF::receive(DTMFFrame &frame, bool blocking = true) {
  u_int16_t frameSize = 0;
  do {
    if (m_receiveBufferMutex.try_lock()) {
      if (m_receiveBuffer->size() > 0) {
        const DTMFFrame frame = m_receiveBuffer->front();
        frameSize = frame.sizeBytes;
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

  }
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
