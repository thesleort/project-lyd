cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_BUILD_TYPE Debug)

project(dtmf VERSION 0.1 DESCRIPTION "DTMF Communication Interface")

OPTION(TEST_SUITE "Compile for test usage" OFF)
OPTION(WITH_SLEEP "Add small wait state in all asynchronous threads" ON)

add_library(dtmf SHARED
  # Main library interface file
  layer-application/libdtmf.cpp
)
IF(WITH_SLEEP)
  MESSAGE(STATUS "Enabling sleep timers")
  ADD_DEFINITIONS(-DWITH_SLEEP)
  IF(SLEEO_MTIME)
    ADD_DEFINITIONS(-DSLEEP_UTIME=${SLEEP_MTIME})
  ELSE()
    ADD_DEFINITIONS(-DSLEEP_UTIME=10)
  ENDIF(SLEEO_MTIME)
ENDIF(WITH_SLEEP)

IF(TEST_SUITE)
  MESSAGE(STATUS "Compiling test suite")
  ADD_DEFINITIONS(-DTEST_SUITE)
ELSE()
  target_sources(dtmf
    PUBLIC
    # DTMF decoder and encoder layer
    layer-physical/dtmfdecoder.cpp
    layer-physical/dtmfEncoder.cpp
    layer-physical/physicallayer.cpp

    # Data transfer layer
    layer-data-transfer/Controller/Controller.cpp
    layer-data-transfer/Cyclic/Cyclic.cpp
    layer-data-transfer/Protocol/bytestuffer.cpp

    layer-data-transfer/FrameGenPlaceholder/framegen.cpp

    # Physical layer
    layer-physical/dtmfdecoder.cpp
    layer-physical/dtmfEncoder.cpp
    layer-physical/physicallayer.cpp
    layer-physical/digitalfilter.cpp
    layer-physical/firfilter.cpp
  )
ENDIF(TEST_SUITE)

include_directories(dtmf
  layer-physical
  layer-data-transfer/Controller
  layer-data-transfer/Cyclic
  layer-data-transfer/FrameGenPlaceholder
  layer-data-transfer/Protocol
  layer-data-transfer/config
)
find_package(SFML 2 REQUIRED network audio graphics window system)
target_link_libraries(dtmf sfml-graphics sfml-audio fftw3)

set_target_properties(dtmf PROPERTIES SOVERSION 1)
set_target_properties(dtmf PROPERTIES PUBLIC_HEADER layer-application/libdtmf.h)
