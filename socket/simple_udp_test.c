/*
 *  ソケット通信のテスト
 *
 *  gcc -D_GNU_SOURCE -Wall -O3 -g simple_udp_test.c -o simple_udp_test
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>


static struct option longopts[] = {
    {"server",     no_argument,       NULL, 'S' },
    {"targethost", required_argument, NULL, 'C' },
    {"bindhost",   required_argument, NULL, 'B' },
};


enum {
    PORT_NUMBER = 13000
};


static int do_server(void);
static int do_client(void);

static const char* bindhostname = "localhost";
static const char* targethostname = "localhost";

int main(int argc, char** argv)
{
    int ch, option_index;
    int is_server = 0;
    int is_client = 0;

    while ((ch = getopt_long(argc, argv, "SC:B:", longopts, &option_index)) != -1) {
        switch(ch) {
        case 'S':
            is_server = 1;
            break;

        case 'C':
            is_client = 1;
            targethostname = optarg;
            break;

        case 'B':
            bindhostname = optarg;
            break;

        default:
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (is_server)
        do_server();
    else if (is_client)
        do_client();

    return 0;
}


static int do_client(void)
{
    int ret, sockfd;
    struct sockaddr_in myaddr;
    struct sockaddr_in youraddr;
    struct hostent     *hp;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        int eno  = errno;
        fprintf(stderr, "socket (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    hp = gethostbyname(bindhostname);
    if (hp == NULL) {
        int eno  = errno;
        fprintf(stderr, "gethostbyname (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = PF_INET;
    bcopy(hp->h_addr, &myaddr.sin_addr.s_addr, hp->h_length);
    myaddr.sin_port        = htons(0);

    hp = gethostbyname(targethostname);
    if (hp == NULL) {
        int eno  = errno;
        fprintf(stderr, "gethostbyname (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    memset(&youraddr, 0, sizeof(youraddr));
    youraddr.sin_family  = PF_INET;
    bcopy(hp->h_addr, &youraddr.sin_addr.s_addr, hp->h_length);
    youraddr.sin_port    = htons(PORT_NUMBER);

    ret = bind(sockfd, (struct sockaddr*)&myaddr, (socklen_t)sizeof(myaddr));
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "bind (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ssize_t account = 1024 * 1024 * 1024;

    do {
        ssize_t size;
        char buffer[1024];

        size = sendto(sockfd, buffer, sizeof(buffer), 0,
                      (const struct sockaddr*)&youraddr, (socklen_t)sizeof(myaddr));
        if (size > 0)
            account -= size;

    } while (account > 0);

    close(sockfd);
    
    return 0;
}


static int do_server(void)
{
    int ret, sockfd;
    struct sockaddr_in myaddr;
    struct hostent     *hp;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        int eno  = errno;
        fprintf(stderr, "socket (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    hp = gethostbyname(bindhostname);
    if (hp == NULL) {
        int eno  = errno;
        fprintf(stderr, "gethostbyname (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = PF_INET;
    bcopy(hp->h_addr, &myaddr.sin_addr.s_addr, hp->h_length);
    myaddr.sin_port        = htons(PORT_NUMBER);

    ret = bind(sockfd, (struct sockaddr*)&myaddr, (socklen_t)sizeof(myaddr));
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "bind (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ssize_t account = 1024 * 1024 * 1024;

    do {
        ssize_t size;
        char buffer[1024];
        struct sockaddr_in youraddr;
        socklen_t youraddrlen;

        size = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                        (struct sockaddr*)&youraddr, &youraddrlen);

        if (size > 0)
            account -= size;

    } while (account > 0);

    close(sockfd);
    
    return 0;
}

