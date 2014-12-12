/*
 * $ gcc -ggdb coredump-test-shm.c -o coredump-test-shm
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define LENGTH       (16UL*1024*1024)
#define ADDR         (NULL)
#define SHMAT_FLAGS  (0)


static int do_server(key_t key);
static int do_client(key_t key);


int main(int argc, char **argv)
{
    key_t key;

#if 1
    key = ftok(argv[0], 1);
    if (key == (key_t)-1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
#else
    key = IPC_PRIVATE;
#endif

    if (argc == 2) {
        if (strcmp("-s", argv[1]) == 0) {
            do_server(key);
            exit(EXIT_SUCCESS);
        }

        if (strcmp("-c", argv[1]) == 0) {
            do_client(key);
            exit(EXIT_SUCCESS);
        }

        printf("%s: -s or -c\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}


static int do_server(key_t key)
{
    int shmid;
    if ((shmid = shmget(key, LENGTH,
                        IPC_CREAT | SHM_R | SHM_W)) < 0) {
        perror("shmget");
        exit(1);
    }
    printf("shmid: 0x%x\n", shmid);

    char *shmaddr;
    shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
    if (shmaddr == (char *)-1) {
        perror("Shared memory attach failure");
        shmctl(shmid, IPC_RMID, NULL);
        exit(2);
    }
    printf("shmaddr: %p\n", shmaddr);

    printf("Starting the writes:\n");
    unsigned long i;
    for (i = 0; i < LENGTH; i++) {
        shmaddr[i] = (char)(i);
        if (!(i % (1024 * 1024)))
            printf(".");
    }
    printf("\n");
    
    printf("Starting the Check...");
    for (i = 0; i < LENGTH; i++)
        if (shmaddr[i] != (char)i)
            printf("\nIndex %lu mismatched\n", i);
    printf("Done.\n");

    for (;;) {
        sleep(100);
    }

    if (shmdt((const void *)shmaddr) != 0) {
        perror("Detach failure");
        shmctl(shmid, IPC_RMID, NULL);
        exit(3);
    }

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}


static int do_client(key_t key)
{
    int shmid;
    if ((shmid = shmget(key, LENGTH,
                        SHM_R | SHM_W)) < 0) {
        perror("shmget");
        exit(1);
    }
    printf("shmid: 0x%x\n", shmid);

    char *shmaddr;
    shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
    if (shmaddr == (char *)-1) {
        perror("Shared memory attach failure");
        shmctl(shmid, IPC_RMID, NULL);
        exit(2);
    }
    printf("shmaddr: %p\n", shmaddr);

    printf("Starting the Check...");
    unsigned long i;
    for (i = 0; i < LENGTH; i++)
        if (shmaddr[i] != (char)i)
            printf("\nIndex %lu mismatched\n", i);
    printf("Done.\n");
    fflush(stdout);

    abort();

    return 0;
}
