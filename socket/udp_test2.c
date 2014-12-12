/*
 *  sendmsg で 0 バイトパケットは送信できるか？
 *  非 0 バイトパケット受信時に recvmsg に 0 が返ることがあるか？
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int sock_fd;
    struct sockaddr_in addr;    
} socket_t;


static void create_udp_socket(socket_t *socket, const char *ipaddress);


int main(int argc, char **argv)
{
    int ret;
    socket_t a, b;

    if (argc < 2) {
        printf("Usage: ip-address\n");
        exit(0);
    }

    create_udp_socket(&a, argv[1]);
    create_udp_socket(&b, argv[1]);

    char send_buffer[1024];

    struct iovec  send_iovec = {
        .iov_base = send_buffer,
        .iov_len  = sizeof(send_buffer),
    };

    struct msghdr send_msghdr = {
        .msg_name    = &b.addr,
        .msg_namelen = sizeof(b.addr),
        .msg_iov     = &send_iovec, 
        .msg_iovlen  = 1,
    };

    ret = sendmsg(a.sock_fd, &send_msghdr, 0);
    assert(ret == sizeof(send_buffer));

    for (;;) {
        char recv_buffer[1024];

        struct iovec recv_iovec = {
            .iov_base = recv_buffer,
            .iov_len  = sizeof(recv_buffer),
        };

        struct msghdr recv_msghdr = {
            .msg_name    = &a.addr,
            .msg_namelen = sizeof(a.addr),
            .msg_iov     = &recv_iovec, 
            .msg_iovlen  = 1,
        };

        ret = recvmsg(b.sock_fd, &recv_msghdr, 0);
        printf("recvmsg: ret = %d\n", ret);

        if (ret < 0) {
            int errsv = errno;
            if (errsv == EAGAIN) {
                break;
            } else {
                printf("recvmsg: errno=%d\n", errsv);
                exit(0);
            }
        }
    }

    return 0;
}


static void create_udp_socket(socket_t *result, const char *ipaddress)
{
    int ret, sock_fd;
    struct sockaddr_in addr;

    sock_fd = socket(PF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    assert(sock_fd > 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;

    ret = inet_pton(AF_INET, ipaddress, &addr.sin_addr);
    assert (ret == 1);

    ret = bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret == 0);

    socklen_t socklen = sizeof(addr);
    ret = getsockname(sock_fd, (struct sockaddr*)&addr, &socklen);
    assert(ret == 0);

    printf("%s:%u\n", ipaddress, ntohs(addr.sin_port));

    result->sock_fd = sock_fd;
    result->addr    = addr;
}
