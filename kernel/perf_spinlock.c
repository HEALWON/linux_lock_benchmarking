#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include "common.h"
#include "perf_spinlock.h"

static spinlock_t spinlock;
static int var = 0;

static int reader_func(void *data)
{
  int i = 0;
  for (i = 0; i < ITER; i++) {
    spin_lock(&spinlock);
    udelay(DURATION);
    spin_unlock(&spinlock);
  }

  return 0;
}

static int writer_func(void *data)
{
  int i = 0;
  for (i = 0; i < ITER; i++) {
    spin_lock(&spinlock);
    var++;
    udelay(DURATION);
    spin_unlock(&spinlock);
  }

  return 0;
}


void perf_spinlock(int num_thread)
{
  struct task_struct *tid[MAX_THREAD];
  ktime_t start, end, elapsed;
  int ret, status;
  int i;

/*
  if (argc < 3) {
    printf("usage: %s [num_thread] [num_writer]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
*/

  // int num_thread = 8;
  int num_writer = 0;

  spin_lock_init(&spinlock);

  start = ktime_get();

  for (i = 0; i < num_thread; i++) {
    tid[i] =(struct task_struct *) kthread_run((i < num_writer)? writer_func : reader_func, NULL, "worker_thread");
    if (tid[i] == NULL) {
      pr_err("pthread create error");
      return; 
    }
  }

  for (i = 0; i < num_thread; i++) {
    kthread_stop(tid[i]);
  }

  end = ktime_get();
  elapsed = end - start;

  if (var != num_writer * ITER)
    pr_err("synchronization failed\n");
  printk("(%d/%d) Elapsed time: %lld.%lld (s)\n", num_writer, num_thread, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  return;
}