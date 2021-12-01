#!/bin/bash

# Create directories for generated files
mkdir -p include
mkdir -p lib

# Prepare libdtmf library
cd ../lib
mkdir -p build
cd build
cmake -DTEST_SUITE=OFF ..
make -j$(nproc)
cp libdtmf.so* ../../masterapp/lib
cd ..
cp */*.h ../masterapp/include
cp */*/*.h ../masterapp/include
cp libdtmf/libdtmf.h ../masterapp/include
cd ../masterapp

# Compile master application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
