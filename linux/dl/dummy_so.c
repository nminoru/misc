#include <stdio.h>

#include "dl_test.h"

void
bar(void)
{
    printf("start bar()\n");
    foo();
    printf("end bar()\n");
}
