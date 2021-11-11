cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS -pthread)

project(testapp VERSION 0.1 DESCRIPTION "testapp for libdtmf")

add_executable(testapp
  main.cpp
)
set_target_properties(testapp PROPERTIES PUBLIC_HEADER libdtmf.h)

target_link_libraries(testapp ${CMAKE_SOURCE_DIR}/libdtmf.so)


