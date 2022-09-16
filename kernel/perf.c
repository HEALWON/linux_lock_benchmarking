#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include "common.h"
#include "perf_spinlock.h"
#include "perf_mutex.h"

static int threads = 4;
module_param(threads, int, 0660);

static int tests = 10;
module_param(tests, int, 0660);

static char *ltype = "spinlock";
module_param(ltype, charp, 0660);

static int mod_init(void)
{
    pr_info("Installing...");

    if (strcmp(ltype, "spinlock") == 0) {
        //perf_spinlock_multiple(threads, tests);
    } else if (strcmp(ltype, "mutex") == 0) {
        perf_mutex_multiple(threads, tests);
    } else {
        pr_info("Not supported lock type: %s\n", ltype);
    }

    return 0;
}

static void mod_exit(void)
{
    pr_info("Uninstalling...\n");
}

MODULE_DESCRIPTION("kernel lock performance");
MODULE_AUTHOR("YW");
MODULE_LICENSE("GPL");

module_init(mod_init);
module_exit(mod_exit);