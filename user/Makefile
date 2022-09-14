CC = gcc
CFLAGS = -pthread

TARGET = perf_mutex perf_rwlock perf_spinlock
INC = common.c common.h

all: $(TARGET)

.PHONY: clean
clean: 
	rm -f $(TARGET)

perf_mutex: perf_mutex.c $(INC)
	$(CC) $(CFLAGS) -o $@ $^

perf_rwlock: perf_rwlock.c $(INC)
	$(CC) $(CFLAGS) -o $@ $^

perf_spinlock: perf_spinlock.c $(INC)
	$(CC) $(CFLAGS) -o $@ $^
