// Build: gcc -ldl -lpthread AltStack.cpp 

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <dlfcn.h>

#include <sys/resource.h>
#include <unistd.h>

#define ALT_STACK_SIZE (1024*1024)
#define ALLOCA_SIZE    (1024*2)

// extern "C"  int pthread_attr_getstack(const pthread_attr_t*, void**, size_t*);

typedef int (*PTHREAD_GETATTR_NP_FUNC)(pthread_t, pthread_attr_t *);
PTHREAD_GETATTR_NP_FUNC  pthread_getattr_np_func = NULL;

static size_t initial_thread_stack_size;
static char*  initial_thread_stack_bottom;

#define INITIAL_THREAD_STACK_BASE ((char*)0xC0000000)


/* Signal Handler */
static void signal_handler(int sig, siginfo_t* sig_info, void* sig_data) {
  if( sig == SIGSEGV ) {

    pthread_attr_t attr;

    if( pthread_getattr_np_func(pthread_self(), &attr) ){
      fprintf(stderr, "Failed pthread_getattr_np.\n" );
      fflush(stderr);
      exit(2);
    }

    void*  stack_base = 0;
    size_t stack_size = 0;
    size_t guard_size = 0;

    pthread_attr_getstack(&attr, &stack_base, &stack_size);
    pthread_attr_getguardsize( &attr, &guard_size );

    fprintf(stderr, "SEGV: %p\n", sig_info->si_addr );
    fflush(stderr);
    exit(1);
  }
}



static void check_linux_thread_system(void) {
  int dummy;

  printf("current stack: %p\n", &dummy );

  pthread_getattr_np_func = (PTHREAD_GETATTR_NP_FUNC) dlsym( NULL, "pthread_getattr_np" );  

  if( pthread_getattr_np_func == NULL ){
    fprintf(stderr, "Can't resolve pthread_getattr_np.\n" );
    fflush(stderr);    
    exit(3);
  }

  struct rlimit rlim;
  getrlimit(RLIMIT_STACK, &rlim);

  printf("limit stacksize %uK %uK\n", (int)( rlim.rlim_cur / 1024), (int)( rlim.rlim_max / 1024) );
}


void* thread_start(void* data) {

  check_linux_thread_system();
  
  pthread_attr_t attr;

  if( pthread_attr_init(&attr) ){
    exit(1);
  }

  if( pthread_getattr_np_func(pthread_self(), &attr) ){
    exit(2);
  }

  void*  stack_base = 0;
  size_t stack_size = 0;
  size_t guard_size = 0;

  pthread_attr_getstack(&attr, &stack_base, &stack_size);
  pthread_attr_getguardsize( &attr, &guard_size );

  printf("Stack %p %u %u\n", stack_base, stack_size / 1024, guard_size / 1024);

#if 0
  /* Set alternative signal stack */
  stack_t ss ;

  ss.ss_sp    = malloc(ALT_STACK_SIZE);
  ss.ss_size  = ALT_STACK_SIZE;
  ss.ss_flags = 0;

  printf( "AltStack: %p %u\n", ss.ss_sp, ss.ss_size / 1024 / 1024 );

  if( sigaltstack(&ss, NULL)) {
    /* error */
    fprintf(stderr, "Failed sigaltstack\n" );
    exit(1);
  }  
#endif


  /* Set siginal handler */
  struct sigaction newAct, oldAct;

  if( sigaction( SIGSEGV, NULL, &oldAct ) == -1 ){
    /* error */
    fprintf(stderr, "Failed to acquire the current sigaction_t.\n" );
    exit(1);
  }

  sigemptyset(&newAct.sa_mask);
  sigaddset(&newAct.sa_mask, SIGSEGV);
  newAct.sa_sigaction = signal_handler;
  newAct.sa_flags     = SA_SIGINFO|SA_RESTART|SA_ONSTACK; 


  if( sigaction( SIGSEGV, &newAct, NULL ) == -1 ){
    /* error */
    fprintf(stderr, "Failed to set my signal handler.\n" );
    exit(1);
  }


  printf("Null pointer\n");
  fflush(stdout);

  /* Let's cause an access violation */
  int* p = (int*) 0x12345678;
  *p = 0;

  return 0;
}


/* main function */
int main(int argc, char** argv) {
  int para1 = 0;

  if( argc > 1 ){
    para1 = atoi(argv[1]);
  }

  switch(para1) {
  case 1:
    {
      thread_start(NULL);      
    }
    break;

  case 2:
    {
      pthread_t pid;
      pthread_create( &pid, NULL, thread_start, NULL );
      
      void* data;
      pthread_join( pid, &data);
    }
    break;

  default:
    printf("Usage: AltStack 1|2" );
    break;
  }

  return 0;
}
