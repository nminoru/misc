#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void myfunc3(void)
{
    print_backtrace();
}

/* "static" はシンボルを公開しないことを意味する */
static void myfunc2(void)
{
    myfunc3();
}

void myfunc(int ncalls)
{
    if (ncalls > 1)
        myfunc(ncalls - 1);
    else
        myfunc2();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s num-calls\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    myfunc(atoi(argv[1]));
    exit(EXIT_SUCCESS);
}
