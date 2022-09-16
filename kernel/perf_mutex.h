#ifndef __PERF_MUTEX_H__
#define __PERF_MUTEX_H__

void perf_mutex_multiple(int threads, int tests);
void perf_mutex_single(int threads, int tests);

#endif