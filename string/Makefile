TARGETS = strcoll strxfrm1 strxfrm2 strxfrm3 strxfrm4 strxfrm5
CFLAGS  = -g -O0 -Wall -D_GNU_SOURCE

all: $(TARGETS)

strcoll: strcoll.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

strxfrm1: strxfrm1.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

strxfrm2: strxfrm2.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

strxfrm3: strxfrm3.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

strxfrm4: strxfrm4.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

strxfrm5: strxfrm5.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

clean:
	rm -f $(TARGETS) *.o *~

.PHONY: clean
