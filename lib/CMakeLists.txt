cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_BUILD_TYPE Debug)

project(dtmf VERSION 0.1 DESCRIPTION "DTMF Communication Interface")

OPTION(TEST_SUITE "Compile for test usage" OFF)

add_library(dtmf SHARED
  # Main library interface file
  libdtmf/libdtmf.cpp
)
IF(TEST_SUITE)
    ADD_DEFINITIONS(-DTEST_SUITE)
ELSE()
  target_sources(dtmf
    PUBLIC
    # DTMF decoder and encoder layer
    layer-dtmf/dtmfdecoder.cpp
    layer-dtmf/dtmfdecoder.h
    layer-dtmf/dtmfEncoder.cpp
    layer-dtmf/dtmfEncoder.h
    layer-dtmf/physicallayer.cpp
    layer-dtmf/physicallayer.h

    # Data transfer layer
    layer-data-transfer/Controller/Controller.cpp
    layer-data-transfer/Controller/Controller.h
    layer-data-transfer/Cyclic/Cyclic.cpp
    layer-data-transfer/Cyclic/Cyclic.h
    layer-data-transfer/Protocol/bytestuffer.cpp
    layer-data-transfer/Protocol/bytestuffer.h

    layer-data-transfer/FrameGenPlaceholder/framegen.cpp
    layer-data-transfer/FrameGenPlaceholder/framegen.h
  )
ENDIF(TEST_SUITE)

include_directories(dtmf
  layer-dtmf
  layer-data-transfer/Controller
  layer-data-transfer/Cyclic
  layer-data-transfer/FrameGenPlaceholder
  layer-data-transfer/Protocol
  layer-data-transfer/config
)
find_package(SFML 2 REQUIRED network audio graphics window system)
target_link_libraries(dtmf sfml-graphics sfml-audio)

set_target_properties(dtmf PROPERTIES SOVERSION 1)
set_target_properties(dtmf PROPERTIES PUBLIC_HEADER libdtmf/libdtmf.h)
