/*
 *  File: pcap-test01.c
 *
 *  Copyright (C) 2013 NAKAMURA Minoru <nminoru@nminoru.jp
 *
 *  [Compile]
 *    gcc -D_GNU_SOURCE -Wall -O3 -g -lpcap -lrt -o pcap-test01 pcap-test01.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>


static pcap_t *open_net_dev(const char *dev);
static int select_pcap(int selectable_fd);
static int do_packet(pcap_t *handle);


static volatile int polling_flag;


int main(int argc, char **argv)
{    
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "%s at %s(%u)\n", errbuf, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    pcap_t *handle;
    handle = open_net_dev(dev);

    int selectable_fd = pcap_get_selectable_fd(handle);

    for (;;) {
        do {
            if (select_pcap(selectable_fd) == 0) {
                do {
                    if (do_packet(handle))
                        break;
                } while (polling_flag == 0);
            }
	} while (polling_flag == 0);
    }

    pcap_close(handle);

    return 0;
}


static pcap_t *open_net_dev(const char *dev)
{
    struct bpf_program fp;
    char filter_exp[] = "tcp";
    char errbuf[PCAP_ERRBUF_SIZE];

    /* Device attribute */
    bpf_u_int32 mask;
    bpf_u_int32 net;
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "%s %s at %s(%u)\n", dev, errbuf, __FILE__, __LINE__);
        net = 0;
        mask = 0;
    }

    pcap_t *handle;
    handle = pcap_open_live(dev, 68, 0 /* false */, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "%s %s at %s(%u)\n", dev, errbuf, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "%s %s at %s(%u)\n", filter_exp, pcap_geterr(handle), __FILE__, __LINE__); 
        exit(EXIT_FAILURE);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "%s %s at %s(%u)\n", filter_exp, pcap_geterr(handle), __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (pcap_setnonblock(handle, 1, errbuf)) {
        fprintf(stderr, "%s %s at %s(%u)\n", dev, errbuf, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    return handle;
}


static int select_pcap(int selectable_fd)
{
    fd_set setread, setexcept;
    struct timeval seltimeout;

    FD_ZERO(&setread);
    FD_SET(selectable_fd, &setread);
    FD_ZERO(&setexcept);
    FD_SET(selectable_fd, &setexcept);

    seltimeout.tv_sec = 1;
    seltimeout.tv_usec = 0;
    int status = select(selectable_fd + 1, &setread,
                        NULL, &setexcept, &seltimeout);
    
    if (status == 0) {
        // time out
        return -1;
    } else if (status == -1) {
        int eno = errno;
        if (eno == EINTR) {
            return -1;
        }
        printf("Select returns error (%d)\n", eno);
    } else {

#if 0
        if (FD_ISSET(selectable_fd, &setread))
            printf("readable, ");
        else
            printf("not readable, ");

        if (FD_ISSET(selectable_fd, &setexcept))
            printf("exceptional condition\n");
        else
            printf("no exceptional condition\n");
#endif
    }
    
    return 0;
}


static int do_packet(pcap_t *handle)
{
    struct pcap_pkthdr header;
    const u_char *packet;

    // u_char *pkt_data;
    packet = pcap_next(handle, &header);
    if (packet == NULL) {
        int eno = errno;
        if (eno == EINTR || eno == EAGAIN) {
            return -1;
        }
        fprintf(stderr, "pcap_next: %d\n", eno);
        exit(EXIT_FAILURE);	      
    }

    if (header.len < sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr)) {
        printf("header.len = %d\n", header.len);
    }

    struct ether_header *eptr;
    eptr = (struct ether_header *)packet;

    if (ntohs(eptr->ether_type) == ETHERTYPE_IP) {
    } else {
        return 0;
        //            fprintf(stderr, "%u %x: %s(%u)\n", ntohs(eptr->ether_type), ntohs(eptr->ether_type), __FILE__, __LINE__); 
        //            exit(1);
    }

    const struct iphdr *iphdr = (const struct iphdr*)(eptr + 1);
    const struct tcphdr *tcphdr = (const struct tcphdr *)(iphdr + 1);

    uint32_t saddr, daddr;
    saddr = ntohl(iphdr->saddr);
    daddr = ntohl(iphdr->daddr);

    uint16_t sport, dport;
    sport = ntohs(tcphdr->source);
    dport = ntohs(tcphdr->dest);

#if 0
    struct pcap_pkthdr {
	struct timeval ts;	/* time stamp */
	bpf_u_int32 caplen;	/* length of portion present */
	bpf_u_int32 len;	/* length this packet (off wire) */
    };

    struct ether_header
    {
        u_int8_t  ether_dhost[ETH_ALEN];	/* destination eth addr	*/
        u_int8_t  ether_shost[ETH_ALEN];	/* source ether addr	*/
        u_int16_t ether_type;		        /* packet type ID field	*/
    } __attribute__ ((__packed__));

#define	ETHERTYPE_IP		0x0800		/* IP */
#define	ETHERTYPE_IPV6		0x86dd		/* IP protocol version 6 */

#endif


    char buf[255];

    time_t time = header.ts.tv_sec;
    struct tm tm;
    localtime_r(&time, &tm);

    strftime(buf, sizeof(buf), "%d %b %Y %H:%M", &tm);

    puts(buf);


    printf("%lu.%06u caplen=%u len=%u\n", (long)header.ts.tv_sec, (int)header.ts.tv_usec, header.caplen, header.len);
    printf("S:%08x:%u D:%08x:%u\n", saddr, sport, daddr, dport);

    return 0;
}
