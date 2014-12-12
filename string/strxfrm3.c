/**
 *  ランダム生成した文字列を使って、strcoll と strxfrm 後の strcmp が
 *  一致しない場合があるか確認する。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


static void generate(char *buffer)
{
    int pos;
    char a, b;

    pos = 0;

    // for (i=0 ; i<4 ; i++)
    {
        switch(2 /* (rand() % 6) + 1 */)
        {
        case 1:
            buffer[pos++] = 0x7F & rand();
            break;
        case 2:
            a = 0xC0 | (0x1F & rand()); // 110x xxxx
            if (a < 0xc2)
                a = 0xc2;
            buffer[pos++] = a;
            buffer[pos++] = 0x80 | (0x3F & rand());
            break;
        case 3:
            a = 0xE0 | (0x0F & rand()); // 1110 xxxx
            b = 0x80 | (0x3F & rand());
            if ((a & 0xF) == 0 && (b & 0x20) == 0)
                b = 0xa0;
            buffer[pos++] = a;
            buffer[pos++] = b;
            buffer[pos++] = 0x80 | (0x3F & rand());
            break;
        case 4:
            a = 0xF0 | (0x07 & rand()); // 1111 0xxx
            b = 0x80 | (0x3F & rand());
            if ((a & 0x7) == 0 && (b & 0x30) == 0)
                b = 0x90;
            buffer[pos++] = a;
            buffer[pos++] = b;
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            break;
        case 5:
            a = 0xF7 | (0x03 & rand()); // 1111 10xx
            b = 0x80 | (0x3F & rand());
            if ((a & 0x3) == 0 && (b & 0x38) == 0)
                b = 0x88;
            buffer[pos++] = a;
            buffer[pos++] = b;
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            break;
        case 6:
            a = 0xFC | (0x01 & rand()); // 1111 110x
            b = 0x80 | (0x3F & rand());
            if ((a & 0x1) == 0 && (b & 0x3C) == 0)
                b = 0x84;
            buffer[pos++] = a;
            buffer[pos++] = b;
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            buffer[pos++] = 0x80 | (0x3F & rand());
            break;
        }
    }
}


int main(int argc, char **argv)
{
    int j;

    setlocale(LC_COLLATE, "ja_JP.UTF-8");

    for (;;) {
        char before01[256] = {0};
        char before02[256] = {0};
        char after01[256];
        char after02[256];
        
        generate(before01);
        generate(before02);

        /* 変換 */
        int d1 = strxfrm(after01, before01, sizeof(after01));
        int d2 = strxfrm(after02, before02, sizeof(after02));

        int ret1 = strcoll(before01, before02);
        int ret2 = strcmp (after01,  after02 );

        if ((ret1 == 0) && strcmp(before01, before02) != 0) {
            printf("STR1: ret1 = %5d, d1 = %5d | ", ret1, d1);
            for (j=0 ; before01[j] ; j++)
                printf("%02x ", (unsigned char)before01[j]);
            printf("\n");

            printf("STR2: ret2 = %5d, d2 = %5d | ", ret2, d2);
            for (j=0 ; before02[j] ; j++)
                printf("%02x ", (unsigned char)before02[j]);
            printf("\n\n");
        }

        if ((ret1 == 0) && (ret2 == 0)) {
            continue;
        }

        if (((ret1  > 0) && (ret2  > 0)) ||
            ((ret1  < 0) && (ret2  < 0)))
            continue;

        printf("STR1: ret1 = %5d, d1 = %5d ", ret1, d1);
        for (j=0 ; before01[j] ; j++)
            printf("%02x ", (unsigned char)before01[j]);
        printf("\n");

        printf("STR2: ret2 = %5d, d2 = %5d ", ret2, d2);
        for (j=0 ; before02[j] ; j++)
            printf("%02x ", (unsigned char)before02[j]);
        printf("\n\n");

        exit(0);
    }

    return EXIT_SUCCESS;
}
