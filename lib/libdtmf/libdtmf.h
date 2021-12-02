#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include <thread>
#include <mutex>
#include <atomic>
#include <string>

#ifndef TEST_SUITE
#include "Controller.h"
#endif

struct DTMFFrame {
  uint8_t data_size;
  uint8_t* data;
  uint8_t frame_response_type;
};

#define DATA_NO_RES  0b00000000
#define DATA_REQ_RES 0b00000001
#define DATA_RES     0b00000010

#define BIT_0 0b00000001
#define BIT_1 0b00000010
#define BIT_2 0b00000100
#define BIT_3 0b00001000
#define BIT_4 0b00010000
#define BIT_5 0b00100000
#define BIT_6 0b01000000
#define BIT_7 0b10000000

class DTMF {
  public:
    DTMF();
    ~DTMF();

    #ifdef TEST_SUITE
    DTMF(std::vector<DTMFFrame> *transmitBuffer, std::vector<DTMFFrame> *receiveBuffer);
    #endif

    void transmit(DTMFFrame frame, bool blocking = true);

    const uint16_t receive(DTMFFrame &frame, bool blocking = true);


  private:
    #ifndef TEST_SUITE
    Controller *m_controller;
    #endif

    std::thread *m_transmitThread;
    std::thread *m_receiveThread;

    void transmitter(std::atomic<bool> &cancellation_token);
    void receiver(std::atomic<bool> &cancellation_token);

    std::atomic<bool> m_stopFlag;
    std::atomic<bool> m_responseFlag;

    std::mutex m_transmitBufferMutex;
    std::mutex m_receiveBufferMutex;
    std::vector<DTMFFrame> *m_transmitBuffer;
    std::vector<DTMFFrame> *m_receiveBuffer;

    std::vector<bool> generateBooleanFrame(DTMFFrame &frame);
    DTMFFrame convertBoolVectorToFrame(std::vector<bool> boolFrame);
};

#endif
