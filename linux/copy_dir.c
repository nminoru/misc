#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <dirent.h>
#include <attr/xattr.h>
#include <assert.h>

       

enum {
    BUFFER_SIZE = 32768
};

static void copy_directory(const char *src_dir, const char *dest_dir);
static void copy_file(int fd_src, int fd_dest);
static void copy_xattr(int fd_src, int fd_dest);

int main(int argc, char **argv)
{
    copy_directory(argv[1], argv[2]);

    return 0;
}

static void copy_directory(const char *src_dir, const char *dest_dir)
{
    DIR *dir = opendir(src_dir);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent entry;
    struct dirent *result;
    while(readdir_r(dir, &entry, &result) == 0 && result != NULL) {
        if (strcmp(result->d_name, ".") == 0)
            continue;
        if (strcmp(result->d_name, "..") == 0)
            continue;
        
        char in_buf[PATH_MAX];
        char out_buf[PATH_MAX];
        sprintf(in_buf, "%s/%s", src_dir, result->d_name);
        sprintf(out_buf, "%s/%s", dest_dir, result->d_name);

        int fd_src, fd_dest;
        fd_src  = open(in_buf, O_RDONLY|O_NOATIME);
        if (fd_src < 0) {
            perror("open: fd_src");
            exit(EXIT_FAILURE);
        }
        
        fd_dest = open(out_buf, O_CREAT|O_WRONLY|O_TRUNC, 0600);
        if (fd_dest < 0) {
            perror("open: fd_dest");
            exit(EXIT_FAILURE);
        }

        copy_file(fd_src, fd_dest);

        close(fd_dest);
        close(fd_src);
    }

    closedir(dir);
}

static void copy_file(int fd_src, int fd_dest)
{
    int ret;
    char buffer[BUFFER_SIZE];
    struct stat buf;

    ret = fstat(fd_src, &buf);
    if (ret <  0) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    ret = fchown(fd_dest, buf.st_uid, buf.st_gid);
    if (ret < 0) {
        perror("fchown");
        exit(EXIT_FAILURE);
    }

    ret = fchmod(fd_dest, buf.st_mode);
    if (ret < 0) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }

    /* data */
    /* lstat */
    ssize_t rsize, wsize;

    do {
    read_retry:
        rsize = read(fd_src, buffer, BUFFER_SIZE);
        if (rsize == (ssize_t)-1) {
            int eno = errno;
            if (eno == EINTR || eno == EAGAIN || eno == EWOULDBLOCK)
                goto read_retry;
        }

    write_retry:
        wsize = write(fd_dest, buffer, rsize);
        if (wsize == (ssize_t)-1) {
            int eno = errno;
            if (eno == EINTR || eno == EAGAIN || eno == EWOULDBLOCK)
                goto write_retry;
        }
    } while (rsize > 0);

    /* xattr */
    copy_xattr(fd_src, fd_dest);

    /* time */
    struct timespec ts[2];
    ts[0].tv_sec  = buf.st_atime;
    ts[0].tv_nsec = 0;
    ts[1].tv_sec  = buf.st_mtime;
    ts[1].tv_nsec = 0;
    ret = futimens(fd_dest, ts);
    if (ret < 0) {
        perror("futimes");
        exit(EXIT_FAILURE);
    }
}


static void copy_xattr(int fd_src, int fd_dest)
{
    ssize_t ret;
    ret = flistxattr(fd_src, NULL, 0);
    if (ret == (ssize_t)-1) {
        perror("flistxattr");
        exit(EXIT_FAILURE);
    }

    char *buf = (char*)malloc(ret);
    assert(buf != NULL);
    ret = flistxattr(fd_src, buf, ret);
    if (ret == (ssize_t)-1) {
        perror("flistxattr");
        exit(EXIT_FAILURE);
    }

    int i;
    ssize_t maxlen = ret;
    for (i=0; i<maxlen ; ) {
        ssize_t ret;
        int len = strlen(buf + i);

        ret = fgetxattr(fd_src, buf + i, NULL, 0);
        if (ret == (ssize_t)-1) {
            perror("fgetxattr1");
            exit(EXIT_FAILURE);
        }

        void *xattr_val = (char*)malloc(ret);
        ret = fgetxattr(fd_src, buf + i, xattr_val, ret);
        if (ret == (ssize_t)-1) {
            perror("fgetxattr2");
            exit(EXIT_FAILURE);
        }

        ret = fsetxattr(fd_dest, buf + i, xattr_val, ret, 0);
        if (ret == (ssize_t)-1) {
            perror("fsetxattr");
            exit(EXIT_FAILURE);
        }

        free(xattr_val);

        i += len + 1;
    }

    free(buf);
}
