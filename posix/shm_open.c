/*
 *  shm_open()、shm_unlink() のテスト
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define TEMPFILE_SIZE (0x1000000)

int main(int argc, char **argv)
{
    int fd, ret;
    void *addr;
    char tempfile[NAME_MAX];

    snprintf(tempfile, sizeof(tempfile), "/shm-test-%d", getpid());

    fd = shm_open(tempfile, O_RDWR|O_CREAT, 0644);

    ret = ftruncate(fd, (off_t)TEMPFILE_SIZE);

    addr = mmap(NULL, TEMPFILE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    sleep(100);

    shm_unlink(tempfile);

    return 0;
}
