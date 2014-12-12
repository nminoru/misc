TARGETS = auto_port simple_tcp_test simple_udp_test \
	udp_test1 udp_test2 udp_test3

all: $(TARGETS)

auto_port udp_test1 udp_test2 udp_test3 udp_test4: %: %.c
	gcc -D_GNU_SOURCE -Wall -O0 -g $< -o $@

getifaddrs simple_tcp_test simple_udp_test: %: %.c
	gcc -D_GNU_SOURCE -Wall -O3 -g $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: clean
