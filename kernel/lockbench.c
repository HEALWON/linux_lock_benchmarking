#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include "lockbench.h"

static struct mutex mutex[MAX_TEST];
static spinlock_t spinlock[MAX_TEST];
static atomic_t barrier[MAX_TEST]; 
static int var[MAX_TEST];

typedef struct {
  struct completion comp;
  struct completion *ready;
  int num_thread;
  int idx;
} args_t;

static int writer_func_mutex(void *data)
{
  int i;
#ifdef SPIN_CRITICAL_SECTION
  int j;
#endif

  struct completion *comp = &(((args_t *) data)->comp);
  struct completion *ready = ((args_t *) data)->ready;
  int num_thread = ((args_t *) data)->num_thread;
  int idx = ((args_t *) data)->idx;

  atomic_inc(&barrier[idx]);
  wait_for_completion(ready);

  for (i = 0; i < ITER; i++) {
    while (mutex_lock_interruptible(&mutex[idx]) != 0) {;}    
    var[idx]++;
#ifdef DELAY_CRITICAL_SECTION
    ndelay(DURATION);
#endif
#ifdef SPIN_CRITICAL_SECTION
    for (j = 0; j < ITER_SPIN; j++) {;}
#endif
    mutex_unlock(&mutex[idx]);
  }

  complete_and_exit(comp, 0);
}
static int writer_func_spin(void *data)
{
  int i;
#ifdef SPIN_CRITICAL_SECTION
  int j;
#endif

  struct completion *comp = &(((args_t *) data)->comp);
  struct completion *ready = ((args_t *) data)->ready;
  int num_thread = ((args_t *) data)->num_thread;
  int idx = ((args_t *) data)->idx;

  atomic_inc(&barrier[idx]);
  wait_for_completion(ready);

  for (i = 0; i < ITER; i++) {
    spin_lock(&spinlock[idx]);
    var[idx]++;
#ifdef DELAY_CRITICAL_SECTION
    ndelay(DURATION);
#endif
#ifdef SPIN_CRITICAL_SECTION
    for (j = 0; j < ITER_SPIN; j++) {;}
#endif
    spin_unlock(&spinlock[idx]);
  }

  complete_and_exit(comp, 0);
}

char *name_list[] = {
  "mutex",
  "spinlock"
};
int (*func_list[]) (void *) = {
  writer_func_mutex,
  writer_func_spin,
};

static int bench(LockType_t locktype, int threads, int idx)
{
  struct task_struct *tid[MAX_THREAD];
  args_t args[MAX_THREAD];
  struct completion ready;
  ktime_t start, end, elapsed;
  int i;

  int (*func) (void *) = func_list[locktype];

  init_completion(&ready);
  mutex_init(&mutex[idx]);
  spin_lock_init(&spinlock[idx]);
  atomic_set(&barrier[idx], 0);

  for (i = 0; i < threads; i++) {
    init_completion(&(args[i].comp));
    args[i].ready = &ready;
    args[i].num_thread = threads;
    args[i].idx = idx;
    
    tid[i] = kthread_run(func, (void *) &args[i], "worker_thread");
    if (IS_ERR(tid[i])) {
      pr_err("pthread create error");
      return 0;
    }
  }

  while (atomic_read(&barrier[idx]) < threads) {;}

  start = ktime_get();
  complete_all(&ready);

  for (i = 0; i < threads; i++)
    wait_for_completion(&(args[i].comp));

  end = ktime_get();
  elapsed = end - start;

  if (var[idx] != threads * ITER)
    pr_err("synchronization failed\n");
  // printk("(test %d) Elapsed time: %lld.%09lld (s)\n", idx, elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);
  else 
    printk("%lld.%09lld\n", elapsed/NSEC_PER_SEC, elapsed%NSEC_PER_SEC);

  return 0;
}

void bench_multiple(LockType_t locktype, int threads, int tests)
{
  int i;

  if ((threads > MAX_THREAD) || (tests > MAX_TEST)) {
    printk("arguments are too large\n");
    return;
  }

  printk("Benchmarking %s... (threads = %d, tests = %d)\n", name_list[locktype], threads, tests);

  for (i = 0; i < tests; i++) {
    bench(locktype, threads, i);
  }

  return;
}

/*
void perf_mutex_multiple(int threads, int tests) 
{
  struct task_struct *tid[MAX_TEST];
  struct mutex mutex[MAX_TEST];
  int var[MAX_TEST];
  int i;
  args_t args[MAX_TEST];

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
*/
