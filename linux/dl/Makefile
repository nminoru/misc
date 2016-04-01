TARGETS = main dummy_so.so

ALL: $(TARGETS)

main: main.c dummy_so.so 
	$(CC) -g -Wall -ldl $< -o $@

dummy_so.so: dummy_so.c 
	$(CC) -g -Wall -shared -fpic $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
