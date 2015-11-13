/**
 * 指定のサイズの疎なファイルを作成し mmap() した後、各ページをタッチして実体化させるテスト
 *
 * 容量制限のあるファイルシステム上で容量を枯渇した後に何が起きるかをチェックする。
 * (SIGBUS が発生するはず)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    size_t page_size;
    const char *filename;
    size_t filesize;
    int ret;

    page_size = sysconf(_SC_PAGESIZE);

    if (argc < 3)
    {
        printf("usage: filename size\n");
        exit(EXIT_SUCCESS);
    }
        
    filename = argv[1];
    filesize = (size_t) atol(argv[2]);

    int fd;
    fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        fprintf(stderr, "cannot open with O_CREAT: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    /* file を filesize に拡張する。最後の 1 バイトだけダミーデータを書き込む */
    ret = ftruncate(fd, (off_t) filesize - 1);
    if (ret < 0)
    {
        fprintf(stderr, "failed to execute ftruncate()\n");
        exit(EXIT_FAILURE);
    }

    char dummy;
    write(fd, &dummy, 1);
    
    fsync(fd);

    char *addr, *end;

    addr = mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void *) addr == MAP_FAILED)
    {
        fprintf(stderr, "failed to execute mmap()\n");
        exit(EXIT_FAILURE);
    }

    end = addr + filesize;

    for (; addr < end ; addr += page_size)
    {
        *addr = 0;
    }

    close(fd);

    return 0;
}
