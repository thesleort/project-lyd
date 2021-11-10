#!/bin/sh
mkdir -p build
cd build
cmake -DTEST_SUITE=ON ..
make clean
make -j$(nproc)
cp libdtmf.so* ../../testapp
cp ../libdtmf/libdtmf.h ../../testapp
