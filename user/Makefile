CC = gcc
CFLAGS = -pthread

TARGET = bench
OBJS = main.o lockbench.o spin.o

all: $(TARGET)

.PHONY: clean
clean: 
	rm -f *.o
	rm -f $(TARGET)

bench: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c
lockbench.o: lockbench.c lockbench.h
spin.o: spin.c spin.h
