/**
 *  epoll_wait が返すイベントの順序の調査
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h> 
#include <sys/signalfd.h>
#include <signal.h>

enum {
    MAX_EVENTS = 10
};

// int signal_array[] = {34, 35, 36, 37, 38, 39, 40, 41, 42, 43};
int signal_array[] = {34, 36, 38, 40, 42, 35, 37, 39, 41, 43};

int main(int argc, char **argv)
{
    printf("pid = %d\n", getpid());

    int epollfd = epoll_create(MAX_EVENTS);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    sigset_t mask0;
    sigemptyset(&mask0);

    int i;
    for (i=0 ; i<MAX_EVENTS ; i++)
    {
        sigset_t mask;
        int signum = signal_array[i];

        sigemptyset(&mask);
        sigaddset(&mask, signum);
        sigaddset(&mask0, signum);

        struct sigaction sigact;
        memset(&sigact, 0, sizeof(sigact));
        sigact.sa_handler = 0;
        sigact.sa_flags   = SA_SIGINFO;
        sigemptyset(&sigact.sa_mask);
        
        sigaction(signum, &sigact, NULL);

        int sfd = signalfd(-1, &mask, SFD_NONBLOCK);

        struct epoll_event ev;
        ev.events   = EPOLLIN;
        ev.data.u32 = signum;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }

        printf("fd = %d, signum = %d\n", sfd, SIGRTMIN + i);
    }

    sigprocmask(SIG_BLOCK, &mask0, NULL);

    struct epoll_event events[MAX_EVENTS];

    sleep(60);

    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }

    for (i=0 ; i<nfds ; i++) {
        printf("signum = %d, %x\n", events[i].data.u32, events[i].events);
    }

    return 0;
}
