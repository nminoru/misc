#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  if (argc < 1) {
    printf("usage: <filename>");
    exit(EXIT_FAILURE);    
  }
    
  const char *filename = argv[1];
  char buffer[] = "ABCEDFGHIZJKLM";
  
  int fd = open(filename, O_APPEND);
  if (fd < 0) {
    perror("open with O_APPEND");
    exit(EXIT_FAILURE);
  }

  write(fd, buffer, strlen(buffer) + 1);

  close(fd);
	       
  return 0;
}
