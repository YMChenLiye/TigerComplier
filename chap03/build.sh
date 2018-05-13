#!/bin/bash

chmod +x clean.sh
./clean.sh

flex tiger.lex

bison -ydv tiger.grm

mkdir build
cd build
cmake ..
make

#test
./parse ../liyetest.tig
