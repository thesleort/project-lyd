cmake_minimum_required(VERSION 3.5)

project(dtmf_SFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include_directories("/home/peter/SFML/include/")
#link_directories("/home/peter/SFML/lib")

add_executable(
    dtmf_SFML
    main.cpp
    dtmf.cpp
    dtmf.h
    dtmfdecoder.cpp
    dtmfdecoder.h
    )

#target_link_libraries(dtmf_SFML -lsfml-audio-d -lsfml-system-d)

find_package(SFML 2 REQUIRED network audio graphics window system)
target_link_libraries(dtmf_SFML sfml-graphics sfml-audio)
