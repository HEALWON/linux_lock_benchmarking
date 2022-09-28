#!/bin/bash

T=(1 2 4 8 12 16)
I=5

make clean
make all

echo "testing mutex..."

for i in ${T[@]}; do
    sudo rmmod bench
    sudo insmod bench.ko ltype=mutex threads=${i} tests=${I}
done

echo "testing spinlock..."

for i in ${T[@]}; do
    sudo rmmod bench
    sudo insmod bench.ko ltype=spinlock threads=${i} tests=${I}
done