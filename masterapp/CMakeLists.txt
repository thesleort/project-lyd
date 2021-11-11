cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS -pthread)

project(masterapp VERSION 0.1 DESCRIPTION "Master application")

add_executable(masterapp
  main.cpp
)
set_target_properties(masterapp PROPERTIES PUBLIC_HEADER libdtmf.h)

target_link_libraries(masterapp ${CMAKE_SOURCE_DIR}/libdtmf.so)


