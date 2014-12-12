#include <stdio.h>
#include <sys/types.h>
#include <attr/xattr.h>


int main(int argc, char **argv)
{
    int dummy = 0;

    int ret = setxattr(argv[1], "user.ceph.update", (void*)&dummy, 4, 0);    
    if (ret < 0) {
        perror("setxattr");
    }

    return 0;
}
