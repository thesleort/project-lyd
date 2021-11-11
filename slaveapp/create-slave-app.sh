#!/bin/bash

# Prepare libdtmf library
cd ../lib
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cp libdtmf.so* ../../slaveapp
cd ..
cp lib/libdtmf.h ../slaveapp
cd ../masterapp

# Compile slave application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
