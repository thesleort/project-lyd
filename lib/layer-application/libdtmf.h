#ifndef LIB_DTMF_H
#define LIB_DTMF_H

#include <vector>

#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <condition_variable>

#ifndef TEST_SUITE
#include "Controller.h"
#endif

struct DTMFPacket {
  uint8_t data_size;
  uint8_t* data;
  uint8_t packet_response_type;
};

#define DATA_NO_RESPONSE      0b00000000
#define DATA_REQUIRE_RESPONSE 0b00000001
#define DATA_RESPONSE         0b00000010

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
    DTMF(std::vector<DTMFPacket> *transmitBuffer, std::vector<DTMFPacket> *receiveBuffer);
    #endif

    void transmit(DTMFPacket packet, bool blocking = true);

    const uint16_t receive(DTMFPacket &packet, bool blocking = true);

    const bool isWaitingResponse();

  private:
    #ifndef TEST_SUITE
    Controller *m_controller;
    #endif

    std::thread *m_transmitThread;
    std::thread *m_receiveThread;


    void transmitter(std::atomic<bool> &cancellation_token);
    void receiver(std::atomic<bool> &cancellation_token);

    std::atomic<bool> m_waitingResponse;
    std::atomic<bool> m_stopFlag;
    std::atomic<bool> m_responseFlag;

    std::condition_variable m_cv_transmitted;
    std::mutex m_transmitMutex;
    std::condition_variable m_cv_waitingResponse;
    std::mutex m_waitingResponseMutex;

    std::mutex m_transmitBufferMutex;
    std::mutex m_receiveBufferMutex;
    std::vector<DTMFPacket> *m_transmitBuffer;
    std::vector<DTMFPacket> *m_receiveBuffer;

    std::vector<bool> generateBooleanFrame(DTMFPacket &packet);
    DTMFPacket convertBoolVectorToPacket(std::vector<bool> boolFrame);
};

#endif
