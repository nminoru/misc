#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#if 1

#define BTRFS_IOCTL_MAGIC      0x94
#define BTRFS_IOC_TRANS_START  _IO(BTRFS_IOCTL_MAGIC, 6)
#define BTRFS_IOC_TRANS_END    _IO(BTRFS_IOCTL_MAGIC, 7)

#else

#include <linux/btrfs.h>

#endif


int main(int argc, char **argv)
{
    int fd, ret;

    fd = open("normal-file.dump", O_RDWR);
    if (fd < 0) {
        int eno = errno;
        fprintf(stderr, "open = %d\n", eno);
        exit(EXIT_FAILURE);
    }

    ret = ioctl(fd, BTRFS_IOC_TRANS_START);
    if (ret < 0) {
        int eno = errno;
        fprintf(stderr, "ioctl(fd, BTRFS_IOC_TRANS_START) = %d\n", eno);
        exit(EXIT_FAILURE);
    }

    // 適当な書き込み
    char str[] = "1234";
    ret = write(fd, str, 4);
    if (ret < 0) {
        int eno = errno;
        fprintf(stderr, "ioctl(fd, BTRFS_IOC_TRANS_START) = %d\n", eno);
        exit(EXIT_FAILURE);
    }

    ioctl(fd, BTRFS_IOC_TRANS_END);
    close(fd);

    return 0;
}
