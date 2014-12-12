#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pcap/pcap.h>


static int make_address_string(char *buffer, struct sockaddr *addr, const char *tagname);


int main(int argc, char **argv)
{
    pcap_if_t *first_if;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&first_if, errbuf) < 0) {
        fprintf(stderr, "pcap_findalldevs: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    pcap_if_t *cur_if;
    for (cur_if = first_if ; cur_if ; cur_if = cur_if->next) {
        printf("name = %s, descriptoin=%s, flags=%x\n",
               cur_if->name, cur_if->description, cur_if->flags);

        struct pcap_addr *cur_addr;
        for (cur_addr = cur_if->addresses ; cur_addr ; cur_addr = cur_addr->next) {
            int ret = 0;
            char buffer[256];

            assert(cur_addr->addr);

            ret += sprintf(buffer + ret, "\t");

            ret += make_address_string(buffer + ret, cur_addr->addr, "addr");

            if (cur_addr->netmask)
                ret += make_address_string(buffer + ret, cur_addr->netmask, "netmask");

            if (cur_addr->broadaddr)
                ret += make_address_string(buffer + ret, cur_addr->broadaddr, "broadaddr");

            if (cur_addr->dstaddr)
                ret += make_address_string(buffer + ret, cur_addr->dstaddr, "dstaddr");

            buffer[ret -2] = '\0'; // cut tail ", "

            puts(buffer);
        }
    }  

    pcap_freealldevs(first_if);

    return 0;
}


static int make_address_string(char *buffer, struct sockaddr *addr, const char *tagname)
{
    switch (addr->sa_family) {

    case AF_INET:
        return sprintf(buffer, "%s: %08x, ",
                       tagname, ntohl(((struct sockaddr_in*)addr)->sin_addr.s_addr));

    case AF_INET6:
        return sprintf(buffer, "%s: PF_INET6, ",
                       tagname);

    case PF_PACKET:
        return sprintf(buffer, "%s: PF_PACKET, ",
                       tagname);

    default:
        return sprintf(buffer, "%s: sa_family=%d, ",
                       tagname, addr->sa_family);
    }
}

