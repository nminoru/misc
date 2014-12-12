/**
 *  ランダム生成した文字列を使って、strcoll と strxfrm 後の strcmp が
 *  一致しない場合があるか確認する。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


static int comparator(const char *s1, const char *s2)
{
    int ret;
    ret = strcoll(s1, s2);
    if (ret == 0)
        ret = strcmp(s1, s2);
    return ret;
}

static int get_sign(int v)
{
    if (v == 0)
        return 0;
    if (v > 0)
        return 1;
    return -1;
}

static void generate(char *buffer)
{
    int i, pos;
    char a, b;

    pos = 0;

    // for (i=0 ; i<4 ; i++)
    {
        switch(4 /* (rand() % 6) + 1 */)
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
        char str1[256] = {0};
        char str2[256] = {0};

        generate(str1);
        generate(str2);

        /* 変換 */
        int ret1 = comparator(str1, str2);
        int ret2 = strcmp    (str1, str2);

        if (get_sign(ret1) != get_sign(ret2)) {
            char convert1[256] = {0};
            char convert2[256] = {0};
            
            strxfrm(convert1, str1, 256);
            strxfrm(convert2, str2, 256);

            printf("%d %d\n", get_sign(ret1), get_sign(ret2));
            for (j=0 ; str1[j] ; j++)
                printf("%02x ", (unsigned char)str1[j]);
            printf("\n");

            for (j=0 ; str2[j] ; j++)
                printf("%02x ", (unsigned char)str2[j]);
            printf("\n\n");

            for (j=0 ; convert1[j] ; j++)
                printf("%02x ", (unsigned char)convert1[j]);

            printf("\n");

            for (j=0 ; convert2[j] ; j++)
                printf("%02x ", (unsigned char)convert2[j]);
            printf("\n\n");

            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
