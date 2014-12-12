/*
 *  inotify.c �Υ���ץ�
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
     *  �ƻ��оݤȤ���ǥ��쥯�ȥ����ꡣ
     *  �ƻ��оݥǥ��쥯�ȥ��ʣ�����ꤹ�뤳�Ȥ��ǽ��
     */
    int wd = inotify_add_watch(fd, argv[1], 
                               IN_ALL_EVENTS /* ���ƤΥ��٥�Ȥ�ƻ� */);

    if (wd < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        int i, aux=0, ret;
        char name_buf[PATH_MAX];
        char buffer[65536];

        /*
         *  - fd �����ʣ���Υ��٥�Ȥ�Ʊ���˼����Ǥ��롣
         *  - �ƻ��оݤΥǥ��쥯�ȥ꤬ʣ��������Ϥ��ä���Хåե���
         *    �������Ƥ��顢�桼��¦�Ƕ��̤��롣
         *  - �Хåե��������Ϥ��������礭���ʤ��ȥ��顼�Ȥʤ롣
         *    sizeof(struct inotify_event) ����ꤷ��1���٥�ȤŤĺΤ�
         *    �Τ�̵����
         *  - �Хåե����������礭���Ƥ�������� inotify ���٥�Ȥ��ʤ�
         *    �ʤ���֤äƤ��롣
         *  - �Ǹ�Υ��٥�Ȥ�����Ⱦü�ʾ��֤ޤ��ɤ߹��ޤ�뤳�Ȥ����롣
         *    ���ξ��ϥ��٥�Ȥθ�Ⱦ��ʬ���ɤ�ľ��ɬ�פ����롣
         *    �ץ������Ǥ� aux ��Ȥä�Ĵ�����Ƥ��롣
         */
    reread:
        /*  buffer ����Ƭ���� aux �ޤǤϤ��Ǥ��ɤ߹���Ǥ��롣 */
        ret = read(fd, buffer + aux, sizeof(buffer) - aux);
        if (ret == -1) {
            if (ret == -EINTR)
                goto reread;
            perror("read");
            exit(EXIT_FAILURE);
        }
        ret += aux;

        /*
         *  �Хåե����������礭���Τ˺ǽ�Υ��٥�Ȥ�����Ⱦü�ˤ���
         *  �ɤ�ʤ��ʤ饨�顼�Ȥ��롣
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
                /* inotify ���٥�Ȥθ�����ʬ���ɤ߹���Ƥ��ʤ��� */
                aux = ret - i;
                memmove(buffer, buffer + i, aux);
                goto reread;
            }

            int size = sizeof(struct inotify_event) + inotify_p->len;
            if (ret < i + size) {
                /* inotify ���٥�Ȥβ�����ʬ���ɤ߹���Ƥ��ʤ��� */
                aux = ret - i;
                memmove(buffer, buffer + i, aux);
                goto reread;                
            }

            /* ���٥�Ȥβ��� */
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
