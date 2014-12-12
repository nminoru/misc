#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

struct Foo {
    unsigned int a;
    unsigned int b;
    unsigned int c;
};

unsigned int Foo_sum(const struct Foo *foo)
{
    return foo->a + foo->b + foo->c;
} 

int main(int argc, char **argv)
{
    int ret;
    struct rlimit rlimit;

    /* コアダンプの可能な設定に変更する */
    ret = getrlimit(RLIMIT_CORE, &rlimit);
    if (ret) {
        exit(EXIT_FAILURE);
    }

    rlimit.rlim_cur = RLIM_INFINITY;
    ret = setrlimit(RLIMIT_CORE, &rlimit);
    if (ret) {
        exit(EXIT_FAILURE);
    }

    struct Foo foo = {1, 2, 3};

    /* コアダンプ */
    abort();

    return 0;
}
