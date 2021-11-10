#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include <thread>
#include <mutex>

#include "Controller.h"

struct DTMFFrame {
  uint16_t sizeBytes;
};

class DTMF {
  public:
    DTMF();
    ~DTMF();

    void transmit(DTMFFrame &frame, bool blocking);

    const uint16_t receive(DTMFFrame &frame, bool blocking);
  private:
    Controller m_controller;

    std::mutex m_messagesBufferMutex;
    std::mutex m_transmitBufferMutex;
    std::vector<DTMFFrame> *m_transmitBuffer;
    std::vector<DTMFFrame> *m_receiveBuffer;
};

#endif
