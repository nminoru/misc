/*
 *  inotify.c のサンプル
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h> /* for offsetof */
#include <string.h> /* for memmove */
#include <errno.h>
#include <limits.h>
#include <sys/inotify.h>


static void display_inotify_event(struct inotify_event *inotify_p);


int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: [directory-to-watch]\n");
        exit(EXIT_SUCCESS);
    }

    int fd  = inotify_init();

    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    /*  
     *  監視対象とするディレクトリを指定。
     *  監視対象ディレクトリは複数設定することも可能。
     */
    int wd = inotify_add_watch(fd, argv[1], 
                               IN_ALL_EVENTS /* 全てのイベントを監視 */);

    if (wd < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        int i, aux=0, ret;
        char name_buf[PATH_MAX];
        char buffer[65536];

        /*
         *  - fd からは複数のイベントを同時に収集できる。
         *  - 監視対象のディレクトリが複数ある場合はいったんバッファに
         *    受信してから、ユーザ側で区別する。
         *  - バッファサイズはある程度大きくないとエラーとなる。
         *    sizeof(struct inotify_event) を指定して1イベントづつ採る
         *    のは無理。
         *  - バッファサイズが大きくても受信する inotify イベントがなく
         *    なれば返ってくる。
         *  - 最後のイベントは中途半端な状態まで読み込まれることがある。
         *    その場合はイベントの後半部分を読み直す必要がある。
         *    プログラム中では aux を使って調整している。
         */
    reread:
        /*  buffer の先頭から aux まではすでに読み込んでいる。 */
        ret = read(fd, buffer + aux, sizeof(buffer) - aux);
        if (ret == -1) {
            if (ret == -EINTR)
                goto reread;
            perror("read");
            exit(EXIT_FAILURE);
        }
        ret += aux;

        /*
         *  バッファサイズが大きいのに最初のイベントが中途半端にしか
         *  読めないならエラーとする。
         */
        if (ret < sizeof(struct inotify_event)) {
            fprintf(stderr, "short of red bytes\n");
            exit(EXIT_FAILURE);
        }

        i = 0;
        while (i < ret) {
            struct inotify_event *inotify_p;
            inotify_p = (struct inotify_event *)(buffer + i);
            
            if (ret < i + offsetof(struct inotify_event, name)) {
                /* inotify イベントの固定部分が読み込めていない。 */
                aux = ret - i;
                memmove(buffer, buffer + i, aux);
                goto reread;
            }

            int size = sizeof(struct inotify_event) + inotify_p->len;
            if (ret < i + size) {
                /* inotify イベントの可変部分が読み込めていない。 */
                aux = ret - i;
                memmove(buffer, buffer + i, aux);
                goto reread;                
            }

            /* イベントの解析 */
            display_inotify_event(inotify_p);
            i += size;
        }
    }

    int ret = inotify_rm_watch(fd, wd);
    if (ret == -1) {
        perror("inotify_rm_watch");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}


static void display_inotify_event(struct inotify_event *inotify_p)
{
    int ret = 0;
    char buffer[256];
    uint32_t mask = inotify_p->mask;

    // File was accessed (read) (*).
    if (mask & IN_ACCESS)
        ret += sprintf(buffer + ret, "ACCESS ");

    // Metadata   changed
    if (mask & IN_ATTRIB)
        ret += sprintf(buffer + ret, "ATTRIB ");

    // File opened for writing was closed (*).
    if (mask & IN_CLOSE_WRITE)
        ret += sprintf(buffer + ret, "CLOSE_WRITE ");

    // File not opened for writing was closed (*).
    if (mask &  IN_CLOSE_NOWRITE)
        ret += sprintf(buffer + ret, "CLOSE_NOWRITE ");

    // File/directory created in watched directory (*).
    if (mask & IN_CREATE)
        ret += sprintf(buffer + ret, "CREATE ");

    // File/directory deleted from watched directory (*).
    if (mask & IN_DELETE)
        ret += sprintf(buffer + ret, "DELETE ");

    // Watched file/directory was itself deleted.
    if (mask & IN_DELETE_SELF)
        ret += sprintf(buffer + ret, "DELETE_SELF ");

    // File was modified (*).
    if (mask & IN_MODIFY)
        ret += sprintf(buffer + ret, "MODIFY ");

    // Watched file/directory was itself moved.
    if (mask & IN_MOVE_SELF)
        ret += sprintf(buffer + ret, "MODIFY_SELF ");

    // File moved out of watched directory (*).
    if (mask & IN_MOVED_FROM)
        ret += sprintf(buffer + ret, "MOVE_FROM ");

    // File moved into watched directory (*).
    if (mask & IN_MOVED_TO)
        ret += sprintf(buffer + ret, "MOVE_TO ");
    
    // File was opened (*).
    if (mask & IN_OPEN)
        ret += sprintf(buffer + ret, "OPEN ");

    // Watch  was removed explicitly (inotify_rm_watch(2)) or
    // automatically (file was deleted, or  file  system  was
    // unmounted).
    if (mask & IN_IGNORED)
        ret += sprintf(buffer + ret, "IGNORED ");

    // Subject of this event is a directory.
    if (mask & IN_ISDIR)
        ret += sprintf(buffer + ret, "ISDIR ");

    // Event queue overflowed (wd is -1 for this event).
    if (mask & IN_Q_OVERFLOW)
        ret += sprintf(buffer + ret, "Q_OVERFLOW ");

    // File system containing watched object was unmounted.
    if (mask & IN_UNMOUNT)
        ret += sprintf(buffer + ret, "Q_UNMOUNT ");

    printf("%d %x %u %u \"%s\" [%s]\n",
           inotify_p->wd, 
           inotify_p->mask,
           inotify_p->cookie,
           inotify_p->len,
           &inotify_p->name,
           buffer);
}
