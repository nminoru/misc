#include <stdio.h>

#include "test.h"

int main(int argc, char **argv)
{
    call_default_func1();
    call_default_func2();

    call_protected_func1();
    call_protected_func2();

    call_hidden_func1();
    call_hidden_func2();

    call_internal_func1();
    call_internal_func2();
    
    return 0;
}
