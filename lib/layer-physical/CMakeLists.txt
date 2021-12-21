cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread") # fix for error: CMakeFiles/dtmf_SFML.dir/physicallayer.cpp.o: undefined reference to symbol 'pthread_create@@GLIBC_2.2.5'
Set(CMAKE_CXX_STANDARD 11)
project(dtmf_SFML)
add_executable(
    dtmf_SFML
    main.cpp
    dtmfdecoder.cpp
    dtmfdecoder.h
    dtmfEncoder.cpp
    dtmfEncoder.h
    physicallayer.cpp
    physicallayer.h
    digitalfilter.cpp
    digitalfilter.h
    firfilter.cpp
    firfilter.h

    )

find_package(SFML 2 REQUIRED network audio graphics window system)
target_link_libraries(dtmf_SFML sfml-graphics sfml-audio fftw3)
