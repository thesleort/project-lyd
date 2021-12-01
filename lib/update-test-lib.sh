#!/bin/sh
mkdir -p build
cd build
cmake -DTEST_SUITE=OFF -DWITH_SLEEP=ON -DSLEEP_MTIME=10 .. ..
make clean
make -j$(nproc)
cp libdtmf.so* ../../testapp
cp ../libdtmf/libdtmf.h ../../testapp
