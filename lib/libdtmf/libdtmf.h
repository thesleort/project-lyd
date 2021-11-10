#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include <thread>
#include <mutex>
#include <atomic>

#ifndef TEST_SUITE
#include "Controller.h"
#endif

struct DTMFFrame {
  uint16_t sizeBytes;
};

class DTMF {
  public:
    DTMF();
    ~DTMF();

    #ifdef TEST_SUITE
    DTMF(std::vector<DTMFFrame> *transmitBuffer, std::vector<DTMFFrame> *receiveBuffer);
    #endif

    void transmit(DTMFFrame &frame, bool blocking);

    const uint16_t receive(DTMFFrame &frame, bool blocking);


  private:
    #ifndef TEST_SUITE
    Controller m_controller;
    #endif

    std::thread *m_transmitThread;
    std::thread *m_receiveThread;

    void transmitter(std::atomic<bool> &cancellation_token);
    void receiver(std::atomic<bool> &cancellation_token);

    std::atomic<bool> m_stopFlag;

    std::mutex m_transmitBufferMutex;
    std::mutex m_receiveBufferMutex;
    std::vector<DTMFFrame> *m_transmitBuffer;
    std::vector<DTMFFrame> *m_receiveBuffer;
};

#endif
