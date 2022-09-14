obj-m += bench.o
bench-objs += perf.o perf_spinlock.o perf_mutex.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
