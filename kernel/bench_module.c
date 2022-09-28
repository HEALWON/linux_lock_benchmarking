#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include "lockbench.h"

static int threads = 4;
module_param(threads, int, 0660);

static int tests = 1;
module_param(tests, int, 0660);

static char *ltype = "mutex";
module_param(ltype, charp, 0660);

static int mod_init(void)
{
    pr_info("Installing...");

    if (strcmp(ltype, "mutex") == 0) {
        bench_multiple(LOCKTYPE_MUTEX, threads, tests);
    } else if (strcmp(ltype, "spinlock") == 0) {
        bench_multiple(LOCKTYPE_SPINLOCK, threads, tests);
    } else {
        pr_info("Not supported lock type: %s\n", ltype);
    }

    pr_info("Done\n");

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