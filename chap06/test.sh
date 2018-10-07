#!/bin/bash

chmod +x clean.sh
./clean.sh

mkdir build
cd build
cmake ..
make
#../build/parse test.tig

cd ..
#test
for file in ../testcase/*.tig
do
    echo "test" $file
    build/parse $file
done
