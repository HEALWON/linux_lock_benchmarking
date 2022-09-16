#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include "common.h"
#include "perf_mutex.h"

typedef struct {
  struct completion comp;
  struct mutex *lock;
} args_inner_t;

typedef struct {
  struct completion comp;
  struct mutex *lock;
  int num_thread;
} args_t;

static int writer_func(void *data)
{
  int i = 0;
  struct completion *comp = &(((args_inner_t *) data)->comp);
  struct mutex *lock = ((args_inner_t *) data)->lock;

  for (i = 0; i < ITER; i++) {
    while(mutex_lock_interruptible(lock) != 0) {;}
    // var++;
    udelay(DURATION);
    mutex_unlock(lock);
  }

  complete_and_exit(comp, 0);
}

static int perf_mutex(void *data)
{
  struct task_struct *tid[MAX_THREAD];
  args_inner_t args[MAX_THREAD];
  ktime_t start, end, elapsed;
  int i;

  struct completion *comp = &(((args_t *) data)->comp);
  struct mutex *lock = ((args_t *) data)->lock;
  int num_thread = ((args_t *) data)->num_thread; 

  mutex_init(lock);

  start = ktime_get();

  for (i = 0; i < num_thread; i++) {
    init_completion(&(args[i].comp));
    args[i].lock = lock;
    tid[i] = kthread_run(writer_func, (void *) &args[i], "worker_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return 0;
    }
  }

  for (i = 0; i < num_thread; i++)
    wait_for_completion(&(args[i].comp));

  end = ktime_get();
  elapsed = end - start;

  // if (var != num_writer * ITER)
  //   pr_err("synchronization failed\n");
  // printk("(test %d) Elapsed time: %lld.%09lld (s)\n", idx, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);
  printk("%lld.%09lld\n", elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  complete_and_exit(comp, 0);
}

void perf_mutex_single(int threads, int tests) 
{
  struct task_struct *tid[MAX_THREAD];
  args_inner_t args[MAX_THREAD];
  struct mutex mutex;
  int var;
  ktime_t start, end, elapsed;
  int i;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("Benchmarking mutex... (threads = %d, tests = %d)\n", threads, tests);

  mutex_init(&mutex);

  start = ktime_get();

  for (i = 0; i < threads; i++) {
    init_completion(&(args[i].comp));
    args[i].lock = &mutex;
    tid[i] = kthread_run(writer_func, (void *) &args[i], "worker_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return;
    }
  }

  for (i = 0; i < threads; i++)
    wait_for_completion(&(args[i].comp));

  end = ktime_get();
  elapsed = end - start;

  // if (var != num_writer * ITER)
  //   pr_err("synchronization failed\n");
  // printk("(test %d) Elapsed time: %lld.%09lld (s)\n", idx, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);
  printk("%lld.%09lld\n", elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  return;
}

void perf_mutex_multiple(int threads, int tests) 
{
  struct task_struct *tid[MAX_TEST];
  struct mutex mutex[MAX_TEST];
  int var[MAX_TEST];
  int i;
  args_t *args;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("Benchmarking mutex... (threads = %d, tests = %d)\n", threads, tests);

  for (i = 0; i < tests; i++) {
    init_completion(&(args[i].comp));
    args->lock = &mutex[i];
    args->num_thread = threads;
    tid[i] = kthread_run(perf_mutex, (void *) args, "bench_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return; 
    }
  }

  for (i = 0; i < tests; i++)
    wait_for_completion(&(args[i].comp));

  return;
}
