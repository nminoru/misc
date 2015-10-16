#include <stdio.h>

static void
f(void)
{
    printf("call f()\n");
}

void g(void) __attribute__ ((alias ("f")));
