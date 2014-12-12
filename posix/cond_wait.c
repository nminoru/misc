#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void handler(int signum, siginfo_t *info, void *p)
{
    printf("handler: signum=%d\n", signum);
}

static void *thread_rountine(void *p)
{
    int ret;
    struct timeval now;
    struct timespec timeout;

    pthread_mutex_lock(&mutex);
    // ret = pthread_cond_wait(&cond, &mutex);
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 3;
    timeout.tv_nsec = now.tv_usec * 1000;
    ret = pthread_cond_timedwait(&cond, &mutex, &timeout);

    pthread_mutex_unlock(&mutex);

    char buf[1024];
    strerror_r(ret, buf, sizeof(buf));

    printf("ret = %s(%d)\n", buf, ret);

    return NULL;
}


int main(int argc, char **argv)
{
    struct sigaction act;

    memset(&act, 0, sizeof(act));

    act.sa_flags   = SA_SIGINFO;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, thread_rountine, NULL);

    sigset_t usr1_set;
    sigemptyset(&usr1_set);
    sigaddset(&usr1_set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &usr1_set, NULL);

    sleep(1);

    pthread_kill(thread, SIGUSR1);

    pthread_join(thread, NULL);

    return 0;
}

