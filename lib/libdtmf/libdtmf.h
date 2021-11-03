#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include "Controller.h"

struct DTMFFrame {

};

class DTMFProtocol {
  public:
    DTMFProtocol();

    void send();

    const std::vector<DTMFFrame>* getReceivedMessagesBuffer();

  private:
    Controller m_controller;

    const std::vector<DTMFFrame> *m_receivedBuffer;
};

#endif
