cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
project(libdtmf VERSION 0.1 DESCRIPTION "DTMF Communication Interface")

add_library(libdtmf SHARED
  # Main library interface file
  libdtmf/libdtmf.cpp

  # DTMF decoder and encoder layer
  layer-dtmf/dtmfdecoder.cpp
  layer-dtmf/dtmfdecoder.h
  layer-dtmf/dtmfEncoder.cpp
  layer-dtmf/dtmfEncoder.h
  layer-dtmf/physicallayer.cpp
  layer-dtmf/physicallayer.h

  # Data transfer layer
  layer-data-transfer/
)

include_directories(libdtmf
  layer-dtmf
  layer-data-transfer/Controller
  layer-data-transfer/Cyclic
  layer-data-transfer/FrameGenPlaceholder
  layer-data-transfer/Protocol
)

set_target_properties(libdtmf PROPERTIES SOVERSION 1)

set_target_properties(libdtmf PROPERTIES PUBLIC_HEADER libdtmf/libdtmf.h)
