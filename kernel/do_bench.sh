#!/bin/bash

sudo rmmod bench
make all
sudo insmod bench.ko ltype=spinlock threads=4