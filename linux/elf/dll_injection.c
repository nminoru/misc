#include <stdlib.h>
#include <stdio.h>

#include "patch_got_entry.h"
#include "testso.h"

static void
my_testfunc(void)
{
    printf("call my_testfunc() instead of testfunc()\n"); 
}

int
main(int argc, char **argv)
{
    patch_got_entry("testfunc", my_testfunc);

    testfunc();

    return 0;
}

