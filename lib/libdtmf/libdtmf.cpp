
#include "libdtmf.h"

DTMFProtocol::DTMFProtocol() {
  m_receivedBuffer = new std::vector<DTMFFrame>();
}
