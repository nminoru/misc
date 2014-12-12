/************************************************
  Written by NAKAMURA Minoru, 2004.03.12 
 ************************************************/

#define _GNU_SOURCE

#include <sys/resource.h>
#include <unistd.h>
#include <dlfcn.h>
#include <assert.h>

typedef int       (*PTHR_GETATTR_NP_FUNC)(pthread_t, pthread_attr_t *);
typedef pthread_t (*PTHR_SELF_FUNC)(void);
typedef int       (*PTHR_ATTR_GETSTACKBASE_FUNC)(pthread_attr_t *, void**);
typedef int       (*PTHR_ATTR_GETSTACKSIZE_FUNC)(pthread_attr_t *, size_t*);

PTHR_GETATTR_NP_FUNC         pthr_getattr_np_func;
PTHR_SELF_FUNC               pthr_self_func;            
PTHR_ATTR_GETSTACKBASE_FUNC  pthr_attr_getstackaddr_func; 
PTHR_ATTR_GETSTACKSIZE_FUNC  pthr_attr_getstacksize_func;
      

#define INITIAL_THREAD_STACK_END ((char*)0xC0000000)


void get_linux_stack_info(void** stackaddr, size_t* stacksize) {
  char dummy;
  char* p = &dummy;

  struct rlimit rlim;
  getrlimit(RLIMIT_STACK, &rlim);
  size_t default_stack_size = (size_t)rlim.rlim_cur ;

  if( INITIAL_THREAD_STACK_END - default_stack_size <= p && p <=  INITIAL_THREAD_STACK_END ){
    *stackaddr = (void*)(INITIAL_THREAD_STACK_END - default_stack_size);
    *stacksize = default_stack_size;
  } else {
    pthr_getattr_np_func = (PTHR_GETATTR_NP_FUNC) dlsym( NULL, "pthread_getattr_np" );  

    if( pthr_getattr_np_func ){
      *stackaddr = (void*)( (size_t)p & (2 * 1024 * 1024 - 1));
      *stacksize = 2 * 1024 * 1024;
    } else {
      pthr_self_func = (PTHR_SELF_FUNC) dlsym(NULL, "pthread_self" ); 
      assert( pthr_self_func );

      pthread_attr_t attr;
      if( pthr_getattr_np_func(pthr_self_func(), &attr) ){
        pthr_attr_getstackaddr_func = (PTHR_ATTR_GETSTACKBASE_FUNC) dlsym(NULL, "pthread_attr_getaddr" );
        pthr_attr_getstacksize_func = (PTHR_ATTR_GETSTACKSIZE_FUNC) dlsym(NULL, "pthread_attr_getsize" ); 

        assert( pthr_attr_getstackaddr_func && pthr_attr_getstacksize_func );

        int ret ;
        ret = pthr_attr_getstackaddr_func( &attr, stackaddr );
        assert( ret == 0 );

        ret = pthr_attr_getstacksize_func( &attr, stacksize );
        assert( ret == 0 );
      }
    }
  }
}



#if 0

/* for test */

#include <pthread.h>
#include <stdio.h>

static void* thread_routine(void*) {
  void*  stackbase ;
  size_t stacksize ;

  get_linux_stack_info( &stackbase, &stacksize );

  printf("base = %p size = %uK\n", stackbase, stacksize / 1024 );
}

int main(int argc, char** argv) {
  pthread_t pid;
  pthread_create( &pid, NULL, thread_routine, NULL );

  void* data;
  pthread_join( pid, &data);

}
#endif
