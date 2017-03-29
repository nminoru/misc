#include <stdio.h>

#include "test.h"

int default_var = 1;
int protected_var = 1;
int hidden_var = 1;

void
call_default_func1(void)
{
    printf("var = %d, %s at %s\n", default_var, __func__, __FILE__);
    default_func();
}

void
default_func(void)
{
    printf("%s at %s\n", __func__, __FILE__);
}

void
call_protected_func1(void)
{
    printf("var = %d, %s at %s\n", protected_var, __func__, __FILE__);
    protected_func();
}

void
protected_func(void)
{
    printf("%s at %s\n", __func__, __FILE__);
}

void
call_hidden_func1(void)
{
    printf("var = %d, %s at %s\n", hidden_var, __func__, __FILE__);
    hidden_func();
}

void
hidden_func(void)
{
    printf("%s at %s\n", __func__, __FILE__);
}

void
call_internal_func1(void)
{
    printf("%s at %s\n", __func__, __FILE__);
    internal_func();
}

void
internal_func(void)
{
    printf("%s at %s\n", __func__, __FILE__);
}
