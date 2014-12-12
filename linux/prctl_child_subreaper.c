/*
 *  gcc -Wall -I /mnt/sdb1/linux-3.7.7/usr/include prctl_test.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

static void sigchld_handler(int sig, siginfo_t * info, void * p)
{
    int status;
    pid_t pid;

    do {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0) {
            printf("SIGCHLD: %d -> %d\n", getpid(), pid);
        }
    } while (pid > 0);
}

static void foo(int i)
{
    if (i == 0)
        return;

    pid_t child = fork();

    if (child == 0) {
#if 0
        int ret;
        ret = prctl(PR_SET_CHILD_SUBREAPER, 1);
        if (ret < 0) {
            fprintf(stderr, "prctl: errno=%d\n", errno);
            exit(EXIT_FAILURE);
        }
#endif
        printf("Child: %d\n", getpid());
        foo(i - 1);
    }

    sleep(i);
}

int main(int argc, char **argv)
{
    int ret;

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigchld_handler;

    ret = sigaction(SIGCHLD, &act, NULL);
    if (ret < 0) {
        fprintf(stderr, "sigactio: errno=%d\n", errno);
        exit(EXIT_FAILURE);
    }

    ret = prctl(PR_SET_CHILD_SUBREAPER, 1);
    if (ret < 0) {
        fprintf(stderr, "prctl: errno=%d\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("Pid: %d\n", getpid());

    pid_t child = fork();

    if (child == 0) {
#if 0
        prctl(PR_SET_CHILD_SUBREAPER, 0);
#endif
        printf("Child: %d\n", getpid());
        foo(3);
        printf("die %d\n", getpid());
        exit(0);
    }

    for (;;) {
        sleep(10);
    }

    return 0;
}
