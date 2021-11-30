cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS -pthread)

project(slaveapp VERSION 0.1 DESCRIPTION "Master application")

add_executable(slaveapp
  main.cpp
  roscom.cpp
  commands.cpp
)
set_target_properties(slaveapp PROPERTIES PUBLIC_HEADER include/libdtmf.h)

include_directories(slaveapp 
  include
)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(slaveapp paho-mqttpp3 paho-mqtt3as nlohmann_json::nlohmann_json)

target_link_libraries(slaveapp ${CMAKE_SOURCE_DIR}/lib/libdtmf.so)


