/*
 *  epoll_wait �Υ����ॢ���Ȼ��֤����ٻ
 *
 *  epoll_wait �� 1 �ߥ��ä���ꤷ�����Υ����ॢ���ȤϤɤ��������Τ˵����뤫��
 *  
 *  $ gcc -g -Wall -lrt epoll_time.c -o epoll_time
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sched.h>
#include <sys/epoll.h> 
#include <sys/signalfd.h>
#include <signal.h>
#include <time.h>
#include <string.h>


enum {
    REPEAT        = 10000,
    WORK_CPUID    = 1,
    EPOLL_TIMEOUT = 1,          // miri seconds
    DURATION      = 100 * 1000, // micro seconds
    MAX_INTERVALS = 10000
};

uint32_t histogram[MAX_INTERVALS];

volatile uint8_t dummy;



static void dummy_sigaction(int signum, siginfo_t *siginfo, void *p);
static void balast();


int main(int argc, char **argv)
{
    int i;

    // ��ͥ���٤Υ��ߡ���٥ץ��������餻�롣
    balast();

    // setitimer ��ư�����ץ�������ꤹ�롣
    cpu_set_t cset;
    CPU_ZERO(&cset);
    CPU_SET(WORK_CPUID, &cset);
    if (sched_setaffinity(0, sizeof(cset), &cset) < 0) {
        perror("sched_setaffinit");
        exit(EXIT_FAILURE);
    }

    // �����ʥ�
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = dummy_sigaction;
    sigact.sa_flags   = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGALRM, &sigact, NULL);

    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);
    int sfd = signalfd(-1, &sigmask, SFD_NONBLOCK);

    struct epoll_event ev;
    ev.events   = EPOLLIN;
    ev.data.u32 = SIGALRM;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    struct itimerval time;
    time.it_interval.tv_sec  = 0;
    time.it_interval.tv_usec = 0;
    time.it_value.tv_sec     = 0;
    time.it_value.tv_usec    = DURATION;

    struct itimerval zero_time;
    zero_time.it_interval.tv_sec  = 0;
    zero_time.it_interval.tv_usec = 0;
    zero_time.it_value.tv_sec     = 0;
    zero_time.it_value.tv_usec    = 0;

    uint64_t account = 0;
    for (i=0 ; i<REPEAT ; i++) {
        if (setitimer(ITIMER_REAL, &time, NULL) < 0) {
            perror("setitime1");
            exit(EXIT_FAILURE);
        }

        struct timespec before, after; 
        clock_gettime(CLOCK_MONOTONIC, &before);
        {
            struct epoll_event events[1];

            //
            // 100 �ߥ��äΥ����ޡ������ꤷ�� 1 ̤�ߥ��äΥ����ॢ���Ȥ�ų�
            // ����Τǡ�epoll_wait �Ϥ��ʤ餺�����ॢ���Ȥ��롣
            // 
            int nfds = epoll_wait(epollfd, events, 1, EPOLL_TIMEOUT);
            if (nfds != 0) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &after);
        
        uint64_t t = (after.tv_sec - before.tv_sec) * 1000000000 + (after.tv_nsec - before.tv_nsec);

        account += t;

        if (t / 1000 < MAX_INTERVALS)
            histogram[t / 1000]++;
        else
            histogram[MAX_INTERVALS -1]++;

        if (setitimer(ITIMER_REAL, &zero_time, NULL) < 0) {
            perror("setitime2");
            exit(EXIT_FAILURE);
        }

        //
        // ��������֤���񤷤Ƽ��� epoll_wait �¹Ի��֤򤺤餹
        // Primergy RX300 S7 �� 0��5 �ߥ��ä��餤�ǤФ餱�롣
        // 
        uint32_t j, r = rand() % 1000000;
        for (j=0 ; j < r ; j++) {
            dummy++;            
        }
    }

    // ���ɽ��
    printf("Result: setitimer overrun %.3f (usec)\n", 1.0 / 1000 * account / REPEAT);

    for (i=0 ; i<MAX_INTERVALS ; i++) {
        if (histogram[i]) {
            printf(" %5d: %u\n", i, histogram[i]);
        }
    }

    return 0;
}


static void dummy_sigaction(int signum, siginfo_t *siginfo, void *p)
{
}


/**
 *  ¸�ߤ��뤹�٤Ƥ� CPU �β�����ͥ���٤Υ��ߡ��ץ��������餻�롣
 */
static void balast()
{
    int i;
    int num_processors = (int)sysconf(_SC_NPROCESSORS_CONF);
    
    if (num_processors < 0) {
        perror("sysconf(_SC_NPROCESSORS_CONF)");
        exit(EXIT_FAILURE);
    }

    for (i=0 ; i<num_processors ; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0) {
                perror("prctl(PR_SET_PDEATHSIG, SIGKILL)");
                exit(EXIT_FAILURE);
            }

            nice(+10);

            cpu_set_t cset;
            CPU_ZERO(&cset);
            CPU_SET(i, &cset);
            if (sched_setaffinity(0, sizeof(cset), &cset) < 0) {
                perror("sched_setaffinit");
                exit(EXIT_FAILURE);
            }
            
            for (;;) {
                ;
            }
        }
    }
}
