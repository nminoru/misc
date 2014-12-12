ALL: test1 test2 test3

test1: test1.c
	gcc -O0 -g -Wall $< -o $@

test2: test2.c
	gcc -O0 -g -Wall $< -o $@

test3: test3.c
	gcc -O0 -g -Wall -lpthread $< -o $@

.PHONY: ALL
