TARGETS = backtrace gprof_test lsxattr setxattr setxattr2 access append_write

all: $(TARGETS)

backtrace: backtrace.c backtrace_main.c
	gcc -g -O0 -rdynamic -D_GNU_SOURCE $^ -o $@

gprof_test: gprof_test.c
	gcc -g -pg -rdynamic -lpthread -D_GNU_SOURCE $< -o $@

lsxattr setxattr setxattr2 access append_write: %: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
