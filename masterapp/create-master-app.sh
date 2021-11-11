#!/bin/bash

# Prepare libdtmf library
cd ../lib
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cp libdtmf.so* ../../masterapp
cd ..
cp libdtmf/libdtmf.h ../masterapp
cd ../masterapp

# Compile master application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
