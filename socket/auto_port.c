/*
 *  自動割り付けしたポート番号の確認
 *
 *  gcc -D_GNU_SOURCE -Wall -O0 -g auto_port.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int ret, sockfd;
    struct sockaddr_in myaddr;


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = PF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port        = htons(0);

    if ((ret = bind(sockfd, (const struct sockaddr *)&myaddr, (socklen_t)sizeof(myaddr))) == -1) { 
        perror("socket");
        exit(EXIT_FAILURE);
    }

    socklen_t addrlen = (socklen_t)sizeof(myaddr);
    if ((ret = getsockname(sockfd, (struct sockaddr *)&myaddr, &addrlen)) == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }

    printf("IP Address:%08x\n", myaddr.sin_addr.s_addr);
    printf("Port:%u\n",        ntohs(myaddr.sin_port));

    return 0;
}
