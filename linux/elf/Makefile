TARGETS = libtestso.so dll_injection

ALL: $(TARGETS)

CFLAGS = -g -Wall

libtestso.so: testso.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@

dll_injection: dll_injection.c patch_got_entry.c libtestso.so
	$(CC) $(CFLAGS) dll_injection.c patch_got_entry.c -o $@ -L . -Wl,-rpath=. -ltestso 

clean:
	rm -f $(TARGETS)

.PHONY: all clean

