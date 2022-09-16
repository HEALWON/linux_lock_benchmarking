#!/bin/bash

sudo rmmod bench
make all
sudo insmod bench.ko ltype=$1 threads=$2 tests=$3