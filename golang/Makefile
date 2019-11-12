TARGETS = bigcache-test sscanf-test json-test

all: $(TARGETS)

lsxattr setxattr setxattr2 access append_write: %: %.go
	go build $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
