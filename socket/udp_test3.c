/*
 * ポート番号の衝突実験
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


enum {
    PORT_NUMBER = 50000
};

static void socket_and_bind(void);


int main(int argc, char **argv)
{
    int ret, sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd >= 0);

    struct sockaddr_in sockaddr_in;

    memset(&sockaddr_in, 0, sizeof(sockaddr_in));

    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port   = htons(PORT_NUMBER);

    ret = inet_pton(AF_INET, "192.168.102.5", &sockaddr_in.sin_addr.s_addr);
    assert(ret == 1);

    ret = bind(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
    assert(ret == 0);

    socket_and_bind();

    return 0;
}

static void socket_and_bind(void)
{
    int ret;
    struct ifaddrs *ifaddr, *ifa;

    ret = getifaddrs(&ifaddr);

    assert(ret    == 0);
    assert(ifaddr != NULL);

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        int family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            printf("%s\n", ifa->ifa_name);

            struct sockaddr_in sockaddr_in = *(struct sockaddr_in *)ifa->ifa_addr;

            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            assert(sockfd >= 0);

            sockaddr_in.sin_port = htons(PORT_NUMBER);

            int ret = bind(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
            if (ret < 0) {
                int errsv = errno;
                fprintf(stderr, "bind: %d\n", errsv);
                exit(EXIT_FAILURE);
            }
        }
    }

    freeifaddrs(ifaddr);
}
