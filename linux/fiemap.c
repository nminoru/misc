/**
 *  FIEMAP ioctl test
 *
 *  gcc -Wall -g fiemap.c -o fiemap
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <linux/fiemap.h>

enum {
    MAX_EXTENT = 64
};


int main(int argc, char **argv)
{
    struct stat     statinfo;
    int             fd;
    int             i;

    if (argc != 2) {
        fprintf(stderr, "Syntax: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &statinfo) < 0) {
        fprintf(stderr, "Cannot stat %s\n", argv[1]);
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    struct fiemap *p_filemap =
        (struct fiemap *)malloc(offsetof(struct fiemap, fm_extents[0]) + sizeof(struct fiemap_extent) * MAX_EXTENT);

    assert(p_filemap);

    int count = 0;
    __u64 start = 0, end = statinfo.st_size;

    while (start < end) {
        p_filemap->fm_start  = start;
        p_filemap->fm_length = end;
        p_filemap->fm_flags  = FIEMAP_FLAG_SYNC;
        p_filemap->fm_extent_count = MAX_EXTENT;

        if (ioctl(fd, FS_IOC_FIEMAP, p_filemap)) {
            printf("ioctl failed: %s\n", strerror(errno));
        }

        printf("FILE: # of extents=%u, flags=%x\n",
               p_filemap->fm_mapped_extents, p_filemap->fm_flags);

        __u64 last = start;
        for (i=0; i<p_filemap->fm_mapped_extents; i++) {
            struct fiemap_extent extent;
            extent = p_filemap->fm_extents[i];

            printf("  Extent: %5d logical=%11llu, phy=%13llu, len=%9llu, flags=%x\n", count,
                   (unsigned long long)extent.fe_logical,
                   (unsigned long long)extent.fe_physical,
                   (unsigned long long)extent.fe_length,
                   extent.fe_flags);

            count++;
            last = extent.fe_logical + extent.fe_length;
        }
        start = last;
    }

    free(p_filemap);

    close(fd);

    exit(EXIT_SUCCESS);
}
