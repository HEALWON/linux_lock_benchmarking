#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "common.h"
#include "perf_spinlock.h"

static spinlock_t spinlock[MAX_TEST];
static int var = 0;

static int reader_func(void *data)
{
  int i = 0;
  spinlock_t *lock = (spinlock_t *) data;

  for (i = 0; i < ITER; i++) {
    spin_lock(lock);
    udelay(DURATION);
    spin_unlock(lock);
  }

  return 0;
}

static int writer_func(void *data)
{
  int i = 0;
  spinlock_t *lock = (spinlock_t *) data;

  for (i = 0; i < ITER; i++) {
    spin_lock(lock);
    // var++;
    udelay(DURATION);
    spin_unlock(lock);
  }

  return 0;
}

typedef struct {
  int threads;
  int idx;
} args_t;

static int perf_spinlock(void *data)
{
  struct task_struct *tid[MAX_THREAD];
  ktime_t start, end, elapsed;
  int ret, status;
  int i;

  int num_thread = ((args_t *) data)->threads; 
  int idx = ((args_t *) data)->idx; 

  kfree(data);

  int num_writer = 0;

  spin_lock_init(&spinlock[idx]);

  start = ktime_get();

  for (i = 0; i < num_thread; i++) {
    tid[i] =(struct task_struct *) kthread_run((i < num_writer)? writer_func : reader_func, &spinlock[idx], "worker_thread");
    if (tid[i] == NULL) {
      pr_err("pthread create error");
      return 0; 
    }
  }

  for (i = 0; i < num_thread; i++) {
    kthread_stop(tid[i]);
  }

  end = ktime_get();
  elapsed = end - start;

  // if (var != num_writer * ITER)
  //   pr_err("synchronization failed\n");
  printk("(test %d) Elapsed time: %lld.%09lld (s)\n", idx, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  return 0;
}

void perf_spinlock_multiple(int threads, int tests) 
{
  struct task_struct *tid[MAX_TEST];
  int i;
  args_t *args;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("benchmarking spinlock... (threads = %d, tests = %d)\n", threads, tests);

  for (i = 0; i < tests; i++) {
    args = (args_t *) kmalloc(sizeof(args_t), GFP_KERNEL);
    args->threads = threads;
    args->idx = i;
    tid[i] =(struct task_struct *) kthread_run(perf_spinlock, (void *) &args, "worker_thread");
    if (tid[i] == NULL) {
      pr_err("pthread create error");
      return; 
    }
  }

  for (i = 0; i < tests; i++) {
    kthread_stop(tid[i]);
  }

  printk("Done\n");

  return;
}
