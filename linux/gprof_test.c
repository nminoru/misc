/**
 *  gprof をマルチスレッドプログラムに適用した時に正しい結果が得られるか？
 */
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>


static void *thread_routine1(void *p);
static void *thread_routine2(void *p);


int main(int argc, char **argv)
{
    int ret;
    pthread_t thread1, thread2;

    ret = pthread_create(&thread1, NULL, thread_routine1, NULL);
    assert(ret == 0);

    ret = pthread_create(&thread2, NULL, thread_routine2, NULL);
    assert(ret == 0);

    pthread_join(thread2, NULL);
    pthread_join(thread1, NULL);

    return 0;
}

static unsigned int random_gen1(int l)
{
    int i;
    unsigned int sum = 0;
    for (i=0 ; i<l ; i++) {
        sum += (unsigned int)lrand48();
    }
    return sum;
}

static void *thread_routine1(void *p)
{
    int i;
    unsigned int sum = 0;

    for (i=0 ; i<1024 ; i++) {
        sum += random_gen1(lrand48() % 65536);
    }
    
    pthread_exit((void*)(uintptr_t)sum);

    return NULL;
}

static unsigned int random_gen2(int l)
{
    int i;
    unsigned int sum = 0;
    for (i=0 ; i<l ; i++) {
        sum += (unsigned int)lrand48();
    }
    return sum;
}

static void *thread_routine2(void *p)
{
    int i;
    unsigned int sum = 0;

    for (i=0 ; i<1024 ; i++) {
        sum += random_gen2(lrand48() % 65536);
    }
    
    pthread_exit((void*)(uintptr_t)sum);

    return NULL;
}
