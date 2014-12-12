#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>



int main(int argc, char **argv)
{
    int ret;
    struct rlimit rlimit;

    ret = getrlimit(RLIMIT_MEMLOCK, &rlimit);
    if (ret) {
        exit(EXIT_FAILURE);
    }

    printf("rlim_cur=%lu, rlim_max=%lu\n",
           rlimit.rlim_cur, rlimit.rlim_max);

    rlimit.rlim_cur = RLIM_INFINITY;
    rlimit.rlim_max = RLIM_INFINITY;

    ret = setrlimit(RLIMIT_MEMLOCK, &rlimit);
    if (ret) {
        perror("setrlimit(RLIMIT_MEMLOCK)");
        exit(EXIT_FAILURE);
    }

    ret = getrlimit(RLIMIT_MEMLOCK, &rlimit);
    if (ret) {
        exit(EXIT_FAILURE);
    }

    printf("rlim_cur=%lu, rlim_max=%lu\n",
           rlimit.rlim_cur, rlimit.rlim_max);

    return 0;
}
