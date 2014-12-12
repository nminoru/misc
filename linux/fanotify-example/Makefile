KERNEL_SOURCE := /lib/modules/$(shell uname -r)/source/usr

CPPFLAGS := -I$(KERNEL_SOURCE)/include -D_GNU_SOURCE
CFLAGS := -g -Wall -W -Wextra -O2
LDFLAGS := $(CFLAGS)

all: fanotify

fanotify: fanotify.o
fanotify.c: $(KERNEL_SOURCE)/include/linux/fanotify.h fanotify-syscalllib.h

clean:
	rm -f fanotify fanotify.o *.orig *.rej
