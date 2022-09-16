#ifndef __COMMON_H__
#define __COMMON_H__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_THREAD 16
#define MAX_TEST 16
#define ITER 10000
#define DURATION 10

void spin(long duration);

#endif