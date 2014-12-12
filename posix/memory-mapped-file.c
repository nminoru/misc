/*
 *  memory-mmaped file はメモリ上のデータが即時反映するか？
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define TEMPFILE_SIZE (0x1000000)

static void *do_mmap(const char* filename);
static void do_test(void *addr, int server);
static void *shm_addr;


int main(int argc, char **argv)
{
    int ret, fd;
    char tempfile[] = "testXXXXXX";

    shm_addr = mmap(NULL, TEMPFILE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (shm_addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("shm_addr = %p\n", shm_addr);

    fd = mkstemp(tempfile);

    if (fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }

    ret = ftruncate(fd, (off_t)TEMPFILE_SIZE);

    close(fd);

    pid_t pid = fork();

    if (pid > 0) {
        void *addr;

        addr = do_mmap(tempfile);

        sleep(2);
        do_test(addr, 0);
        sleep(1);

        ret = waitpid(pid, NULL, 0);
    } else {
        void *addr;

        addr = do_mmap(tempfile);

        sleep(1);
        do_test(addr, 1);
        sleep(2);

        exit(EXIT_SUCCESS);
    }

    unlink(tempfile);

    return 0;
}


static void *do_mmap(const char* filename)
{
    int fd;
    void *addr;

    fd = open(filename, O_RDWR);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    addr = mmap(shm_addr, TEMPFILE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, 0);
    if (addr != shm_addr) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return addr;
}


static void do_test(void *addr, int writer)
{
    int i;
    int *map_array = (int*)addr;

    if (writer) {
        for (i=0 ; i< 1000 ; i++) {
            map_array[i] = i;
        }
    } else {
        for (i=0 ; i< 1000 ; i++) {
            if (map_array[i] != i) {
                fprintf(stderr, "Don't reflect memory map\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
