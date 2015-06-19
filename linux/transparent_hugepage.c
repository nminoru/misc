#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#ifndef MADV_HUGEPAGE 
#define MADV_HUGEPAGE        14    /* Worth backing with hugepages.  */
#endif

#define MEMORY_SIZE (512 * 1024 * 1024)

static void *memory_allocation(void);
static void touch_memory(void *addr);
static void print_smap(void *addr);
static unsigned long print_minor_faults(void);

int
main(int argc, char **argv)
{
    int ret;
    void *addr;

    addr = memory_allocation();

    ret = madvise(addr, MEMORY_SIZE, MADV_HUGEPAGE);
    if (ret < 0)
    {
        fprintf(stderr, "failed: mmap : errno=%d\n", errno);
        exit(EXIT_FAILURE);
    }

    touch_memory(addr);
    
    // print_smap(addr);

    pid_t pid;

    pid = fork();

    if (pid == 0)
    {
        printf("*** forked child ***\n");

        unsigned long before_faults;

        before_faults = print_minor_faults();

        touch_memory(addr);

        printf("minor faults = %lu\n", print_minor_faults() - before_faults);

        // print_smap(addr);
    }
    else
    {
        if (waitpid(pid, NULL, 0) < 0)
        {
            fprintf(stderr, "failed: watipid : errno=%d\n", errno);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

static void *
memory_allocation(void)
{
    void *ret;

    ret = mmap((void *) 0x7f2063400000UL, MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0);
    if (ret == MAP_FAILED)
    {
        fprintf(stderr, "failed: mmap : errno=%d\n", errno);
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
        p[i] = (char) i;
}

static void
print_smap(void *addr)
{
    FILE *file = fopen("/proc/self/smaps", "r");

    if (file == NULL)
    {
        fprintf(stderr, "failed: fopen : errno=%d\n", errno);
        exit(EXIT_FAILURE);
    }

    char addr_string[16 + 1];
    int addr_len;

    if ((uintptr_t) addr > 0xffffffffffff)
    {
        snprintf(addr_string, sizeof(addr_string), "%016lx", (unsigned long)(uintptr_t) addr);
        addr_len = 16;
    }
    else if ((uintptr_t) addr > 0xffffffff)
    {
        snprintf(addr_string, sizeof(addr_string), "%012lx", (unsigned long)(uintptr_t) addr);
        addr_len = 12;
    }
    else
    {
        snprintf(addr_string, sizeof(addr_string), "%08lx", (unsigned long)(uintptr_t) addr);
        addr_len = 8;
    }

    char buffer[256];

    int printing = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (isdigit(buffer[0]) || islower(buffer[0]))
            printing = 0;

        if (strncmp(buffer, addr_string, addr_len) == 0)
            printing = 1;

        if (printing)
            fputs(buffer, stdout);
    }

    fclose(file);
}

static unsigned long
print_minor_faults(void)
{
    unsigned long count = 0;

    FILE *file = fopen("/proc/self/stat", "r");

    if (file == NULL)
    {
        fprintf(stderr, "failed: fopen : errno=%d\n", errno);
        exit(EXIT_FAILURE);
    }

    char buffer[256];

    if (fgets(buffer, sizeof(buffer), file) != NULL)
    { 
        int ret;
        unsigned long minflt, cminflt, majflt, cmajflt;

        ret = sscanf(buffer, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %lu %lu %lu %lu",
                     &minflt, &cminflt, &majflt, &cmajflt);

        if (ret != 4)
        {
            fprintf(stderr, "cannot read /proc/self/stat\n");
            exit(EXIT_FAILURE);
        }
            
        count = minflt;
    }

    fclose(file);

    return count;
}
