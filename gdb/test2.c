#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void recursive_call(int i)
{
    if (i > 0)
        recursive_call(i-1);
    else {
        printf("stop\n");
        raise(SIGSEGV);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }

    recursive_call(atoi(argv[1]));

    return 0;
}
