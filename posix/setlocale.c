/**
 *  setlocale の適用範囲(プロセス or スレッド)を調べる。
 */
#include <stdio.h>
#include <locale.h>
#include <pthread.h>


static void *thread_rountine(void *p);


int main(int argc, char **argv)
{
    setlocale(LC_COLLATE, "ja_JP.eucJP");

    pthread_t thread;
    pthread_create(&thread, NULL, thread_rountine, NULL);
    pthread_join(thread, NULL);

    printf("LC_COLLATE = %s\n", setlocale(LC_COLLATE, NULL));

    return 0;
} 


static void *thread_rountine(void *p)
{
    setlocale(LC_COLLATE, "ja_JP.UTF-8");
    
    return NULL;
}
