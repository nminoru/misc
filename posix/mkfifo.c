#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static void do_child(void)
{
    int fd;

    if ((fd = open("named-pipe", O_RDWR)) < 0) {
        int eno = errno;
        char buffer[256];
        strerror_r(eno, buffer, sizeof(buffer));
        fprintf(stderr, "CHILD: open errno=%d(%s)\n", eno, buffer);
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    read(fd, buffer, 256);
    puts(buffer);

    write(fd, "MOOOOO\n", sizeof("MOOOOO\n"));

    sleep(1);

    close(fd);
}


int main(int argc, char **argv)
{
    if (mkfifo("named-pipe", S_IRUSR|S_IWUSR)) {
        int eno = errno;
        char buffer[256];
        strerror_r(eno, buffer, sizeof(buffer));
        fprintf(stderr, "PARENT: mkfifo errno=%d(%s)\n", eno, buffer);
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();

    if (child == 0) {
        do_child();
        exit(EXIT_SUCCESS);
    }

    int fd;

    if ((fd = open("named-pipe", O_RDWR)) < 0) {
        int eno = errno;
        char buffer[256];
        strerror_r(eno, buffer, sizeof(buffer));
        fprintf(stderr, "PARENT: open errno=%d(%s)\n", eno, buffer);
        exit(EXIT_FAILURE);
    }

    write(fd, "FOOOOO\n", sizeof("FOOOOO\n"));

    sleep(1);

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int ret = read(fd, buffer, 256);
    printf("ret = %d, %s\n", ret, buffer);

    sleep(1);

    printf("END\n");

    close(fd);

    unlink("named-pipe");

    return 0;
}
