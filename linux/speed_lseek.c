/**
 * 複数のプロセスから同一のファイルに lseek() が大量発行された場合の性能測定
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define LSEEK_FILE_SIZE  (1024 * 1024)
#define TEST_DURATION    (60)

typedef struct {
    unsigned char start;
    unsigned char stop;
    struct {
        pid_t pid;
        unsigned long count;
    } children[CPU_SETSIZE];
} shared_area_t;

volatile shared_area_t* shared_area_p;

static unsigned long do_task(int fd, volatile unsigned char *start, volatile unsigned char *stop);

int main(int argc, char **argv)
{
    int fd;

    if (argc < 2) {
        printf("usage: temporary-file-name\n");
        exit(EXIT_SUCCESS);
    }

    /*
     * lseek() 対象の一時ファイルを作成
     */
    const char *filename = argv[1];

    fd = open(filename, O_CREAT|O_RDWR, 0600);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, LSEEK_FILE_SIZE) < 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    fsync(fd);

    unlink(filename);

    /*
     * アフィニティを確認 
     */
    int max_processors, ret;
    cpu_set_t cpu_set;

    max_processors = (int) sysconf(_SC_NPROCESSORS_CONF);

    if (sched_getaffinity(0, sizeof(cpu_set), &cpu_set) != 0) {
        perror("sched_getaffinity");
        exit(EXIT_FAILURE);
    }

    /*
     * 共有メモリを作成
     */
    void *addr;
    addr = mmap(NULL, sizeof(shared_area_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_area_p = (shared_area_t *) addr;

    memset(shared_area_p, sizeof(*shared_area_p), 0);

    /*
     * アフィニティが存在する CPU 毎に子プロセスを作成
     */
    int i;
    for (i = 0 ; i < max_processors ; i++) {
        if (CPU_ISSET(i, &cpu_set)) {
            pid_t pid;

            pid = fork();

            if (pid > 0) {
                /* parent */
                shared_area_p->children[i].pid = pid;
            } else if (pid == 0) {
                /* child */
                cpu_set_t child_cpu_set;

                CPU_ZERO_S(sizeof(child_cpu_set), &child_cpu_set);
                CPU_SET_S(i, sizeof(child_cpu_set), &child_cpu_set);

                if (sched_setaffinity(0, sizeof(child_cpu_set), &child_cpu_set)) {
                    perror("sched_setaffinity");
                    exit(EXIT_FAILURE);
                }

                shared_area_p->children[i].count = do_task(fd, &shared_area_p->start, &shared_area_p->stop);

                exit(EXIT_SUCCESS);
            } else {
                /* error */
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }
    }

    /*
     * 計測
     */
    shared_area_p->start = 1;

    ret = usleep(TEST_DURATION * 1000000);

    if (ret != 0) {
        perror("usleep");
        exit(EXIT_FAILURE);        
    }

    shared_area_p->stop = 1;

    /*
     * 子プロセスの回収と集計
     */
    unsigned long total_count = 0;

    for (i = 0 ; i < max_processors ; i++) {
        pid_t pid = shared_area_p->children[i].pid;
        
        if (pid != 0) {
            pid_t ret;
            int status;

            do {
                ret = waitpid(pid, &status, 0);
            } while (ret == 0);

            if (ret == (pid_t) -1) {
                perror("watipid");
                exit(EXIT_FAILURE);
            }

            assert(pid == ret);

            total_count += shared_area_p->children[i].count;
        }
    }

    printf("TPS: %.3f\n", (double) total_count / TEST_DURATION);

    return 0;
}

static unsigned long
do_task(int fd, volatile unsigned char *start, volatile unsigned char *stop)
{
    unsigned long count = 0;

    while (!*start)
        sched_yield();

    while (!*stop)
    {
        off_t result;

        result = lseek(fd, 0, SEEK_END);

        if (result == (off_t) -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        count++;
    }

    return count;
}
