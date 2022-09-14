#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "common.h"

pthread_spinlock_t spinlock;

int var = 0;

void *reader_func(void *data)
{
  for (int i = 0; i < ITER; i++) {
    pthread_spin_lock(&spinlock);
    spin(DURATION);
    pthread_spin_unlock(&spinlock);
  }

  return 0;
}

void *writer_func(void *data)
{
  for (int i = 0; i < ITER; i++) {
    pthread_spin_lock(&spinlock);
    var++;
    spin(DURATION);
    pthread_spin_unlock(&spinlock);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t tid[MAX_THREAD];
  struct timeval start, end, elapsed;
  int ret, status;
  int i;

  if (argc < 3) {
    printf("usage: %s [num_thread] [num_writer]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int num_thread = atoi(argv[1]);
  int num_writer = atoi(argv[2]);

  pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);

  gettimeofday(&start, NULL);

  for (i = 0; i < num_thread; i++) {
    ret = pthread_create(&tid[i], NULL, (i < num_writer)? writer_func : reader_func, NULL);
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

  if (var != num_writer * ITER)
    perror("synchronization failed\n");
  printf("(%d/%d) Elapsed time: %ld.%06ld (s)\n", num_writer, num_thread, elapsed.tv_sec, elapsed.tv_usec);

  return 0;
}