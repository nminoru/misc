TARGETS = main libtest1.so libtest2.so

ALL: $(TARGETS)

CFLAGS = -g -Wall

libtest1.so: test1.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@

libtest2.so: test2.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@

main: main.c libtest1.so libtest2.so
	$(CC) $(CFLAGS) main.c -o $@ -L .  -Wl,-rpath=. -ltest1 -ltest2

clean:
	rm -f $(TARGETS)

.PHONY: all clean
