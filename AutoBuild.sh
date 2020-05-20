#! /bin/bash

set -x

rm -rf ./build/*
cd `pwd`/build/
cmake ..
make
