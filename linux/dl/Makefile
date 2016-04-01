TARGETS = main dummy_so.so hook.so

ALL: $(TARGETS)

main: main.c dummy_so.so 
	$(CC) -g -Wall -ldl -Wl,--as-need,--enable-new-dtags,-E $< -o $@

dummy_so.so: dummy_so.c 
	$(CC) -g -Wall -shared -fpic -Wl,--as-need,--enable-new-dtags $< -o $@

hook.so: hook.c
	$(CC) -g -Wall -shared -fpic -Wl,--as-need,--enable-new-dtags $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
