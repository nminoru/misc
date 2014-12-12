/**
 *  ランダム生成した文字列を使って、strcoll と strxfrm 後の strcmp が
 *  一致しない場合があるか確認する。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


static void generate(char *buffer, unsigned long v)
{
    int i, pos;
    char a, b;

    pos = 0;

    buffer[pos++] = 0xC0 | (v >> 6);
    buffer[pos++] = 0x80 | (v & 0x3F);
}


int main(int argc, char **argv)
{
    unsigned long i, j;

    setlocale(LC_COLLATE, "ja_JP.UTF-8");

    for (i=0 ; i < 0x7FF ; i++) {
        for (j=i ; j < 0x7FF ; j++) {
            char before01[256] = {0};
            char before02[256] = {0};

            generate(before01, i);
            generate(before02, j);

            int ret1 = strcoll(before01, before02);
            int ret2 = strcmp (before01, before02);

            if ((ret1 == 0) && (ret2 != 0)) {   
                printf("%02x%02x %02x%02x\n",
                       (unsigned char)before01[0], (unsigned char)before01[1], 
                       (unsigned char)before02[0], (unsigned char)before02[1]);
                break;
            }
        }

    }
    return EXIT_SUCCESS;
}
