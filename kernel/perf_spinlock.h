#ifndef __PERF_SPINLOCK_H__
#define __PERF_SPINLOCK_H__


void perf_spinlock_multiple(int threads, int tests);
void perf_spinlock_single(int threads, int tests);

#endif