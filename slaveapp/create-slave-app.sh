#!/bin/bash

# Prepare libdtmf library
cd ../lib
mkdir -p build
cd build
cmake -DTEST_SUITE=OFF ..
make -j$(nproc)
cp libdtmf.so* ../../slaveapp
cd ..
cp */*/*.h ../slaveapp
cp libdtmf/libdtmf.h ../slaveapp
cd ../slaveapp

# Compile slave application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
