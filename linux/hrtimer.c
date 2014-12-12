/*
 *  setitimer �����ٻ
 *
 *  Copyright (C) NAKAMURA Minoru <nminoru@nminoru.jp>
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



#define rdtsc()                                                     \
    ({uint64_t high, low;                                           \
    __asm__ volatile ("lfence\n rdtsc" : "=a" (low), "=d" (high) : : "memory");   \
    ((high << 32) | low);})

#define CLOCK_REALTIME (0)


enum {
    REPEAT     = 10000,
    WORK_CPUID = 1,
    DURATION   = 100 // micro second
};

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

    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    // setitimer �η�̤Υ����ʥ�� sigwaitinfo �ǵۤ��夲
    // ��Τ� SIGALRM �ϥ֥�å����롣
    sigprocmask(SIG_BLOCK, &sigmask, NULL) ;

    struct itimerval time;

    time.it_interval.tv_sec  = 0;
    time.it_interval.tv_usec = 0;
    time.it_value.tv_sec     = 0;
    time.it_value.tv_usec    = DURATION;

    uint64_t account = 0;
    for (i=0 ; i<REPEAT ; i++) {
        struct timespec before; 
        clock_gettime(CLOCK_REALTIME, &before);
        
        if (setitimer(ITIMER_REAL, &time, NULL) < 0) {
            perror("setitime");
            exit(EXIT_FAILURE);
        }

        int ret;
        {
            siginfo_t info;
            ret = sigwaitinfo(&sigmask, &info);
        } while(ret != SIGALRM);

        struct timespec after;
        clock_gettime(CLOCK_REALTIME, &after);

        account += (after.tv_sec - before.tv_sec) * 1000000000 + (after.tv_nsec - before.tv_nsec);
    }

    // ���ɽ��
    printf("Result: setitimer overrun %.3f (usec)\n", 1.0 / 1000 * account / REPEAT - DURATION);

    return 0;
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
