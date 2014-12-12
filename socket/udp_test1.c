/*
 *  ソケット通信のテスト
 *
 *  gcc -D_GNU_SOURCE -Wall -O3 -g udp_test.c -o udp_test
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
    {"server",   no_argument,       NULL, 'S' },
    {"client",   no_argument,       NULL, 'c' },
    {"hostname", required_argument, NULL, 'h' },
};


enum {
    PORT_NUMBER = 13000
};


static int do_server(void);
static int do_client(void);

static const char* hostname = "localhost";

int main(int argc, char** argv)
{
    int ch, option_index;
    int is_server = 0;

    while ((ch = getopt_long(argc, argv, "Sch:", longopts, &option_index)) != -1) {
        switch(ch) {
        case 'S':
            is_server = 1;
            break;

        case 'c':
            is_server = 0;
            break;

        case 'h':
            hostname = optarg;
            break;
        }
    }

    if (is_server)
        do_server();
    else
        do_client();

    return 0;
}


static int do_client(void)
{
    int ret, fd;
    struct sockaddr_in myaddr;
    struct hostent     *hp;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        int eno  = errno;
        fprintf(stderr, "socket (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    hp = gethostbyname(hostname);
    if (hp == NULL) {
        int eno  = errno;
        fprintf(stderr, "gethostbyname (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = PF_INET;
    bcopy(hp->h_addr, &myaddr.sin_addr.s_addr, hp->h_length);
    myaddr.sin_port        = htons(PORT_NUMBER);

    ret = connect(fd, &myaddr, sizeof(myaddr));
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "connect (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    {
        char buffer[4096];

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "0123456789ABCDEF");

        write(fd, buffer, 4096);
        
        int size = 0;
        do {
            int ret = read(fd, buffer + size, 4096 - size);
            if (ret < 0) {
                int eno  = errno;
                fprintf(stderr, "connect (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
                exit(EXIT_FAILURE);
            }
            size += ret;
        } while (size < 4096);
        
        printf("***: %s\n", buffer);
    }

    close(fd);
    
    return 0;
}


static int do_server(void)
{
    int ret, listen_fd;
    struct sockaddr_in myaddr;

    //
    listen_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
    if (listen_fd == -1) {
        int eno  = errno;
        fprintf(stderr, "socket (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    // Socket を listen しているプログラムが終了しても、ポートは規定時間解放されない。
    // 以下の設定は前回の修了状態によらずただちにポートへの bind を成功させる。
    int on = 1;
    ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "setsockopt SOL_SOCKET, SO_REUSEADDR, (%d)  at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    socklen_t len2 = sizeof(on);
    ret = getsockopt(listen_fd, SOL_SOCKET, SO_PRIORITY, &on, &len2);
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "getsockopt SOL_SOCKET, SO_PRIORITY, (%d)  at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("SO_PRIORITY = %d, len = %d\n", on, len2);

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = PF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port        = htons(PORT_NUMBER);
    
    //
    ret = bind(listen_fd, &myaddr, sizeof(myaddr));
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "bind (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    //
    ret = listen(listen_fd, SOMAXCONN);
    if (ret == -1) {
        int eno  = errno;
        fprintf(stderr, "listen (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    //
    int conn_fd;
    struct sockaddr_in connect_addr;
    socklen_t len;
    conn_fd = accept(listen_fd, &connect_addr, &len);
    if (conn_fd == -1) {
        int eno  = errno;
        fprintf(stderr, "accept (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    {
        char buffer[4096];

        int size = 0;
        
        do {
            int ret = read(conn_fd, buffer + size, 4096 - size);
            if (ret < 0) {
                int eno  = errno;
                fprintf(stderr, "connect (%d) at %s(%u)\n", eno, __FILE__, __LINE__);
                exit(EXIT_FAILURE);
            }
            size += ret;
        } while (size < 4096);

        sprintf(buffer, "FEDCBA9876543210");
        write(conn_fd, buffer, 4096);
    }

    close(conn_fd);

    close(listen_fd);

    return 0;
}

