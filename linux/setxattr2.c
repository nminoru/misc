/**
 * fsetxattr
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int dummy = 0;
    int ret;

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
      perror("open");
      exit(EXIT_FAILURE);
    }

    ret = flock(fd, LOCK_EX);
    if (ret < 0) {
        perror("flock");
        exit(EXIT_FAILURE);      
    }

    ret = fsetxattr(fd, "user.ceph.update", (void*)&dummy, 4, 0);
    if (ret < 0) {
        perror("fsetxattr");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
