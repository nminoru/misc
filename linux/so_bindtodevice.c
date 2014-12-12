/*
 *  setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE) の効果の確認プログラム
 *
 *  同一ホスト内の全てのネットワークインターフェイスに同一ポート番号の
 *  UDP ソケットを作り、互いにダイアグラムを投げ合って送信 IP アドレス
 *  を確認する。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/prctl.h>
#include <ifaddrs.h>
#include <net/if.h> /* for IFNAMSIZ */ 


// #define AUTO_PORT 

enum {
    PORT_NUMBER   = 50000,
    MAX_PROCESSES =    32,
    MAX_NICS      =    32,
};


static int epoll_fd;

static int num_nics;

static struct {
    char name[IFNAMSIZ];
    struct sockaddr_in addr;
} nics[MAX_NICS];

static struct {
    int sockfd;
    struct sockaddr_in addr;
} ifs[MAX_NICS];


static void scan_nic(void);
static void create_sockets(int bindtodevice, int portnum);
static void do_client_task(void);
static void do_recvmsg(void);
static void do_test(void);


int main(int argc, char **argv)
{
    int bindtodevice = 0;

    if (argc >= 2) {
        if (strncasecmp(argv[1], "bind", 4) == 0) {
            bindtodevice = 1;
        }
    }
    
    printf("bindtodevice: %d\n", bindtodevice);

    scan_nic();

    assert(num_nics > 0);
    
    int i;
    for (i=0 ; i<MAX_PROCESSES ; i++) {
        pid_t pid;

        pid = fork();
        
        if (pid == 0) {
            nice(+10);

            prctl(PR_SET_PDEATHSIG, SIGKILL);

            epoll_fd = epoll_create(MAX_NICS);
            assert(epoll_fd >= 0);

            create_sockets(bindtodevice, PORT_NUMBER + i);

            do_client_task();

            exit(EXIT_SUCCESS);
        }
    }

    epoll_fd = epoll_create(MAX_NICS);
    assert(epoll_fd >= 0);

    create_sockets(bindtodevice, PORT_NUMBER - 1);

    do_test();

    printf("*\n");

    return 0;
}


static void scan_nic(void)
{
    int ret;
    struct ifaddrs *ifaddr, *ifa;

    ret = getifaddrs(&ifaddr);
    assert(ret == 0);
    assert(ifaddr != NULL);

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

        if (ifa->ifa_addr == NULL)
            continue;

#if 1
        // "ibX" だけを使う
        if ((strncmp(ifa->ifa_name, "ib",  2) != 0) &&
            (strncmp(ifa->ifa_name, "eth", 3) != 0))
            continue;

        if (strncmp(ifa->ifa_name, "virbr", strlen("virbr")) == 0)
            continue;
#endif

        int family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            nics[num_nics].addr = *(const struct sockaddr_in*)ifa->ifa_addr;
            strcpy(nics[num_nics].name, ifa->ifa_name);

            printf("%s: %s\n", ifa->ifa_name, inet_ntoa(nics[num_nics].addr.sin_addr));
            num_nics++;
        }
    }

    freeifaddrs(ifaddr);

    printf("\n");
}


static void create_sockets(int bindtodevice, int portnum)
{
    int i, ret;

    for (i=0 ; i<num_nics ; i++) {
        int sockfd;

        sockfd = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
        assert(sockfd >= 0);

        ifs[i].sockfd        = sockfd;
        ifs[i].addr          = nics[i].addr;
        ifs[i].addr.sin_port = htons(portnum);

        if (bindtodevice) {
            ret = setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, nics[i].name, IFNAMSIZ);

            if (ret != 0) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
        }

        // printf("%s:%u\n", inet_ntoa(ifs[i].addr.sin_addr), portnum);

        ret = bind(sockfd, (const struct sockaddr*)&ifs[i].addr, sizeof(ifs[i].addr));
        assert(ret == 0);

#if 0
        socklen_t socklen = sizeof(struct sockaddr_in);
        ret = getsockname(sockfd, (struct sockaddr*)&ifs[i].addr, &socklen);
        assert(ret == 0);
#endif

	struct epoll_event event;
	event.events   = EPOLLIN;
	event.data.u32 = i;

	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event);
	assert(ret == 0);
    }
}


static void do_client_task(void)
{
    for (;;) {
        int i, num_events;
        struct epoll_event events[MAX_NICS];

    retry_epoll_wait:
        num_events = epoll_wait(epoll_fd, events, MAX_NICS, 0);

        if (num_events < 0) {
            if (errno == EINTR)
                goto retry_epoll_wait;
        }

        if (num_events == 0)
            continue;

        for (i=0 ; i<num_events ; i++) {
            int ret;
            int receiver = events[i].data.u32;

            struct sockaddr_in data[1024];
            struct iovec    iov;

            iov.iov_base = (void*)data;
            iov.iov_len  = sizeof(data);
            
            struct sockaddr_in sockaddr;

            struct msghdr   msghdr;
            memset(&msghdr, 0, sizeof(msghdr));
            msghdr.msg_name       = &sockaddr;
            msghdr.msg_namelen    = sizeof(sockaddr);
            msghdr.msg_iov        = &iov;
            msghdr.msg_iovlen     = 1;

        retry_recvmsg:
            ret = recvmsg(ifs[receiver].sockfd, &msghdr, 0);
            if (ret < 0) {
                switch (errno) {
                case EINTR:
                    goto retry_recvmsg;

                case EAGAIN:
                    goto next_socket;

                default:
                    perror("recvmsg");
                    exit(EXIT_FAILURE);
                    break;
                }
            }
        
            char buffer1[256];
            char buffer2[256];

            if (sockaddr.sin_addr.s_addr != data[0].sin_addr.s_addr) {
                inet_ntop(AF_INET, &data[0].sin_addr.s_addr,     buffer1, sizeof(buffer1));
                inet_ntop(AF_INET, &sockaddr.sin_addr.s_addr,    buffer2, sizeof(buffer2));
        
                printf("Incorrect Sender: orig %s:%u -> real %s:%u\n",
                       buffer1, ntohs(data[0].sin_port), buffer2, ntohs(sockaddr.sin_port));
            }
        
            if (ifs[receiver].addr.sin_addr.s_addr != data[1].sin_addr.s_addr) {
                inet_ntop(AF_INET, &data[1].sin_addr.s_addr,     buffer1, sizeof(buffer1));
                inet_ntop(AF_INET, &ifs[receiver].addr.sin_addr.s_addr, buffer2, sizeof(buffer2));

                printf("Incorrect Receiver: orig %s:%u -> real %s:%u\n",
                       buffer1, ntohs(data[1].sin_port), buffer2, ntohs(ifs[receiver].addr.sin_port));
            }

            data[2] = data[0];
            data[0] = data[1];
            data[1] = data[2];

        retry_sendmsg:
            ret = sendmsg(ifs[receiver].sockfd, &msghdr, 0);
            if (ret < 0) {
                switch (errno) {
                case EINTR:
                    goto retry_sendmsg;

                case EAGAIN:
                    usleep(1);
                    goto retry_sendmsg;

                default:
                    perror("recvmsg");
                    exit(EXIT_FAILURE);
                    break;
                }
            }

        next_socket:
            ;
        }
    }
}


static void do_test(void)
{
    int i;
    for (i=0 ; i<10000 ; i++) {
        int j, ret;

        for (j=0 ; j<100 ; j++) {
            int nic_id = rand() % num_nics;
            
            struct sockaddr_in data[1024];

            // self
            data[0] = nics[nic_id].addr;
            data[0] .sin_port = htons(PORT_NUMBER - 1);

            // send to 
            data[1] = nics[rand() % num_nics].addr;
            data[1] .sin_port = htons(PORT_NUMBER + (rand() % MAX_PROCESSES));

            struct iovec    iov;
            iov.iov_base = (void*)data;
            iov.iov_len  = sizeof(data);

            struct msghdr   msghdr;
            memset(&msghdr, 0, sizeof(msghdr));
            msghdr.msg_name       = &data[1];
            msghdr.msg_namelen    = sizeof(struct sockaddr_in);
            msghdr.msg_iov        = &iov;
            msghdr.msg_iovlen     = 1;

        retry:
            ret = sendmsg(ifs[nic_id].sockfd, &msghdr, 0);

            if (ret < 0) {
                int errsv = errno;

                switch (errsv) {
                case EINTR:
                    goto retry;

                case EAGAIN:
                    usleep(1);
                    goto retry;

                default:
                    perror("sendmsg");
                    fprintf(stderr, "%s:%u\n",
                            inet_ntoa(ifs[nic_id].addr.sin_addr),
                            ntohs(ifs[nic_id].addr.sin_port));
                    exit(EXIT_FAILURE);
                }
            }
        }

        do_recvmsg();
    }
}


static void do_recvmsg(void)
{
    int status;
    pid_t pid;
    
    pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    int i, num_events;
    struct epoll_event events[MAX_NICS];

retry:
    num_events = epoll_wait(epoll_fd, events, MAX_NICS, 0);
    if (num_events < 0) {
        if (errno == EINTR) {
            goto retry;
        }
    }

    if (num_events == 0)
        return;

#if 0
    // 次の処理で recvmsg をするソケット順をシャッフルする 
    for (i = 0 ; i<num_events-1 ; i++) {
        int j = (rand() % (num_events - i)) + i;

        if (i == j)
            continue;

        // i と j を入れ替える
        struct epoll_event tmp;
        tmp       = events[i];
        events[i] = events[j];
        events[j] = tmp;
    }
#endif

    for (i=0 ; i<num_events ; i++) {
        int ret;
        int receiver = events[i].data.u32;

        struct sockaddr_in data[1024];
        struct iovec    iov;

        iov.iov_base = (void*)data;
        iov.iov_len  = sizeof(data);
            
        struct sockaddr_in sockaddr;

        struct msghdr   msghdr;
        memset(&msghdr, 0, sizeof(msghdr));
        msghdr.msg_name       = &sockaddr;
        msghdr.msg_namelen    = sizeof(sockaddr);
        msghdr.msg_iov        = &iov;
        msghdr.msg_iovlen     = 1;

    retry2:
        ret = recvmsg(ifs[receiver].sockfd, &msghdr, 0);
        if (ret < 0) {
            switch (errno) {
            case EINTR:
                goto retry2;

            case EAGAIN:
                goto next;

            default:
                perror("recvmsg");
                exit(EXIT_FAILURE);
                break;
            }
        }
        
        char buffer1[256];
        char buffer2[256];

        if (sockaddr.sin_addr.s_addr != data[0].sin_addr.s_addr) {
            inet_ntop(AF_INET, &data[0].sin_addr.s_addr,     buffer1, sizeof(buffer1));
            inet_ntop(AF_INET, &sockaddr.sin_addr.s_addr,    buffer2, sizeof(buffer2));
        
            printf("Incorrect Sender: orig %s:%u -> real %s:%u\n",
                   buffer1, ntohs(data[0].sin_port), buffer2, ntohs(sockaddr.sin_port));
        }
        
        if (ifs[receiver].addr.sin_addr.s_addr != data[1].sin_addr.s_addr) {
            inet_ntop(AF_INET, &data[1].sin_addr.s_addr,     buffer1, sizeof(buffer1));
            inet_ntop(AF_INET, &ifs[receiver].addr.sin_addr.s_addr, buffer2, sizeof(buffer2));

            printf("Incorrect Receiver: orig %s:%u -> real %s:%u\n",
                   buffer1, ntohs(data[1].sin_port), buffer2, ntohs(ifs[receiver].addr.sin_port));
        }

        next:
        ;
    }
}
