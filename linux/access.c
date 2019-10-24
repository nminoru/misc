#include <stdio.h>
#include <unistd.h>

int main(int agrc, char **argv)
{
  int ret;
  const char *filename = argv[1];

  ret = access(filename, F_OK);
  if (ret == 0)
    printf("access(F_OK) ok\n");
  else
    perror("access(F_OK)");
  
  ret = access(filename, R_OK);
  if (ret == 0)
    printf("access(R_OK) ok\n");
  else
    perror("access(R_OK)");

  ret = access(filename, W_OK);
  if (ret == 0)
    printf("access(W_OK) ok\n");
  else
    perror("access(W_OK)");

  ret = access(filename, X_OK);
  if (ret == 0)
    printf("access(X_OK) ok\n");
  else
    perror("access(X_OK)");        
  
  return 0;
}
