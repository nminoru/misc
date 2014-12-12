#include <stdio.h>
#include <sys/time.h>

int global_value = 0;

int main(int argc, char** argv) 
{
  int i,j;
  struct timeval time1, time2;


  gettimeofday(&time1, NULL);
  for(i=1 ; i<10000 ; i++){
    for(j=1 ; j<10000 ; j++){
      global_value += i * j ;
    }    
  }
  gettimeofday(&time2, NULL);

  {
    double diff = time2.tv_sec - time1.tv_sec + 
      ( time2.tv_usec - time1.tv_usec ) / 1000000.0;

    printf( "tv_sec diff:  %f\n",  diff);
  }
    
  return  0;
}
