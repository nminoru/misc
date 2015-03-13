#include <stdio.h>
#include <malloc.h>
#include <string.h>

static void my_init_hook(void);
static void my_free(void *ptr, const void *caller);

static void (*old_free_hook)(void *ptr, const void *caller);

void (*__malloc_initialize_hook)(void) = my_init_hook;

static void
my_init_hook(void)
{
    old_free_hook = __free_hook;
    __free_hook = my_free;
}

static void
my_free(void *ptr, const void *caller)
{
    size_t size;

    if (!ptr)
        return;

    __free_hook = old_free_hook;

    size = malloc_usable_size(ptr);

    memset(ptr, 0xFF, size);

    free(ptr);

    __free_hook = my_free;
}

int main(int argc, char **argv)
{
    int *p;

    p = malloc(sizeof(int));

    *p = 0xCAFEBABE;

    printf("p = %p, %x\n", p, *p);

    free(p);

    printf("p = %p, %x\n", p, *p);

    return 0;
}


