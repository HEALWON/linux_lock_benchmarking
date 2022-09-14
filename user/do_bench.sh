#!/bin/bash

W=0
T=(1 2 4 8 12 16)

make all

echo "testing spinlock..."

for i in ${T[@]}; do
    taskset ff ./perf_spinlock ${i} ${W}
done

# echo "testing wrlock..."

# for i in ${W[@]}; do
#     taskset ff ./perf_rwlock ${T} ${i}
# done

echo "testing mutex..."

for i in ${T[@]}; do
    taskset ff ./perf_mutex ${i} ${W}
done