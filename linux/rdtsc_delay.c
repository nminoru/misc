/**
 *  RDSTC 命令の投機実行効果の測定 (x86-64/Linux 用)
 *
 *  gcc -O3 -Wall -g -lpthread rdtsc_delay.c -o rdtsc_delay
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>


#define MAX_CPU_SIZE       (64)
#define SLEEP_SECONDS      (10)
#define CACHE_LINE_SIZE    (64)
#define CACHE_LINE_ALIGNED  __attribute__ ((aligned (CACHE_LINE_SIZE)))


/**
 *  アセンブラ
 */
#define rdtsc()                                                     \
    ({uint64_t high, low;                                           \
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));       \
        ((high << 32) | low);})

#define rdtsc_barrier()                                                 \
    ({uint64_t high, low;                                               \
        __asm__ volatile ("lfence\n rdtsc" : "=a" (low), "=d" (high) : : "memory"); \
        ((high << 32) | low);})

#define cmpxchg8(addr,oldValue,newValue)\
    ({uint64_t prev;                            \
        uint64_t _newValue = (uint64_t)newValue;        \
        __asm__ __volatile__("lock; cmpxchgq %1,%2"     \
                             : "=a"(prev)                               \
                             : "r"(_newValue), "m"(*(uint64_t*)(addr)), "0"(oldValue) \
                             : "memory");                               \
        prev;})



static void func1(int x);


static volatile unsigned char stop;
static unsigned char with_barrier CACHE_LINE_ALIGNED;

static volatile unsigned long global_tsc CACHE_LINE_ALIGNED;

// 統計データ
static volatile struct {
    unsigned long succ;
    unsigned long failed;
    unsigned long delay;
    unsigned long worst_delay_cycles;
} CACHE_LINE_ALIGNED statics[MAX_CPU_SIZE];


int main(int argc, char** argv) 
{
    int i;
    pthread_t threads[MAX_CPU_SIZE];

    if (argc < 2) {
        printf("Usage: [0|1]\n"
               "  0: with no barrier\n"
               "  1: with barrier\n");
        exit(0);
    }
    
    with_barrier = atoi(argv[1]);

    printf("Condition: %s\n", with_barrier ? "barrier" : "no barrier");

    // CPU を数える
    int nr_cpus = (int)sysconf(_SC_NPROCESSORS_ONLN);

    if (nr_cpus > MAX_CPU_SIZE) {
        nr_cpus = MAX_CPU_SIZE;
    }

    // スレッドの作成と実行開始
    for (i=0 ; i<nr_cpus ; i++) {
        pthread_create(&threads[i], NULL, (void *)func1,
                       (void *)(uintptr_t)i);
    }

    // 10 秒間待機
    printf("Please wait %u seconds.\n", SLEEP_SECONDS);
    sleep(SLEEP_SECONDS);

    // 子スレッドを停止させる。
    stop = 1;

    // 子スレッドの停止を待つ。
    for (i=0 ; i<nr_cpus ; i++) {
        pthread_join(threads[i], NULL);
    }

    // 結果の表示
    printf("   : %-10s %-10s %-10s  %s\n",
           "#succ", "#failed", "#delay", "delay-cycle");
    for (i=0 ; i<nr_cpus ; i++) {
        printf("CPU%u: %10lu %10lu %10lu  %f\n",
               i,
               statics[i].succ,
               statics[i].failed,
               statics[i].delay,
               1.0 * statics[i].worst_delay_cycles);
    }

    return 0;
}


static inline unsigned long rdtsc_test(void)
{
    return with_barrier ? rdtsc_barrier() : rdtsc();
}


static void func1(int x)
{
    cpu_set_t mask;
    
    CPU_ZERO(&mask);
    CPU_SET(x, &mask);

    nice(+1);

    if (sched_setaffinity(0, sizeof(mask), &mask)) {
        perror("sched_setaffinity"), exit(1);
    }

    unsigned long old;
    old = 0;

    while (!stop) {
        unsigned long cur, mem;

        cur = rdtsc_test();

        if (cur < old) {
            statics[x].delay++;
            if (old - cur > statics[x].worst_delay_cycles) {
                statics[x].worst_delay_cycles = old - cur;
            }

            do {
                cur = rdtsc_test();
            } while (cur < old);
        }

        mem = cmpxchg8(&global_tsc, old, cur);

        if (mem == old) {
            statics[x].succ++;
        } else {
            statics[x].failed++;
        }
        
        old = mem;
    }
}
