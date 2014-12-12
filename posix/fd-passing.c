/*
 *  file descriptor passing と shm_open を組み合わせた処理
 *
 *  > gcc -g -o fd-passing -lrt fd-passing.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define UNIX_DOMAIN_PATH "/tmp/unix-domain-socket"
#define TEMPFILE_SIZE (0x1000000)

static int create_shm_file(void);
static int do_client(void);
static void do_server(int shmfd);
static void setnonblocking(int fd);
static int recv_file_descriptor(int socket);
static int send_file_descriptor(int socket, int fd_to_send);

int main(int argc, char **argv)
{
    pid_t pid;

    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);        
    } else if (pid == 0) {
        // クライアント(子プロセス)
        int shmfd;
        
        // Unix ドメインクライントを作り shmfd を受信
        shmfd = do_client();

        // shmfd を使って mmap() を行い共有メモリに書き込み
        void *addr;
        addr = mmap(NULL, TEMPFILE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
        if (addr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        sprintf((char*)addr, "ABCDEFG");

        close(shmfd);        
    }  else {
        // サーバー(親プロセス)

        // 共有ファイルの file descriptor を取得
        int shmfd;
        shmfd = create_shm_file();

        // Unix ドメインサーバを作り shmfd を送信
        do_server(shmfd);

        // 子プロセスの終了を待つ
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // shmfd を使って mmap() を行い共有メモリの内容を表示
        void *addr;
        addr = mmap(NULL, TEMPFILE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
        if (addr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        close(shmfd);

        // 子プロセスが書き込んだ内容が見えれば成功
        puts((const char*)addr);
    }

    return 0;
}

//
// shm_open() を使って新しい共有ファイルを作りそれをオープンする
// ただし unlink して共有ファイルは削除し、fd だけを返す。
//
static int
create_shm_file(void)
{
    int fd;
    char tempfile[NAME_MAX];

    snprintf(tempfile, sizeof(tempfile), "/fd-passing-shm-%d", getpid());

    if ((fd = shm_open(tempfile, O_RDWR|O_CREAT, 0644)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(fd, (off_t)TEMPFILE_SIZE)) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shm_unlink(tempfile);

    return fd;
}


static void
do_server(int shmfd)
{
    int listenfd;

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un saddr = {0};
    saddr.sun_family = AF_UNIX;

    strcpy(saddr.sun_path, UNIX_DOMAIN_PATH);

    unlink(saddr.sun_path);

    if (bind(listenfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        goto error;
    }

    if (listen(listenfd, 128) == -1) {
        perror("listen");
        goto error;
    }

    int sockfd;

    sockfd = accept(listenfd, NULL, NULL);
    if (sockfd == -1) {
        perror("accept");
        goto error;
    }

    setnonblocking(sockfd);

    send_file_descriptor(sockfd, shmfd);

    close(sockfd);
    close(listenfd);

    unlink(saddr.sun_path);

    return;

error:
    close(listenfd);
    exit(EXIT_FAILURE);
}

static int
do_client(void)
{
    int sockfd;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un saddr = {0};
    saddr.sun_family = AF_UNIX;

    strcpy(saddr.sun_path, UNIX_DOMAIN_PATH);

    if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        goto error;
    }

    setnonblocking(sockfd);

    int shmfd;

    while ((shmfd = recv_file_descriptor(sockfd)) == -1) {
        if (errno != EAGAIN) {
            perror("recv_file_descriptor");
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);

    return shmfd;

error:
    close(sockfd);
    exit(EXIT_FAILURE);

    return -1;
}

static void
setnonblocking(int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

static int
recv_file_descriptor(int socket)
{
    int sent_fd;
    struct msghdr message;
    struct iovec iov[1];
    struct cmsghdr *control_message = NULL;
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];
    int res;

    memset(&message, 0, sizeof(struct msghdr));
    memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

    /* For the dummy data */
    iov[0].iov_base = data;
    iov[0].iov_len = sizeof(data);

    message.msg_name = NULL;
    message.msg_namelen = 0;
    message.msg_control = ctrl_buf;
    message.msg_controllen = CMSG_SPACE(sizeof(int));
    message.msg_iov = iov;
    message.msg_iovlen = 1;

    if((res = recvmsg(socket, &message, 0)) <= 0)
        return res;

    /* Iterate through header to find if there is a file descriptor */
    for(control_message = CMSG_FIRSTHDR(&message);
        control_message != NULL;
        control_message = CMSG_NXTHDR(&message,
                                      control_message))
    {
        if( (control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS) )
        {
            return *((int *) CMSG_DATA(control_message));
        }
    }

    return -1;
}

static int
send_file_descriptor(int socket, int fd_to_send)
{
    struct msghdr message;
    struct iovec iov[1];
    struct cmsghdr *control_message = NULL;
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];

    memset(&message, 0, sizeof(struct msghdr));
    memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

    /* We are passing at least one byte of data so that recvmsg() will not return 0 */
    data[0] = ' ';
    iov[0].iov_base = data;
    iov[0].iov_len = sizeof(data);

    message.msg_name = NULL;
    message.msg_namelen = 0;
    message.msg_iov = iov;
    message.msg_iovlen = 1;
    message.msg_controllen =  CMSG_SPACE(sizeof(int));
    message.msg_control = ctrl_buf;

    control_message = CMSG_FIRSTHDR(&message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(int));

    *((int *) CMSG_DATA(control_message)) = fd_to_send;

    return sendmsg(socket, &message, 0);
}
