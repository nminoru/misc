#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct {
    struct {
        int etype;
        const char *target_function;
        const char *before_position;
        const char *variable;
        int count;
        int error_no;
        int msgid;
    } verify;

} global_options_t;


global_options_t global_options;

int main(int argc, char **argv)
{
    int i, save_errno;
    void* p;

    // FOOO

    for (i=0 ; i<8 ; i++) {
        p = malloc(1024);
        if (p == NULL) {
            save_errno = errno;
            printf("OK: i = %d, srv_errno = %d\n", i, save_errno);
            exit(0);
        }
    }

    printf("NG\n");

    return 0;
}

static void __attribute__((constructor))
setup(void)
{
    global_options.verify.etype           = 2;
    global_options.verify.target_function = "malloc";
    global_options.verify.before_position = "";
    global_options.verify.variable        = "p";
    global_options.verify.count           = 2;
    global_options.verify.error_no        = ENOMEM;
}
