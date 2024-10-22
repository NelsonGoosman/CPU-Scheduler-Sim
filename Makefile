CC = gcc
CFLAGS = -g -Wall -Werror

OBJS = scheduler.o rand.o utils.o process_sim.o

scheduler: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o scheduler

scheduler.o: scheduler.c scheduler.h rand.h utils.h process_sim.h process_structs.h
	$(CC) $(CFLAGS) -c scheduler.c

rand.o: rand.c rand.h
	$(CC) $(CFLAGS) -c rand.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

process_sim.o: process_sim.c process_sim.h
	$(CC) $(CFLAGS) -c process_sim.c

test01: scheduler
	./scheduler sample_io/input/input-1

test02: scheduler
	./scheduler sample_io/input/input-2

test03: scheduler
	./scheduler sample_io/input/input-3

clean:
	rm -f scheduler *.o *~
