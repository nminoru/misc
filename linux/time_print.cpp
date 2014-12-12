#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char** argv) 
{
  int i,j;
  struct timeval time1;

  gettimeofday(&time1, NULL);

  time_t t =  time1.tv_sec;
  struct tm* tm_info = localtime( &t );

  if( tm_info ){
    printf("%04d-%02d-%02d %02d:%02d:%02d\n", 
	   tm_info->tm_year + 1900, 
	   tm_info->tm_mon  + 1, 
	   tm_info->tm_mday, 
	   tm_info->tm_hour, 
	   tm_info->tm_min, 
	   tm_info->tm_sec );
  }

  return 0;
}
