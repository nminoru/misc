TARGETS = mongotest

ALL: $(TARGETS)

MONGO_INCLUDE = $(shell pkg-config --cflags libmongoc-1.0)
SHLIB_LINK = $(shell pkg-config --libs libmongoc-1.0)

mongotest: mongotest.c
	gcc -O0 -g -Wall $(MONGO_INCLUDE) $(SHLIB_LINK) $< -o $@

clean:
	-rm -f $(TARGETS) *.o *~

.PHONY: clean
