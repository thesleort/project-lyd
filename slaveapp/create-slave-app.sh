#!/bin/bash

# Create directories for generated files
mkdir -p include
mkdir -p lib

# Prepare libdtmf library
cd ../lib
mkdir -p build
cd build
cmake -DTEST_SUITE=OFF -DWITH_SLEEP=ON -DSLEEP_MTIME=10 ..
make -j$(nproc)
cp libdtmf.so* ../../slaveapp/lib
cd ..
cp */*.h ../slaveapp/include
cp */*/*.h ../slaveapp/include
cp libdtmf/libdtmf.h ../slaveapp/include
cd ../slaveapp

# Compile slave application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
