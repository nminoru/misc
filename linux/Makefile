all: backtrace gprof_test

TARGETS = backtrace gprof_test

backtrace: backtrace.c backtrace_main.c
	gcc -g -O0 -rdynamic -D_GNU_SOURCE $^ -o $@

gprof_test: gprof_test.c
	gcc -g -pg -rdynamic -lpthread -D_GNU_SOURCE $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
