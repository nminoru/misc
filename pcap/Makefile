TARGET = pcap-test01 pcap-findalldevs
CFLAGS = -g -Wall -D_GNU_SOURCE -O1

ALL: $(TARGET)

pcap-test01: pcap-test01.c
	gcc $(CFLAGS) -lpcap $< -o $@

pcap-findalldevs: pcap-findalldevs.c
	gcc $(CFLAGS) -lpcap $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: clean
