#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipefd[2];

char message[] = "1234567890";

static void read_pipe(int fd)
{
    int ret;

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    ret = read(fd, buffer, strlen(message));
    if (ret < 0) {
        perror("read");
        exit(EXIT_FAILURE);            
    }
    printf("Child or grand child(%d): read %d, \"%s\"\n", getpid(), ret, buffer);
}

int main(int argc, char **argv)
{
    int ret;

    if (pipe2(pipefd, O_NONBLOCK) < 0) {
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid > 0) {
        printf("Pid: %d\n", pid);
        ret = write(pipefd[1], message, strlen(message));
        if (ret < 0) {
            perror("write");
            exit(EXIT_FAILURE);            
        }
        
        printf("Parent: write %d\n", ret);

        ret = waitpid(pid, NULL, 0);

    } else {
        printf("Pid: child %d\n", getpid());

#if 1
        pid_t pid2 = fork();

        if (pid2 > 0) {
            // printf("Pid2: child %d\n", pid2);

            read_pipe(pipefd[0]);

            ret = waitpid(pid2, NULL, 0);
            exit(EXIT_SUCCESS);
        }
#endif

        read_pipe(pipefd[0]);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
