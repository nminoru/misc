#define __USE_XOPEN2K8
#include <stdio.h>
#include <string.h>
#include <locale.h>

int main(int argc, char ** argv)
{
    const char str1[] = "漢字入力";
    const char str2[] = "漢字だす";

    locale_t mylocale = newlocale(LC_ALL_MASK, "ja_JP.utf8", (locale_t)NULL);

    int ret = strcoll_l(str1, str2, mylocale);

    printf("string compare: %s %s %s\n",
           str1,
           ((ret == 0) ? "==" : ((ret > 0) ? ">" : "<")),
           str2);
        
    return 0;
}
