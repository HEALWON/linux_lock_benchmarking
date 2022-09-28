#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "lockbench.h"

/* Parameters */
LockType_t locktype = LOCKTYPE_MUTEX;
int threads = 1;
int tests = 1;
int verbose = 0;

int main(int argc, char *argv[])
{
  pthread_t tid[MAX_TEST];
  int ret, status;
  int i;
  int *arg;

  switch (argc) {
    case 5:
      verbose = 1;
    case 4:
      tests = atoi(argv[3]);
    case 3:
      threads = atoi(argv[2]);
    case 2:
      if (strcmp(argv[1], "mutex") == 0) {
        locktype = LOCKTYPE_MUTEX;
      } else if (strcmp(argv[1], "spinlock") == 0) {
        locktype = LOCKTYPE_SPINLOCK;
      } else {
        printf("Not supported lock type: %s\n", argv[1]);
        exit(EXIT_FAILURE);
      }
      break;
    case 1:
      printf("Lock performance test\n");
      printf("Usage: %s [locktype] [threads=1] [tests=1] [verbose=0]\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  bench_multiple(locktype, threads, tests, verbose);

  return 0;
}