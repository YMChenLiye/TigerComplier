#!/bin/bash

rm build/succ -f
rm build/fail -f

for file in ../testcase/*.tig
do
    build/lextest $file 2>> build/fail 1>> build/succ
    echo $file;
    wc -l build/fail;
    wc -l build/succ;
    rm build/succ -f
    rm build/fail -f
done

for file in ../testcase/*.tig
do
    echo $file 1>> build/succ;
    build/lextest $file 2>> build/fail 1>> build/succ
done

echo "total:"
wc -l build/fail;
wc -l build/succ;

rm build/succ -f
rm build/fail -f

