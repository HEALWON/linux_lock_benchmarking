#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include "common.h"
#include "perf_spinlock.h"

static spinlock_t spinlock[MAX_TEST];
static atomic_t barrier[MAX_TEST]; 
static int var[MAX_TEST];

typedef struct {
  struct completion comp;
  int num_thread;
  int idx;
} args_t;

static int writer_func(void *data)
{
  int i;

  struct completion *comp = &(((args_t *) data)->comp);
  int num_thread = ((args_t *) data)->num_thread;
  int idx = ((args_t *) data)->idx;

  atomic_add(1, &barrier[idx]);
  while (atomic_read(&barrier[idx]) != num_thread + 1) {;}

  for (i = 0; i < ITER; i++) {
    spin_lock(&spinlock[idx]);
    var[idx]++;
    udelay(DURATION);
    spin_unlock(&spinlock[idx]);
  }

  complete_and_exit(comp, 0);
}

static int perf_spinlock(void *data)
{
  struct task_struct *tid[MAX_THREAD];
  args_t args[MAX_THREAD];
  ktime_t start, end, elapsed;
  int i;
  
  struct completion *comp = &(((args_t *) data)->comp);
  int num_thread = ((args_t *) data)->num_thread; 
  int idx = ((args_t *) data)->idx;

  spin_lock_init(&spinlock[idx]);
  atomic_set(&barrier[idx], 0);

  for (i = 0; i < num_thread; i++) {
    init_completion(&(args[i].comp));
    args[i].num_thread = num_thread;
    args[i].idx = i;
    tid[i] = kthread_run(writer_func, (void *) &args[i], "worker_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return 0;
    }
  }

  while (atomic_read(&barrier[idx]) != num_thread) {;}

  start = ktime_get();
  atomic_add(1, &barrier[idx]);

  for (i = 0; i < num_thread; i++)
    wait_for_completion(&(args[i].comp));

  end = ktime_get();
  elapsed = end - start;

  if (var[idx] != num_thread * ITER)
    pr_err("synchronization failed\n");
  // printk("(test %d) Elapsed time: %lld.%09lld (s)\n", idx, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);
  printk("%lld.%09lld\n", elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  complete(comp);

  return 0;
}

void perf_spinlock_multiple_seq(int threads, int tests) 
{
  int i;
  args_t args;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("Benchmarking spinlock... (threads = %d, tests = %d)\n", threads, tests);

  for (i = 0; i < tests; i++) {
    init_completion(&(args.comp));
    args.num_thread = threads;
    args.idx = i;
    perf_spinlock(&args);
  }

  return;
}

/*
void perf_spinlock_multiple(int threads, int tests) 
{
  struct task_struct *tid[MAX_TEST];
  spinlock_t spinlock[MAX_TEST];
  int var[MAX_TEST];
  int i;
  args_t *args;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("Benchmarking spinlock... (threads = %d, tests = %d)\n", threads, tests);

  for (i = 0; i < tests; i++) {
    init_completion(&(args[i].comp));
    args->lock = &spinlock[i];
    args->num_thread = threads;
    tid[i] = kthread_run(perf_spinlock, (void *) args, "bench_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return; 
    }
  }

  for (i = 0; i < tests; i++)
    wait_for_completion(&(args[i].comp));

  return;
}
*/