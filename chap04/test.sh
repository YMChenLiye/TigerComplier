#!/bin/bash

chmod +x clean.sh
./clean.sh

mkdir build
cd build
cmake ..
make
