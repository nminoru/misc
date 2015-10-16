ALL: alias-test

alias-test: alias-test1.c alias-test2.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	-rm -f alias-test *.o

.PHONY: clean

