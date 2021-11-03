#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include <thread>
#include <mutex>

#include "Controller.h"

struct DTMFFrame {

};

class DTMFProtocol {
  public:
    DTMFProtocol();
    ~DTMFProtocol();

    void send();

    const DTMFFrame getMessage();
    const std::vector<DTMFFrame>* getReceivedMessagesBuffer();

  private:
    Controller m_controller;

    std::mutex m_messagesBufferLock;
    std::vector<DTMFFrame> *m_receivedMessagesBuffer;
};

#endif
