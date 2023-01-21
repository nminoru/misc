TARGETS = json-test setfile-test sscanf-test string-test time-test

all: $(TARGETS)

bigcache-test json-test setfile-test sscanf-test string-test time-test: %: %.go
	go build $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
