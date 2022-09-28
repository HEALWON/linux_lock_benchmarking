#!/bin/bash

T=(1 2 4 8 12 16)
I=5

make all

echo "testing mutex..."

for i in ${T[@]}; do
    taskset ff ./bench mutex ${i} ${I}
done

echo "testing spinlock..."

for i in ${T[@]}; do
    taskset ff ./bench spinlock ${i} ${I}
done

# echo "testing wrlock..."

# for i in ${W[@]}; do
#     taskset ff ./perf_rwlock ${T} ${i}
# done