#define __USE_XOPEN2K8
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <locale.h>

int main(int argc, char ** argv)
{
    const char str1[] = "漢字入力";
    const char str2[] = "漢字だす";
    locale_t mylocale = newlocale(LC_ALL_MASK, "ja_JP.utf8", (locale_t)NULL);
    // locale_t mylocale = newlocale(LC_ALL_MASK, "en_GB.utf8", (locale_t)NULL);
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t ret1, ret2;
    char *xfrm_str1 = alloca(len1 * 2);
    char *xfrm_str2 = alloca(len2 * 2);

    ret1 = strxfrm_l(xfrm_str1, str1, len1 * 2, mylocale);
    ret2 = strxfrm_l(xfrm_str2, str2, len2 * 2, mylocale);

    printf("len1 = %lu ret1 = %lu\n", (unsigned long)len1, (unsigned long)ret1);
    printf("len2 = %lu ret2 = %lu\n", (unsigned long)len2, (unsigned long)ret2);
    
    int ret = strcmp(xfrm_str1, xfrm_str2);

    printf("string compare: %s %s %s\n",
           str1,
           ((ret == 0) ? "==" : ((ret > 0) ? ">" : "<")),
           str2);

    int i;
    printf("Orig: ");
    for (i=0 ; str1[i] ; i++)
        printf("%02x ", (unsigned char)str1[i]); 
    printf("\n");
    printf("Xfrm: ");
    for (i=0 ; xfrm_str1[i] ; i++)
        printf("%02x ", (unsigned char)xfrm_str1[i]); 
    printf("\n");
        
    return 0;
}
