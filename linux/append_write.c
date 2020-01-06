#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void print_offset(int fd);

int
main(int argc, char **argv)
{
    if (argc < 2) {
       printf("usage: <filename>");
       exit(EXIT_FAILURE);    
    }
    
    const char *filename = argv[1];
    char buffer[] = "ABCEDFGHIZJKLM\n";
  
    int fd = open(filename, O_RDWR | O_APPEND);
    if (fd < 0) {
        perror("open with O_APPEND");
        exit(EXIT_FAILURE);
    }

    print_offset(fd);
    
    ssize_t size = write(fd, buffer, strlen(buffer) + 1);
    if (size < 0) {
        perror("write with O_APPEND");
        exit(EXIT_FAILURE);      
    }

    printf("write %d\n", (int)size);

    print_offset(fd);    

    close(fd);
	       
    return 0;
}

static void
print_offset(int fd)
{
    off_t off = lseek(fd, 0, SEEK_CUR);
    if (off < 0) {
        perror("lseek with O_APPEND");
        exit(EXIT_FAILURE);      
    }

    printf("lseek %d\n", (int)off);
}
