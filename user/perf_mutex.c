#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "common.h"

pthread_mutex_t mutex[MAX_TEST];
int var = 0;

int num_thread = 1;
int num_test = 1;

void *reader_func(void *data)
{
  pthread_mutex_t *lock = (pthread_mutex_t *) data;

  for (int i = 0; i < ITER; i++) {
    pthread_mutex_lock(lock);
    spin(DURATION);
    pthread_mutex_unlock(lock);
  }

  return 0;
}

void *writer_func(void *data)
{
  pthread_mutex_t *lock = (pthread_mutex_t *) data;

  for (int i = 0; i < ITER; i++) {
    pthread_mutex_lock(lock);
    // var++;
    spin(DURATION);
    pthread_mutex_unlock(lock);
  }

  return 0;
}

void *perf_mutex(void *data)
{
  pthread_t tid[MAX_THREAD];
  struct timeval start, end, elapsed;
  int ret, status;
  int i;
  int idx = *((int *) data);
  free(data);

  pthread_mutex_init(&mutex[idx], NULL);

  gettimeofday(&start, NULL);

  for (i = 0; i < num_thread; i++) {
    ret = pthread_create(&tid[i], NULL, writer_func, &mutex[idx]);
    if (ret < 0) {
      perror("pthread create error");
      exit(EXIT_FAILURE);    
    }
  }

  for (i = 0; i < num_thread; i++) {
    pthread_join(tid[i], NULL);
  }

  gettimeofday(&end, NULL);
  timersub(&end, &start, &elapsed);

  // if (var != num_writer * ITER)
  //   perror("synchronization failed\n");
  printf("(test %d) Elapsed time: %ld.%06ld (s)\n", idx, elapsed.tv_sec, elapsed.tv_usec);

  return;
}

int main(int argc, char *argv[])
{
  pthread_t tid[MAX_TEST];
  int ret, status;
  int i;
  int *arg;

  if (argc < 3) {
    printf("usage: %s [num_thread] [num_test]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  num_thread = atoi(argv[1]);
  num_test = atoi(argv[2]);

  printf("benchmarking mutex... (threads = %d, tests = %d)\n", num_thread, num_test);

  for (i = 0; i < num_test; i++) {
    arg = (int *)malloc(sizeof(int));
    *arg = i;
    ret = pthread_create(&tid[i], NULL, perf_mutex, arg);
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