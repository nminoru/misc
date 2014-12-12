#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>

/*
  > g++ StackOverflowHandling_linux.cpp linux_stack_info.cpp -ldl -lpthread
 */


/* �����ƥ�β��ۥ���Υ����� (��Ĵ��) */
#define VM_PAGE_SIZE            (4096)

/* �����ɥڡ����Υ����� (��Ĵ��) */
#define SYSTEM_GUARD_PAGE_SIZE  (VM_PAGE_SIZE)

/* ���إ����ʥ륹���å����礭�� (���ۥ���Υڡ����������������ܤ�) */
#define ALT_STACK_SIZE (4096*8)


extern void get_linux_stack_info(void** stackaddr, size_t* stacksize);

static void set_signal_handler() ;
static void signal_handler(int sig, siginfo_t* sig_info, void* sig_data);
static void set_alternative_signal_stack(char* stackaddr, size_t stacksize);
static void cause_stack_overflow();


/* ���إ����ʥ륹���å����ΰ����¸ */
static char* start_alternative_stack_addr;
static char* end_alternative_stack_addr;

/* �����å������С��ե���ϥ�ɥ�󥰤����ΰ����¸ */
static char* start_new_guard;
static char* end_new_guard;


/*
  ���Υץ����Ǥϥ����å����ɥ쥹�β�Ⱦʬ��ϥ�ɥ�󥰤��롣
  
  +-------------------+
  |                  |
  |                  |
  |                  |
  |                  |
  |                  |
  +------------------+
  |                  |
  |                  |
  |                  |
  +------------------+
  | Glibc gurad page |
  +------------------+

 */
int main(int argc, char** argv) 
{

  char*  stackaddr;
  size_t stacksize;
  get_linux_stack_info((void**)&stackaddr, &stacksize);


  /* ���إ����ʥ륹���å��Υ��å� */
  start_alternative_stack_addr = stackaddr + SYSTEM_GUARD_PAGE_SIZE;
  end_alternative_stack_addr   = start_alternative_stack_addr + ALT_STACK_SIZE;

  if (mmap(start_alternative_stack_addr, ALT_STACK_SIZE,
           PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) == (void*)-1){
    fprintf(stderr, "faield mmap\n");
    fflush(stderr);
    exit(1);    
  }
  set_alternative_signal_stack(start_alternative_stack_addr, ALT_STACK_SIZE);
  
  /* �����ʥ�ϥ�ɥ�Υ��å� */
  set_signal_handler();

  printf( "Stack region: [%p, %p)\n", stackaddr, stackaddr + stacksize);

  // Linux
  start_new_guard = stackaddr + SYSTEM_GUARD_PAGE_SIZE + ALT_STACK_SIZE;  
  end_new_guard   = stackaddr + (stacksize / 2);

  start_new_guard = (char*)(((intptr_t)start_new_guard + VM_PAGE_SIZE -1) & (~(VM_PAGE_SIZE - 1)));
  end_new_guard   = (char*)(((intptr_t)end_new_guard) & (~(VM_PAGE_SIZE - 1)));


  if (mmap(start_new_guard, end_new_guard - start_new_guard,
           PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) == (void*)-1 ){
    fprintf(stderr, "faield mmap\n");
    fflush(stderr);
    exit(1);    
  }

  printf( "New guard area: [%p, %p)\n", start_new_guard, end_new_guard);


  /* �虜�ȥ����å������С��ե���ȯ�������� */
  cause_stack_overflow();

  return 0;
}


/* �����ʥ�ϥ�ɥ� */
static void signal_handler(int sig, siginfo_t* sig_info, void* sig_data) {
  if (sig == SIGSEGV &&  sig_info->si_code == SEGV_ACCERR) {
    char* address = (char*)sig_info->si_addr;

    /* �����ɥ��ꥢ���ȯ������ SIGSEGV �� */
    if (start_new_guard <= address & address < end_new_guard) {
      /* �����å������С��ե���ϥ�ɥ�� */

      if (address < start_new_guard + VM_PAGE_SIZE) {
        /* �Ǹ�� 1�ڡ����ʤΤǥ��顼��å�������Ф������ */
        fprintf(stderr, "Stack overflow!!\n"); 
        fflush(stderr);
        exit(1);
        
      } else {
        /* �̾� */
        char* start = (char*)(((intptr_t)address) & (~(VM_PAGE_SIZE - 1)));
        if (mprotect(start, VM_PAGE_SIZE, PROT_READ|PROT_WRITE) == -1) {
          fprintf(stderr, "faield mprotect\n");
          fflush(stderr);
          exit(1);    
        }

        printf("New memory mapped: [%p,%p", start, start + VM_PAGE_SIZE);
        fflush(stdout);
      }
    }
  }
}


/* ���إ����ʥ�ϥ�ɥ�Υ��å� */
static void set_alternative_signal_stack(char* stackaddr, size_t stacksize)
{
  stack_t ss ;

  /* alternative signal stack �Ȥ��ƥ������� */
  ss.ss_sp    = stackaddr;
  ss.ss_size  = stacksize;
  ss.ss_flags = 0;
  if (sigaltstack(&ss, NULL)) {
    /* error */
    fprintf(stderr, "Failed to set sigaltstack\n" );
    exit(1);
  }  
}



/* �����ʥ�ϥ�ɥ�Υ��å� */
static void set_signal_handler() 
{
  struct sigaction newAct, oldAct;

  /* �Ť������ʥ�ϥ�ɥ������ */
  if (sigaction( SIGSEGV, NULL, &oldAct) == -1 ){
    /* error */
    fprintf(stderr, "Failed to acquire the current sigaction_t.\n" );
    exit(1);
  }

  /* �����������ʥ�ϥ�ɥ�򥻥å� */
  sigemptyset(&newAct.sa_mask);
  sigaddset(&newAct.sa_mask, SIGSEGV);
  newAct.sa_sigaction = signal_handler;
  newAct.sa_flags     = SA_SIGINFO|SA_RESTART|SA_ONSTACK; 
  if (sigaction( SIGSEGV, &newAct, NULL) == -1 ){
    /* error */
    fprintf(stderr, "Failed to set my signal handler.\n" );
    exit(1);
  }
}


/* �虜�ȥ����å������С��ե���ȯ�������� */
static void cause_stack_overflow() {
  cause_stack_overflow();
}
