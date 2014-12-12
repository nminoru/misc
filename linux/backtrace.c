#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static FILE *output;

__attribute__((constructor))
static void init(void)
{
    output = fopen("/tmp/backtrace.log", "w");
    if (output == NULL)
        abort();
}

void print_backtrace(void)
{
    int j, nptrs;
#define SIZE 100
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    fprintf(output, "backtrace() returned %d addresses\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        fprintf(output, "%s\n", strings[j]);

    free(strings);

    fprintf(output, "\n");
}
