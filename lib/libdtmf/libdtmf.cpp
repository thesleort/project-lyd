
#include "libdtmf.h"

/**
 * @brief Construct a new DTMFProtocol::DTMFProtocol object
 * 
 */
DTMFProtocol::DTMFProtocol() {
  m_receivedMessagesBuffer = new std::vector<DTMFFrame>();
}

/**
 * @brief Destroy the DTMFProtocol::DTMFProtocol object
 * 
 */
DTMFProtocol::~DTMFProtocol() {
  delete m_receivedMessagesBuffer;
}

/**
 * @brief Returns the earliest frame still available on the buffer.
 * Acts as a FIFO queue.
 * 
 * @return const DTMFFrame 
 */
const DTMFFrame DTMFProtocol::getMessage() {

  // Make sure no one is writing to the received messages buffer.
  m_messagesBufferLock.lock();
  if (m_receivedMessagesBuffer->size() > 0) {
    const DTMFFrame frame = m_receivedMessagesBuffer->front();
    m_receivedMessagesBuffer->erase(m_receivedMessagesBuffer->begin());
    m_messagesBufferLock.unlock();
    return frame;
  }
}

/**
 * @brief Returns a pointer to the buffer containing the latest frames.
 * 
 * @return const std::vector<DTMFFrame>* 
 */
const std::vector<DTMFFrame> *DTMFProtocol::getReceivedMessagesBuffer() {
  return m_receivedMessagesBuffer;
}
