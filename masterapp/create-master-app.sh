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
cp libdtmf.so* ../../masterapp
cd ..
cp */*/*.h ../masterapp
cp libdtmf/libdtmf.h ../masterapp
cd ../masterapp

# Compile master application
mkdir -p build
cd build
cmake ..
make -j$(nproc)
