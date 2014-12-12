/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* macros */
#define N 256

/* main */
int main(int argc, char **argv)
{
    char before01[] = "スヌBーピー";
    char before02[] = "スヌーAピー";
    char after01[N] = {'\0'};
    char after02[N] = {'\0'};

    setlocale(LC_COLLATE, "ja_JP.UTF-8");

    /* 変換 */
    int d1 = strxfrm(after01, before01, N);
    int d2 = strxfrm(after02, before02, N);

    printf("d1 = %d\n", d1);
    printf("d2 = %d\n", d2);

    int i;
    for (i=0 ; i<sizeof(before01) ; i++) {
        printf("%02x ", (unsigned char)before01[i]);
    }
    printf("\n");
    for (i=0 ; i<sizeof(before02) ; i++) {
        printf("%02x ", (unsigned char)before02[i]);
    }
    printf("\n");

    for (i=0 ; i<32 ; i++) {
        printf("%02x ", (unsigned char)after01[i]);
    }
    printf("\n");
    for (i=0 ; i<32 ; i++) {
        printf("%02x ", (unsigned char)after02[i]);
    }
    printf("\n");

    /* s1 と s2 を比較する */
    if (strcmp(after01, after02) == 0) {
        printf("同じ文字列です．\n");
    } else {
        printf("異なる文字列です．\n");
    }

    return EXIT_SUCCESS;
}
