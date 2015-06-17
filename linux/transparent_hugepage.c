#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#ifndef MADV_HUGEPAGE 
#define MADV_HUGEPAGE        14    /* Worth backing with hugepages.  */
#endif

#define MEMORY_SIZE (512 * 1024 * 1024)

static void *memory_allocation(void);
static void touch_memory(void *addr);

int
main(int argc, char **argv)
{
    int ret;
    void *addr;
    char buffer[256];

    addr = memory_allocation();

    ret = madvise(addr, MEMORY_SIZE, MADV_HUGEPAGE);
    if (ret < 0)
    {
        fprintf(stderr, "failed: mmap errno=%d", errno);
        exit(EXIT_FAILURE);
    }

    printf("madivse: ret = %d\n", ret);

    touch_memory(addr);

    printf("addr = %p\n", addr);

    snprintf(buffer, sizeof(buffer), "cat /proc/%d/smaps", getpid());

    system(buffer);

    return 0;
}

static void *
memory_allocation(void)
{
    void *ret;

    ret = mmap(NULL, MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    if (ret == MAP_FAILED)
    {
        fprintf(stderr, "failed: mmap errno=%d", errno);
        exit(EXIT_FAILURE);
    }

    touch_memory(ret);

    return ret;
}

static void
touch_memory(void *addr)
{
    int i;
    char *p;

    p = (char *) addr;

    for (i = 0 ; i < 512 * 1024 * 1024 ; i += 4096)
        *p = (char) i;
}

