#include <stdio.h>
#include <pthread.h>

static void* start_routine(void * p)
{
    return NULL;
}

static int foo(void)
{
    int ret;
    pthread_t thread;

    printf("before\n");
    
    ret = pthread_create(&thread, NULL, start_routine, NULL);

    printf("after: %d\n", ret);

    return ret;
}

int main(int arg, char **argv)
{
   foo();
    
    return 0;
}

