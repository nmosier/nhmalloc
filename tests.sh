#!/bin/bash

export LD_PRELOAD=./nhmalloc.so

./test
./test -p 10   -n 10000 -o 10000
./test -p 100  -n 1000  -o 30000
./test -p 1000 -n 1000  -o 20000
