TARGETS = printf-1000sep langinfo cond_wait setlocale mkfifo pipe memory-mapped-file shm_open fd-passing sjlj_anomaly overcommit
CFLAGS  = -g -O0 -Wall -D_GNU_SOURCE

all: $(TARGETS)

printf-1000sep langinfo mkfifo pipe memory-mapped-file overcommit: %: %.c
	$(CC) $(CFLAGS) $< -o $@

shm_open fd-passing: %: %.c
	$(CC) -lrt $(CFLAGS) $< -o $@

cond_wait: cond_wait.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

setlocale: setlocale.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

sjlj_anomaly: sjlj_anomaly.c
	$(CC) $(CFLAGS) -O3 $< -o $@

clean:
	rm -f $(TARGETS) *.o *~

.PHONY: clean
