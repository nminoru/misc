#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "dl_test.h"

static void call_bar(int flags);

int
main(int argc, char **argv)
{
    printf("dlopen with RTLD_NOW | RTLD_GLOBAL\n");
    call_bar(RTLD_NOW | RTLD_GLOBAL);

    printf("dlopen with RTLD_LAZY | RTLD_GLOBAL\n");
    call_bar(RTLD_LAZY | RTLD_GLOBAL);

    printf("dlopen with RTLD_LAZY\n");
    call_bar(RTLD_LAZY);
    
    return 0; 
}

static void
call_bar(int flags)
{
    void *handle;
    void (*func_p)(void);
    char *error;

    handle = dlopen("./dummy_so.so", flags);
    if (handle == NULL)
    {
        fprintf(stderr, "cannot open dl_so.so: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror(); /* clear */

    func_p = (void (*)())dlsym(handle, "bar");

    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "cannot resolve bar(): %s\n", error);
        exit(EXIT_FAILURE);
    }

    func_p();

    dlclose(handle);
}

void
foo(void)
{
    printf("foo() in main\n");
}
