#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <fcntl.h>
#include <alloca.h>
#include <string.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    
    ssize_t ret;
    ret = flistxattr(fd, NULL, 0);
    if (ret == (ssize_t)-1) {
        perror("flistxattr");
        exit(EXIT_FAILURE);
    }

    char *buf = (char*)malloc(ret);
    assert(buf != NULL);
    ret = flistxattr(fd, buf, ret);
    if (ret == (ssize_t)-1) {
        perror("flistxattr");
        exit(EXIT_FAILURE);
    }

    int i;
    ssize_t maxlen = ret;
    for (i=0; i<maxlen ; ) {
        ssize_t ret;
        int len = strlen(buf + i);

        ret = fgetxattr(fd, buf + i, NULL, 0);
        if (ret == (ssize_t)-1) {
            perror("fgetxattr1");
            exit(EXIT_FAILURE);
        }

        void *xattr_val = (char*)malloc(ret);
        ret = fgetxattr(fd, buf + i, xattr_val, ret);
        if (ret == (ssize_t)-1) {
            perror("fgetxattr2");
            exit(EXIT_FAILURE);
        }

        printf("%s = (%d) ", buf + i, ret);
        int j;
        for (j=0 ; j<ret ; j++) {
            printf("%02x ", *(unsigned char*)(xattr_val + j));
        }
        printf("\n");

        free(xattr_val);

        i += len + 1;
    }

    free(buf);

    close(fd);

    return 0;
}
