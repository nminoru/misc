TARGETS = collating strcoll1 strcoll2 strxfrm1 strxfrm2 strxfrm3 strxfrm4 strxfrm5 strxfrm6
CFLAGS  = -g -O0 -Wall -D_GNU_SOURCE

all: $(TARGETS)

$(TARGETS): %: %.c 
	$(CC) $(CFLAGS) -lpthread $< -o $@

clean:
	rm -f $(TARGETS) *.o *~

.PHONY: clean
