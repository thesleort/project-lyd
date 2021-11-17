cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS -pthread)

project(slaveapp VERSION 0.1 DESCRIPTION "Master application")

add_executable(slaveapp
  main.cpp
)
set_target_properties(slaveapp PROPERTIES PUBLIC_HEADER include/libdtmf.h)

include_directories(slaveapp 
  include
)

target_link_libraries(slaveapp ${CMAKE_SOURCE_DIR}/lib/libdtmf.so)


