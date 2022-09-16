#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "common.h"

pthread_spinlock_t spinlock[MAX_TEST];
pthread_barrier_t barrier[MAX_TEST]; 

int var = 0;

int num_thread = 1;
int num_test = 1;
int verbose = 0;

void *writer_func(void *data)
{
  int idx = *((int *) data);
  free(data);

  pthread_barrier_wait(&barrier[idx]);

  for (int i = 0; i < ITER; i++) {
    pthread_spin_lock(&spinlock[idx]);
    // var++;
    spin(DURATION);
    pthread_spin_unlock(&spinlock[idx]);
  }

  return 0;
}

void *perf_spinlock(void *data)
{
  pthread_t tid[MAX_THREAD];
  struct timeval start, end, elapsed;
  int ret, status;
  int i;
  int *arg;
  int idx = *((int *) data);
  free(data);

  pthread_spin_init(&spinlock[idx], PTHREAD_PROCESS_SHARED);
  pthread_barrier_init(&barrier[idx], NULL, num_thread + 1);

  for (i = 0; i < num_thread; i++) {
    arg = (int *)malloc(sizeof(int));
    *arg = idx;
    ret = pthread_create(&tid[i], NULL, writer_func, arg);
    if (ret < 0) {
      perror("pthread create error");
      exit(EXIT_FAILURE);    
    }
  }

  gettimeofday(&start, NULL);
  pthread_barrier_wait(&barrier[idx]);

  for (i = 0; i < num_thread; i++) {
    pthread_join(tid[i], NULL);
  }

  gettimeofday(&end, NULL);
  timersub(&end, &start, &elapsed);

  // if (var != num_writer * ITER)
  //   perror("synchronization failed\n");
  if (verbose)
    printf("(test %d) Elapsed time: %ld.%06ld (s)\n", idx, elapsed.tv_sec, elapsed.tv_usec);
  else
    printf("%ld.%06ld\n", elapsed.tv_sec, elapsed.tv_usec);

  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t tid[MAX_TEST];
  int ret, status;
  int i;
  int *arg;

  if (argc < 3) {
    printf("usage: %s [num_thread] [num_test] [verbose]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  num_thread = atoi(argv[1]);
  num_test = atoi(argv[2]);
  verbose = (argc < 4)? 0 : 1;

  printf("benchmarking spinlock... (threads = %d, tests = %d)\n", num_thread, num_test);

  for (i = 0; i < num_test; i++) {
    arg = (int *)malloc(sizeof(int));
    *arg = i;
    ret = pthread_create(&tid[i], NULL, perf_spinlock, arg);
    if (ret < 0) {
      perror("pthread create error");
      exit(EXIT_FAILURE);    
    }
  }

  for (i = 0; i < num_test; i++) {
    pthread_join(tid[i], NULL);
  }

  printf("Done\n");

  return 0;
}