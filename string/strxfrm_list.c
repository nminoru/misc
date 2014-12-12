/**
 *  strxfrm の結果を確認用。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

int main(int argc, char **argv)
{
    int i, j;

    setlocale(LC_COLLATE, "ja_JP.UTF-8");

    char str[] = {0xe6, 0x84, 0x9f, 0xe6, 0x84, 0xe6, 0x84, 0x9f };
    char buffer[256];


    strxfrm(buffer, str, 256);
    
    for (j=0 ; str[j] ; j++)
        printf("%02x ", (unsigned char)str[j]);
    printf("\n");

    for (j=0 ; buffer[j] ; j++)
        printf("%02x ", (unsigned char)buffer[j]);
    printf("\n");

    return EXIT_SUCCESS;
}
