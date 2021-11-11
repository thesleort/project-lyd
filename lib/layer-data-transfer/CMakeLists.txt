cmake_minimum_required(VERSION 3.11-3.18)

project(layer-data-transfer VERSION 0.1 DESCRIPTION "DTMF Layer: Data transfer")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BUILD_TYPE Debug)

add_executable(layer-data-transfer
  Controller/main.cpp
  Controller/Controller.cpp
  Controller/Controller.h

  Cyclic/Cyclic.cpp
  Cyclic/Cyclic.h

  FrameGenPlaceholder/framegen.cpp
  FrameGenPlaceholder/framegen.h

  Protocol/bytestuffer.cpp
  Protocol/bytestuffer.h
)

include_directories(
  config
  Controller
  Cyclic
  FrameGenPlaceholder
  Protocol
)

find_package(Threads REQUIRED)
target_link_libraries(layer-data-transfer PRIVATE Threads::Threads)


