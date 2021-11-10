
#include "libdtmf.h"

/**
 * @brief Construct a new DTMF::DTMF object
 * 
 */
DTMF::DTMF() {
  m_receiveBuffer = new std::vector<DTMFFrame>();
  m_transmitBuffer = new std::vector<DTMFFrame>();
}

/**
 * @brief Destroy the DTMF::DTMF object
 * 
 */
DTMF::~DTMF() {
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
    if (m_messagesBufferMutex.try_lock()) {
      if (m_receiveBuffer->size() > 0) {
        const DTMFFrame frame = m_receiveBuffer->front();
        frameSize = frame.sizeBytes;
        m_receiveBuffer->erase(m_receiveBuffer->begin());
        blocking = false;
      }
      m_messagesBufferMutex.unlock();
    }
  } while (blocking);
  return frameSize;
}
